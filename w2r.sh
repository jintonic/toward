#!/bin/bash
# positive integer check does not work for #!/bin/sh in Ubuntu,
# but #!/bin/bash works

# get run and channel number
run=0; ch=0 # default value
if [ "$#" -lt 1 ]; then # zero argument
  echo -e "Usage:\n./w2r.sh <run number> <channel number>"; exit
else # >= 1 arguments
  if [[ "$1" =~ ^[0-9]+$ ]]; then # positive integer check
    run=$1
  else
    echo "run number \"$1\" is not a positive integer"; exit
  fi
  if [ "$#" -gt 1 ]; then # > 1 arguments
    if [[ "$2" =~ ^[0-9]+$ ]]; then # positive integer check
      ch=$2
    else
      echo "channel number \"$2\" is not a positive integer"; exit
    fi
  fi
fi
echo -e "run: $run \nchannel: $ch"

# check if WaveDump output binary file "wave?.dat" exists
input="run/$run/wave$ch.dat"
if [ ! -f "$input" ]; then echo "$input does not exist!"; exit; fi

# check if WaveDump configuration file "daq.cfg" exists
cfg="run/$run/daq.cfg"
if [[ $run -eq 0 ]]; then cfg="run/$run/ch$ch.cfg"; fi
if [ ! -f "$cfg" ]; then echo "$cfg does not exist!"; exit; fi

# fetch model number of digitizer from the configuration file
model=`awk '/^#[ ]+Digitizer:[ \t]/{print tolower($NF)}' $cfg`
byte=2 # length of a sample value
if [[ "$model" == 721 ]] || [[ "$model" == 731 ]]; then byte=1; fi
echo digitizer: $model

# fetch polarity setup from the configuration file
polarity=1 # default value
v=`awk '/^PULSE_POLARITY[ \t]/{print tolower($2)}' $cfg`
if [[ "$v" == negative ]]; then polarity=-1; fi
echo polarity: $v

# fetch trigger threshold
threshold=`awk '/^TRIGGER_THRESHOLD[ \t]/{print tolower($2)}' $cfg`
echo trigger threshold: $threshold ADC

# fetch record length and post trigger percentage
len=`awk '/^RECORD_LENGTH[ \t]/{print tolower($2)}' $cfg`
echo record length: $len samples
post_trg=`awk '/^POST_TRIGGER[ \t]/{print tolower($2)}' $cfg`

pct=10 # percent of record length for baseline calculation
if [[ $post_trg -lt $pct ]]; then pct=$post_trg; fi
nbase=`echo - | awk -v l="$len" -v p="$pct" '{printf "%.0f", l*p/100}'`
echo number of samples to calculate baseline: $nbase

# pass arguments to w2r.C
root -b -q -l w2r.C"($run,$ch,$threshold,$polarity,$nbase,$byte)"
