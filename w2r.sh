#!/bin/bash
# Positive integer check does not work for sh in Ubuntu, but bash works

# get run and channel number
run=0; ch=0
if [ "$#" -lt 1 ]; then
  echo "Usage:"
  echo "./w2r.sh <run number> <channel number>"
  exit
else
  if [[ "$1" =~ ^[0-9]+$ ]]; then # positive integer
    run=$1
  else
    echo "run number \"$1\" is not a positive integer"; exit
  fi
  if [ "$#" -gt 1 ]; then
    if [[ "$2" =~ ^[0-9]+$ ]]; then # positive integer
      ch=$2
    else
      echo "channel number \"$2\" is not a positive integer"; exit
    fi
  fi
fi

# check if WaveDump configuration file exists
cfg="run/$run/ch$ch.cfg"
if [ ! -f "$cfg" ]; then echo "$cfg does not exist!"; exit; fi

# check model of digitizer
model=`awk '/^#[ ]+Digitizer:[ \t]/{print tolower($NF)}' $cfg`
byte=2 # length of the sample value
if [[ "$model" == 721 ]] || [[ "$model" == 731 ]]; then byte=1; fi
echo digitizer: $model

# check polarity setup in the configuration file
polarity=1
v=`awk '/^PULSE_POLARITY[ \t]/{print tolower($2)}' $cfg`
if [[ "$v" == negative ]]; then polarity=-1; fi
echo polarity: $v

# check record length
length=`awk '/^RECORD_LENGTH[ \t]/{print tolower($2)}' $cfg`
echo record length: $length samples

awk '/^POST_TRIGGER[ \t]/{print $2}' run/$run/ch$ch.cfg

# check trigger threshold
threshold=`awk '/^TRIGGER_THRESHOLD[ \t]/{print tolower($2)}' $cfg`
echo trigger threshold: $threshold ADC units

# check if run/$run/wave$ch.dat exists
if [ ! -f "run/$run/wave$ch.dat" ]; then
  echo "run/$run/wave$ch.dat does not exist!"
  exit
fi

# run script w2r.C with two arguments $run and $ch
echo run: $run
echo channel: $ch
echo root -b -q -l w2r.C"($run,$ch)"
