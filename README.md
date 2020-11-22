# TOWARD
Tools, Objects for Waveform Analysis, Reformatting & Drawing

## Prerequisites

CERN [ROOT][] is needed to run scripts ended with `.C`. [shell scripts][sh] can be run in a Linux or Mac terminal. They can also be run in [MobaXterm][] in a Windows PC.

[CAEN][] [WaveDump][] is not needed to run the scripts here, but its output is the input for these cripts.

## Scripts

- [b2r.C](b2r.C): a [ROOT][] script to convert a [CAEN][] [WaveDump][] binary output to [ROOT][] format
- [b2r.sh](b2r.sh): a [shell script][sh] to call [b2r.C](b2r.C)
- [getVoltagePulseHeight.C](getVoltagePulseHeight.C): a [ROOT][] script to get basic parameters of digitized voltage pulses using [trapezoidal filter][https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/]

## Features

- processing scripts can be run directly in [ROOT][] without compilation

[ROOT]:https://root.cern.ch
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[sh]:https://www.shellscript.sh/
[MobaXterm]:https://mobaxterm.mobatek.net/
