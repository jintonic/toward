[![Homepage](https://img.shields.io/badge/toward-homepage-blue?style=flat)](https://github.com/jintonic/toward)
[![Examples](https://img.shields.io/badge/example-data-red?style=flat)](0)

## Data structure

The way to organize digital waveform data depends on analysis tools and ecosystem to be used. A [class][] named `Waveform` or `Waveforms` may not be the best choice. Generally speaking, data structures are the way we organize information on our computer; it should involve things more than just [data members][] of a [class][]. For example, a well designed directory structure and output file naming scheme may simplify the coding significantly. They should be considered as a part of data structure.

[class]:https://en.wikipedia.org/wiki/Class_(computer_programming)
[data members]:http://www.cplusplus.com/doc/tutorial/classes/

### Directory structure

[Binary waveform data](0#output-file-settings) from [WaveDump][] should be saved as runs in this directory.

Run [0](0) contains example data. They serve as test input of the [processing scripts](TOWARD). One can use them to experience the power of [TOWARD][] without physically owning a [CAEN][] [digitizer][].

To create a new run, for example, **run 1**,  make a new folder here and simply name it **1**. Create a [WaveDump][] configuration file, `WaveDumpConfig.txt` in that folder and run `wavedump WaveDumpConfig.txt` in it. In order for [../w2r.sh](../w2r.sh) to process the configuration file properly, please don't name it other than `WaveDumpConfig.txt`. Settings that can be tuned in the configuration file can be found in the [WaveDump mannual](https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view). One can also copy [0/WaveDumpConfig.txt](0/WaveDumpConfig.txt) to `1/WaveDumpConfig.txt` and modify it as needed.

[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[digitizer]:https://www.caen.it/sections/digitizer-families/
[TOWARD]:https://github.com/jintonic/toward
