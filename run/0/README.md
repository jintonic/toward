Binary data here were taken with [CAEN][] [WaveDump][]. [WaveDump][] saves waveforms from each channel separately, for example, `wave0.dat` is from channel 0.

- wave0.dat contains waveforms from a charge-sensitive pre-amplifier with a [DT5751][] digitizer
- wave1.dat contains waveforms from a Hamamatsu SiPM module with a [DT5751][] digitizer
- wave2.dat contains waveforms from a NaI(Tl) detector with a [DT5751][] digitizer

- [ch0.cfg](ch0.cfg) contains detailed comments on common [WaveDump][] settings
- [ch1.cfg](ch1.cfg) contains minimal comments on common [WaveDump][] setting blocks
- [ch2.cfg](ch2.cfg) contains only necessary settings without any comments

- [cfg.vim](cfg.vim) enables [syntax highlighting][1] of a [WaveDump][] configuration file in [vim][]. It can be installed the following way in Linux or Mac:
```sh
$ mkdir -p ~/.vim/after/syntax
$ cp cfg.vim ~/.vim/after/syntax
```
[ch1.cfg](ch1.cfg) looks like the following in vim with [syntax highlighting][1] enabled:

![wavedump config file syntax highlighting in vim](vim.png)

## Binary data structure

According to the WaveDump [manual][], each event starts with six 32-bit integers (HEADER):

1. event size in byte (HEADER size + sample size)
2. board Id
3. pattern a VME specific parameter
4. channel Id
5. event Id
6. trigger time tag

and followed by sample values as 16 or 8-bit integers.

[1]:https://en.wikipedia.org/wiki/Syntax_highlighting
[vim]:https://www.vim.org/
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[DT5751]:https://www.caen.it/products/dt5751/
[TOWARD]:https://github.com/jintonic/toward
[manual]:https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view
