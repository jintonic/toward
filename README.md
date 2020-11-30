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
- Data are saved as basic types in [ROOT][] [TTree][] [ntuple][]s, which can be easily open without loading extra libraries defining complicated data structure. [Uproot][] can be used to load the data for analysis in [Python][]
- Super short variable names for quick analysis using [ROOT][] [TTree][]::[Draw][] function in a [ROOT interactive session](https://root.cern.ch/root/html534/guides/users-guide/GettingStarted.html). For example,
```cpp
[root] t->Draw("s:Iteration$","is==0","l",5,2)
```
shows 5 non-saturated (`is==0`) waveforms (`s`: individual waveform samples, [Iterations][Draw]\$: sample index) starting from event 2:

![waveform display using simple TTree variables](run/0/wfs.png)

[ROOT]:https://root.cern.ch
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[sh]:https://www.shellscript.sh/
[MobaXterm]:https://mobaxterm.mobatek.net/
[tree]:https://root.cern.ch/doc/master/classTTree.html#addcoltoexistingtree
[TTree]: https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html
[ntuple]:https://en.wikipedia.org/wiki/Tuple
[Uproot]:https://github.com/scikit-hep/uproot4
[Python]:https://www.python.org/
[Draw]:https://root.cern.ch/doc/master/classTTree.html#a73450649dc6e54b5b94516c468523e45
