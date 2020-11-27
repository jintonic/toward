#!/bin/sh

# get run and channel number
run=0
cha=0
if [ "$#" -lt 1 ]; then
  echo "Usage:"
  echo "./w2r.sh <run number> <channel number>"
  exit
else
  if [[ "$1" =~ ^[0-9]+$ ]]; then # positive integer
    run=$1
  else
    echo "run number \"$1\" is not a positive integer"
    exit
  fi
  if [ "$#" -gt 1 ]; then
    if [[ "$2" =~ ^[0-9]+$ ]]; then # positive integer
      cha=$2
    else
      echo "channel number \"$2\" is not a positive integer"
      exit
    fi
  fi
fi

# check if run/$run/wave$cha.dat exists
if [ ! -f "run/$run/wave$cha.dat" ]; then
  echo "run/$run/wave$cha.dat does not exist!"
  exit
fi

# run script w2r.C with two arguments $run and $cha
root -b -q -l w2r.C"($run,$cha)"
