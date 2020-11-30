Run [0](0) contains data from a few channels, which serve as example input for the testing of [processing scripts](https://github.com/jintonic/toward).

To create a new run, for example, **run 1**,  make a new folder here and simply name it **1**. Create a [WaveDump][] configuration file, `daq.cfg` in that folder and run `wavedump daq.cfg` in it. In order for [../w2r.sh](../w2r.sh) to process the configuration file properly, please don't name it other than `daq.cfg`. Settings that can be tuned in the configuration file can be found in the [WaveDump mannual](https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view). Make sure that the following two are set as follows:

```
OUTPUT_FILE_FORMAT BINARY
OUTPUT_FILE_HEADER YES
```

One can also copy [0/ch0.cfg](0/ch0.cfg) to `1/daq.cfg` and modify it.

[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[DT5720]:https://www.caen.it/products/dt5720/
[TOWARD]:https://github.com/jintonic/toward
