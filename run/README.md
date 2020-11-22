Run 0 was taken using [CAEN][] [WaveDump][] with a [DT5720][] digitizer. It serves as example input data for the testing of processing scripts in [TOWARD][].

To create a new run, for example, run 1,  make a new folder simply called 1. Create a [WaveDump][] configuration file, `wavedump.cfg` in that folder and run `wavedump wavedump.cfg` in it. Settings that can be tuned in the configuration file can be found in the [WaveDump mannual](https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view). Make sure that the following two are set as follows:

```
OUTPUT_FILE_FORMAT BINARY
OUTPUT_FILE_HEADER YES
```

[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[DT5720]:https://www.caen.it/products/dt5720/
[TOWARD]:https://github.com/jintonic/toward
