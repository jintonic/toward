#!/bin/bash

exe=$(basename $0)
name=${exe%.sh}

PrintUsage()
{
  echo "Usage:"
  echo "$exe 2345"
  echo "start run 2345"
  echo "$exe 2345 30"
  echo "start run 2345 and quit when 30 events are collected" 
  exit
}

# if number of arguments is less than 1 or more than 2
if [ $# -lt 1 -o $# -gt 2 ]; then PrintUsage; fi

# if argument 1 is not a positive integer
if ! [[ $1 =~ ^[0-9]+$ ]]; then PrintUsage; fi
subdir=`expr $1 / 100`
subdir=`printf "%04d00" $subdir`

# if run number is too big
if [ $1 -ge 1000000 ]; then
  echo "run number should have only 6 digits"
  exit
fi

# get path where this script locates
while [ -h "$0" ] ; do src="$(readlink "$0")"; done
src="$(cd -P "$(dirname "$src")" && pwd)"

# go to GEE data directory
if [ -d $GEEDAT ]; then
  mkdir -p $GEEDAT/$subdir
  cd $GEEDAT/$subdir
fi

# copy daq.cfg
cfgfile=`printf "run_%06d.000001.cfg" $1`
if [ ! -f $cfgfile ]; then cp $src/daq.cfg $cfgfile; fi

# run executable
if [ $# -ne 2 ]; then
  $src/$name.exe $cfgfile $1
else
  # if argument 2 is not a positive integer
  if ! [[ $1 =~ ^[0-9]+$ ]]; then PrintUsage; fi
  # else
  $src/$name.exe $cfgfile $1 $2
fi
