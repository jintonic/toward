# TOWARD
Tools, Objects for Waveform Analysis, Reformatting & Drawing

## Prerequisites

CERN [ROOT][] is needed to run scripts ended with `.C`. [Shell scripts][sh] can be run in a Linux or Mac terminal. They can also be run in [MobaXterm][] in a Windows PC.

[CAEN][] [WaveDump][] is not needed to run the scripts here, but its output is the input for these scripts.

## Scripts

- [w2r.C](w2r.C): a [ROOT][] script to convert a [CAEN][] [WaveDump][] binary output to [ROOT][] format
- [w2r.sh](w2r.sh): a [shell script][sh] to call [w2r.C](w2r.C)
- [q2i.C](q2i.C): a [ROOT][] script to get basic parameters of digitized charge pulses using [trapezoidal filter](https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/) and convert the charge pulses to current ones using [numerical differentiation](https://terpconnect.umd.edu/%7Etoh/spectrum/Differentiation.html)

## Features

- Scripts can be run directly in [ROOT][] without compilation
- Works with [WaveDump][] without any modification of code on both sides
- Cross major platforms: Linux, Mac, and Windows

[ROOT]:https://root.cern.ch
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[sh]:https://www.shellscript.sh/
[MobaXterm]:https://mobaxterm.mobatek.net/
