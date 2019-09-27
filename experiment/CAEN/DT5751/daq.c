#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <CAENDigitizer.h>
#include "cfg.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

volatile sig_atomic_t interrupted = 0;
void ctrl_c_pressed(int sig) { interrupted = 1; }

int main(int argc, char* argv[])
{
  // handle Ctrl-C
  signal(SIGINT, ctrl_c_pressed); 

  // connect to digitizer
  int dt5751;
  CAEN_DGTZ_ErrorCode err;
  err = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 0, 0, 0, &dt5751);
  if (err) {
    printf("Can't open DT5751! Abort.\n");
    return 1;
  }

  // get board info
  CAEN_DGTZ_BoardInfo_t board;
  err = CAEN_DGTZ_GetInfo(dt5751, &board);
  if (err) { 
    printf("Can't get board info! Abort.\n");
    CAEN_DGTZ_CloseDigitizer(dt5751);
    return 1;
  }
  printf("Connected to %s\n", board.ModelName);
  printf("ROC FPGA Release is %s\n", board.ROC_FirmwareRel);
  printf("AMC FPGA Release is %s\n", board.AMC_FirmwareRel);

  // load configurations
  printf("Parse %s\n", argv[1]);
  FILE *fcfg = fopen(argv[1],"r");
  RUN_CFG_t cfg;
  ParseConfigFile(fcfg, &cfg);
  fclose(fcfg);

  // global settings
  err |= CAEN_DGTZ_Reset(dt5751);
  err |= CAEN_DGTZ_SetRecordLength(dt5751,cfg.ns);
  err |= CAEN_DGTZ_SetPostTriggerSize(dt5751,cfg.post);
  err |= CAEN_DGTZ_SetMaxNumEventsBLT(dt5751,1024); // maximized
  err |= CAEN_DGTZ_SetAcquisitionMode(dt5751,CAEN_DGTZ_SW_CONTROLLED);
  err |= CAEN_DGTZ_SetChannelEnableMask(dt5751,cfg.mask);
  if (cfg.trg==EXTERNAL_TTL) {
    err |= CAEN_DGTZ_SetExtTriggerInputMode(dt5751,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
    err |= CAEN_DGTZ_SetIOLevel(dt5751,CAEN_DGTZ_IOLevel_TTL);
  } else if (cfg.trg==EXTERNAL_NIM) {
    err |= CAEN_DGTZ_SetExtTriggerInputMode(dt5751,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
    err |= CAEN_DGTZ_SetIOLevel(dt5751,CAEN_DGTZ_IOLevel_NIM);
  } else if (cfg.trg==SOFTWARE_TRG)
    err |= CAEN_DGTZ_SetSWTriggerMode(dt5751,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
  else
    err |= CAEN_DGTZ_SetChannelSelfTrigger(
	dt5751,CAEN_DGTZ_TRGMODE_ACQ_ONLY,cfg.mask);
  // configure individual channels
  int ich /* channel index */, Non=0 /* number of channels turned on */;
  for (ich=0; ich<Nch; ich++) {
    if ((cfg.mask & (1<<ich))>>ich) Non++;
    err |= CAEN_DGTZ_SetTriggerPolarity(
	dt5751,ich,CAEN_DGTZ_TriggerOnFallingEdge);
    err |= CAEN_DGTZ_SetChannelDCOffset(
	dt5751,ich,(uint32_t)cfg.offset[ich]);
    err |= CAEN_DGTZ_SetChannelTriggerThreshold(
	dt5751,ich,(uint32_t)cfg.thr[ich]);
  }
  if (err) { 
    printf("Can't configure board! Abort.\n");
    CAEN_DGTZ_CloseDigitizer(dt5751);
    return 1;
  }
  sleep(1);

  // allocate memory for data taking
  char *buffer = NULL; uint32_t bytes;
  err = CAEN_DGTZ_MallocReadoutBuffer(dt5751,&buffer,&bytes);
  if (err) {
    printf("Can't allocate memory! Abort.\n");
    CAEN_DGTZ_CloseDigitizer(dt5751);
    return 1;
  }
  printf("Allocated %d kB of memory\n",bytes/1024);

  // open output file
  cfg.run=atoi(argv[2]);
  cfg.subrun=1;
  char fname[32];
  sprintf(fname, "run_%06d.%06d",cfg.run,cfg.subrun);
  printf("Open output file %s\n", fname);
  FILE *output = fopen(fname,"wb");

  // start acquizition
  time_t t = time(NULL);
  struct tm lt = *localtime(&t);
  printf("Run %d starts at %d/%d/%d %d:%d:%d\n", cfg.run, lt.tm_year+1900,
      lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
  err = CAEN_DGTZ_SWStartAcquisition(dt5751);
  if (err) {
    printf("Can't start acquisition! Abort.\n");
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_CloseDigitizer(dt5751);
    return 1;
  }

  // write run cfg
  ConfigRunTime(&cfg);
  EVENT_HEADER_t hdr;
  hdr.size=sizeof(EVENT_HEADER_t)+sizeof(RUN_CFG_t);
  hdr.evtCnt=0; hdr.trgCnt=0; hdr.type=0;
  fwrite(&hdr,1,sizeof(EVENT_HEADER_t),output);
  fwrite(&cfg,1,sizeof(RUN_CFG_t),output);
  
  // record start time
  int now, tpre = now = cfg.tsec*1000 + cfg.tus/1000, dt, runtime=0;

  // output loop
  int nEvtTot=0, nEvtInBuf, nNeeded = 16777216, nEvtIn5min=0;
  if (argc==4) nNeeded=atoi(argv[3]);
  uint32_t bsize, fsize=0;
  while (nEvtTot<nNeeded && !interrupted) {
    // send software trigger to the board
    if (cfg.trg==SOFTWARE_TRG) CAEN_DGTZ_SendSWtrigger(dt5751);

    // read data from board
    CAEN_DGTZ_ReadMode_t rMode=CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT;
    err = CAEN_DGTZ_ReadData(dt5751,rMode,buffer,&bsize);
    if (err) {
      printf("Can't read data from board! Abort.\n");
      break;
    }
    fsize += bsize;

    // write data to file
    int i;
    CAEN_DGTZ_GetNumEvents(dt5751,buffer,bsize,&nEvtInBuf);
    for (i=0; i<nEvtInBuf; i++) {
      char *rawEvt = NULL;
      CAEN_DGTZ_EventInfo_t eventInfo;
      CAEN_DGTZ_UINT16_EVENT_t *evt = NULL;
      CAEN_DGTZ_GetEventInfo(dt5751,buffer,bsize,i,&eventInfo,&rawEvt);
      CAEN_DGTZ_DecodeEvent(dt5751,rawEvt,(void **)&evt);

      hdr.size=sizeof(EVENT_HEADER_t)+sizeof(uint16_t)*cfg.ns*Non;
      hdr.evtCnt=eventInfo.EventCounter;
      hdr.trgCnt=eventInfo.TriggerTimeTag;
      hdr.type=1; // a real event
      fwrite(&hdr,1,sizeof(EVENT_HEADER_t),output);
      for (ich=0; ich<Nch; ich++)
	if ((cfg.mask & (1<<ich))>>ich)
	  fwrite(evt->DataChannel[ich],sizeof(uint16_t),cfg.ns,output);

      CAEN_DGTZ_FreeEvent(dt5751,(void **)&evt);

      nEvtTot++; nEvtIn5min++;
      if (nEvtTot>=nNeeded) break;
    }

    // display progress every 5 seconds
    struct timeval tv;
    gettimeofday(&tv,NULL);
    now = tv.tv_sec*1000 + tv.tv_usec/1000;
    runtime = now - (cfg.tsec*1000+cfg.tus/1000);
    dt = now - tpre;
    if (dt>5000) {
      printf("-------------------------\n");
      printf("run lasts: %6.2f min\n",(float)runtime/60000.);
      printf("now event: %d\n", nEvtTot);
      printf("trg rate:  %6.2f Hz\n", (float)nEvtIn5min/(float)dt*1000.);
      printf("press Ctrl-C to stop run\n");
      tpre=now;
      nEvtIn5min=0;
    }

    // start a new sub run when previous file is over 1GB
    if (fsize>1024*1024*1024) {
      fsize=0;
      printf("\nClose output file %s\n", fname);
      ConfigRunTime(&cfg);
      hdr.size=sizeof(EVENT_HEADER_t)+sizeof(RUN_CFG_t);
      hdr.evtCnt=0; hdr.trgCnt=0; hdr.type=0;
      fwrite(&hdr,1,sizeof(EVENT_HEADER_t),output);
      fwrite(&cfg,1,sizeof(RUN_CFG_t),output);
      fclose(output);

      cfg.subrun++;
      sprintf(fname, "run_%06d.%06d",cfg.run,cfg.subrun);
      printf("Open output file %s\n", fname);
      output = fopen(fname,"wb");
      fwrite(&hdr,1,sizeof(EVENT_HEADER_t),output);
      fwrite(&cfg,1,sizeof(RUN_CFG_t),output);
    }
  }

  printf("\nClose output file %s\n", fname);
  ConfigRunTime(&cfg);
  hdr.size=sizeof(EVENT_HEADER_t)+sizeof(RUN_CFG_t);
  hdr.evtCnt=0; hdr.trgCnt=0; hdr.type=0;
  fwrite(&hdr,1,sizeof(EVENT_HEADER_t),output);
  fwrite(&cfg,1,sizeof(RUN_CFG_t),output);
  fclose(output);

  t = time(NULL);
  lt = *localtime(&t);
  printf("Run %d stops at %d/%d/%d %d:%d:%d\n", cfg.run, lt.tm_year+1900,
      lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
  CAEN_DGTZ_SWStopAcquisition(dt5751);
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  CAEN_DGTZ_CloseDigitizer(dt5751);

  return 0;
}

