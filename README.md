![ TOWARD - Tools, Objects for Waveform Analysis, Reformatting & Drawing](run/0/logo.png)

[Bash][], [Python][] and [ROOT][] scripts for the process of digital waveforms taken with a [CAEN][] [digitizer][] and its [WaveDump][] readout program.

[Git]:https://git-scm.com/
[Bash]:https://en.wikipedia.org/wiki/Bash_(Unix_shell)
[Python]:https://www.python.org/
[ROOT]:https://root.cern.ch
[CAEN]:https://www.caen.it/
[digitizer]:https://www.caen.it/sections/digitizer-families/
[WaveDump]:https://www.caen.it/products/caen-wavedump/

[![Data structure](https://img.shields.io/badge/data-structure-blue?style=flat)](run)
[![Examples](https://img.shields.io/badge/example-data-red?style=flat)](run/0)

## Prerequisites

[CAEN][] [WaveDump][] is not needed to run the scripts here, but its output is the input for these scripts. A few example [WaveDump][] output files named as `wave[0-3].dat` are included in the folder [run/0](run/0) so that the processing scripts can be tested without running [WaveDump][].

CERN [ROOT][] is needed to run scripts ended with `.C`. The [binary distribution of ROOT 5.34/38 for Windows](https://root.cern/releases/release-53438/) is good enough for this project and should be used before [version 6](https://root.cern/install/all_releases) for Windows becomes stable.

[Shell scripts][sh] can be run in a Linux or Mac terminal. They can also be run in [Git Bash][] or [MobaXterm][] in a Windows PC.

[Python][] is available for Linux, Mac and Windows. [Python][] packages [Uproot4][], [awkward][] and [matplotlib][] are needed. They can be installed as a normal user using the [pip][] command:

```sh
$ pip install --upgrade pip # upgrade pip
$ pip install uproot4
$ pip install awkward
$ pip install matplotlib
```

[sh]:https://www.shellscript.sh/
[Git Bash]:https://gitforwindows.org/
[MobaXterm]:https://mobaxterm.mobatek.net/
[awkward]:https://pypi.org/project/awkward/
[matplotlib]:https://matplotlib.org/
[pip]:https://pypi.org/project/pip/

## Getting started

### Exploring example data

A few example [WaveDump][] output files named as `wave[0-3].dat` are included in the folder [run/0](run/0) to demonstrate the usage of the processing scripts. To explore them, please

1. download the package from <https://github.com/jintonic/toward.git>
2. get to the `toward` directory, run `./w2r.sh <run number> <channel number>` to convert **wave?.dat** to **wave?.root**
3. run `./show.py <run number>` to show waveforms in the root file. Note that in [Git Bash][] or [MobaXterm][], the command has to be changed to `winpty python show.py <run number>` to enable [running Python installed in Windows in a Linux-like terminal](https://stackoverflow.com/questions/48199794/winpty-and-git-bash)
4. use other scripts to process the generated root file

### Analyzing new data

1. get into folder [run](run) in your local copy of `toward`, create a directory there with its name to be an integer run number, for example, **1**
2. get into the just created subdirectory and create a [WaveDump][] configuration file there named **WaveDumpConfig.txt** (You can take [run/0/WaveDumpConfig.txt](run/0/WaveDumpConfig.txt) as an example)
3. run `wavedump WaveDumpConfig.txt` there to create binary output file **wave?.dat**, where **?** is the channel number. Note that in Window, you may have to run [WaveDump][] from its installation folder, its configuration file and output files need to be copied or moved from there to the newly created folder `toward\run\<run number>`
4. follow the previous section starting from step 2.

## Scripts

- [w2r.C](w2r.C): a [ROOT script][] to convert a [CAEN][] [WaveDump][] binary output to [ROOT][] format, align baselines of waveforms to zero, and save basic parameters of waveforms
- [w2r.sh](w2r.sh): a [shell script][sh] to fetch information from a [WaveDump][] configuration file and pass it to [w2r.C](w2r.C)
- [q2i.C](q2i.C): a [ROOT script][] to get heights of charge pulses using [trapezoidal filter](https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/) and convert charge pulses to current ones using [numerical differentiation](https://terpconnect.umd.edu/%7Etoh/spectrum/Differentiation.html)
- [i2q.C](i2q.C): a [ROOT script][] to convert current pulses to charge ones
- [integrate.C](integrate.C): a [ROOT script][] to integrate waveforms in a certain range and save the result to the original data [tree][]
- [show.py](show.py): a [Python][] script to show waveforms in a [ROOT][] file.

## Features

- Works with [WaveDump][] without any modification of code on both sides
- All scripts can be run directly in Linux, Mac and Windows without compilation and installation
- [Data](run#data-structure) are saved as basic types in [ROOT][] [TTree][] [ntuple][]s, which can be easily open without loading extra libraries defining complicated data structure. [Uproot4][] can be used to load the data for analysis in [Python][]
- Super short variable names for quick analysis using [ROOT][] [TTree][]::[Draw][] function in a [ROOT interactive session](https://root.cern.ch/root/html534/guides/users-guide/GettingStarted.html). For example,
```cpp
[root] t->Draw("s:Iteration$","is==0","l",5,2)
```
shows 5 non-saturated (`is==0`) waveforms (`s`: individual waveform samples, [Iterations][Draw]\$: sample index) starting from event 2:

![waveform display using simple TTree variables](run/0/wfs.png)

[tree]:https://root.cern.ch/doc/master/classTTree.html#addcoltoexistingtree
[TTree]: https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html
[ntuple]:https://en.wikipedia.org/wiki/Tuple
[Uproot4]:https://github.com/scikit-hep/uproot4
[Draw]:https://root.cern.ch/doc/master/classTTree.html#a73450649dc6e54b5b94516c468523e45
[ROOT script]:https://root.cern.ch/root/htmldoc/guides/users-guide/Cling.html
