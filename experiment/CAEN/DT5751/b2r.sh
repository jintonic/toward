#!/bin/bash

exe=$(basename $0)
name=${exe%.sh}

PrintUsage()
{
  echo "Usage:"
  echo "$exe 2345"
  echo "run $name.exe for the 1st sub run in run 2345"
  echo "$exe 2345 3"
  echo "run $name.exe for sub run 3 in run 2345" 
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

# run executable
cd $GEEDAT/$subdir
if [ $# -ne 2 ]; then
  $src/$name.exe $1 1 $GEEDAT
else
  if [[ $2 =~ ^[0-9]+$ ]]; then 
    if [ $2 -ge 1000000 ]; then
      echo "sub run number should have only 6 digits"
    else
      subrun=`printf "%06d" $2`
      $src/$name.exe $1 $2 $GEEDAT
    fi
  else
    PrintUsage
  fi
fi
