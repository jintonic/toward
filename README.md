# TOWARD
Tools, Objects for Waveform Analysis, Reformatting & Drawing

## Prerequisites

CERN [ROOT][] is needed to run scripts ended with `.C`. [Shell scripts][sh] can be run in a Linux or Mac terminal. They can also be run in [MobaXterm][] in a Windows PC.

[CAEN][] [WaveDump][] is not needed to run the scripts here, but its output is the input for these scripts.

## Get started

- get into folder [run](run), create a directory there with its name to be an integer run number, for example, **153**
- get into the just created subdirectory and create a [WaveDump][] configuration file there named **daq.cfg** (one can take [run/0/ch0.cfg](run/0/ch0.cfg) as an example)
- run `wavedump daq.cfg` there to create binary output file **wave?.dat**, where **?** is the channel number
- get back to the base directory, run `./w2r.sh <run number> <channel number>` to convert **wave?.dat** to **wave?.root**
- use other scripts to process the generated root file

## Scripts

- [w2r.C](w2r.C): a [ROOT][] script to convert a [CAEN][] [WaveDump][] binary output to [ROOT][] format, align baselines of waveforms to zero, and save basic parameters of waveforms
- [w2r.sh](w2r.sh): a [shell script][sh] to fetch information from a [WaveDump][] configuration file and pass it to [w2r.C](w2r.C)
- [q2i.C](q2i.C): a [ROOT][] script to get heights of charge pulses using [trapezoidal filter](https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/) and convert charge pulses to current ones using [numerical differentiation](https://terpconnect.umd.edu/%7Etoh/spectrum/Differentiation.html)
- [i2q.C](i2q.C): a [ROOT][] script to convert current pulses to charge ones
- [integrate.C](integrate.C): a [ROOT][] script to integrate waveforms in a certain range and save the result to the original data [tree][]

## Features

- Scripts can be run directly in [ROOT][] without compilation
- Works with [WaveDump][] without any modification of code on both sides
- Cross major platforms: Linux, Mac, and Windows

[ROOT]:https://root.cern.ch
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[sh]:https://www.shellscript.sh/
[MobaXterm]:https://mobaxterm.mobatek.net/
[tree]:https://root.cern.ch/doc/master/classTTree.html#addcoltoexistingtree
