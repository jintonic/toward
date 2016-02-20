#include <time.h>
#include <string.h>
#include <CAENDigitizerType.h>
#include "cfg.h"

void ConfigRunTime(RUN_CFG_t *cfg)
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  cfg->tsec = tv.tv_sec;
  cfg->tus = tv.tv_usec;
}

int ParseConfigFile(FILE *fcfg, RUN_CFG_t *cfg) 
{
  char str[1000], str1[1000];
  int i, ch=-1, val;

  cfg->ns=1024;
  cfg->post=75;
  cfg->mask=0x0;
  for (i=0; i<Nch; i++) cfg->mode[i]=CAEN_DGTZ_TRGMODE_ACQ_ONLY;
  for (i=0; i<Nch; i++) cfg->thr[i]=500;
  for (i=0; i<Nch; i++) cfg->offset[i]=0;

  while (!feof(fcfg)) {
    // read a word from the file
    int read = fscanf(fcfg, "%s", str);
    if(!read || (read == EOF) || !strlen(str)) continue;

    // skip comments
    if(str[0] == '#') {
      fgets(str, 1000, fcfg);
      continue;
    }

    // Section (COMMON or individual channel)
    if (str[0] == '[') {
      if (strstr(str, "COMMON")) {
	ch = -1;
	continue; 
      }
      sscanf(str+1, "%d", &val);
      if (val < 0 || val >= Nch) {
	printf("%s: Invalid channel number\n", str);
      } else {
	ch = val;
      }
      continue;
    }

    // Acquisition Record Length (number of samples)
    if (strstr(str, "RECORD_LENGTH")!=NULL) {
      read = fscanf(fcfg, "%d", &cfg->ns);
      continue;
    }

    // Post Trigger (percent of the acquisition window)
    if (strstr(str, "POST_TRIGGER")!=NULL) {
      read = fscanf(fcfg, "%d", &cfg->post);
      continue;
    }

    // DC offset
    if (!strcmp(str, "DC_OFFSET")) {
      read = fscanf(fcfg, "%d", &val);
      if (ch == -1)
	for(i=0; i<Nch; i++) cfg->offset[i] = val;
      else
	cfg->offset[ch] = val;
      continue;
    }

    // Threshold
    if (strstr(str, "TRIGGER_THRESHOLD")!=NULL) {
      read = fscanf(fcfg, "%d", &val);
      if (ch == -1)
	for(i=0; i<Nch; i++) cfg->thr[i] = val;
      else
	cfg->thr[ch] = val;
      continue;
    }

    // Global trigger mode
    if (strstr(str, "TRIGGER_MODE")!=NULL) {
      TRG_MODE_t tm;
      read = fscanf(fcfg, "%s", str1);
      if (strcmp(str1, "SOFTWARE")==0)
	tm = SOFTWARE_TRG;
      else if (strcmp(str1, "INTERNAL")==0)
	tm = INTERNAL_TRG;
      else if (strcmp(str1, "EXTERNAL_NIM")==0)
	tm = EXTERNAL_NIM;
      else if (strcmp(str1, "EXTERNAL_TTL")==0)
	tm = EXTERNAL_TTL;
      else {
	printf("%s: Invalid Parameter\n", str);
	continue;
      }
      cfg->trg = tm;
    }

    // Channel trigger mode 
    if (strstr(str, "CHANNEL_TRIGGER")!=NULL) {
      CAEN_DGTZ_TriggerMode_t tm;
      read = fscanf(fcfg, "%s", str1);
      if (strcmp(str1, "DISABLED")==0)
	tm = CAEN_DGTZ_TRGMODE_DISABLED;
      else if (strcmp(str1, "ACQUISITION_ONLY")==0)
	tm = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
      else if (strcmp(str1, "ACQ_AND_EXTOUT")==0)
	tm = CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;
      else if (strcmp(str1, "EXTOUT_ONLY")==0)
	tm = CAEN_DGTZ_TRGMODE_EXTOUT_ONLY;
      else {
	printf("%s: Invalid Parameter\n", str);
	continue;
      }
      if (ch == -1)
	for(i=0; i<Nch; i++) cfg->mode[i] = tm;
      else
	cfg->mode[ch] = tm;
      continue;
    }

    // Channel Enable (YES/NO)
    if (strstr(str, "ENABLE_INPUT")!=NULL) {
      read = fscanf(fcfg, "%s", str1);
      if (strcmp(str1, "YES")==0) {
	if (ch == -1)
	  cfg->mask = 0xF;
	else
	  cfg->mask |= (1 << ch);
	continue;
      } else if (strcmp(str1, "NO")==0) {
	if (ch == -1)
	  cfg->mask = 0x0;
	else
	  cfg->mask &= ~(1 << ch);
	continue;
      } else {
	printf("%s: invalid option\n", str);
      }
      continue;
    }

    printf("%s: invalid setting\n", str);
  }
  return 0;
}

