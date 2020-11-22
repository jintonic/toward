Run 0 was taken using [CAEN][] [WaveDump][] with a [DT5720][] digitizer. It serves as example input data for the testing of processing scripts in [TOWARD][]. [WaveDump][] saves waveforms from each channel separately, for example, `wave0.dat` is from channel 0.

According to the [WaveDump manual](https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view), each event starts with six 32-bit integers (HEADER):

1. event size in byte (HEADER size + sample size)
2. board Id
3. pattern a VME specific parameter
4. channel Id
5. event Id
6. trigger time tag

and followed by sample values as 16-bit integers.

[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[DT5720]:https://www.caen.it/products/dt5720/
[TOWARD]:https://github.com/jintonic/toward
