[![Homepage](https://img.shields.io/badge/toward-homepage-blue?style=flat)](https://github.com/jintonic/toward)
[![Data structure](https://img.shields.io/badge/data-structure-red?style=flat)](..)

## Example data

[WaveDump][] saves waveforms from each channel separately, for example, `wave0.dat` is from channel 0.

- wave0.dat contains waveforms from a charge-sensitive pre-amplifier with a [DT5751][] digitizer
- wave1.dat contains waveforms from a Hamamatsu SiPM module with a [DT5751][] digitizer
- wave2.dat contains waveforms from a NaI(Tl) detector with a [DT5751][] digitizer

## WaveDump configurations

- [daq.cfg](daq.cfg) is an example [WaveDump][] configuration file that can be copied to other runs as a starting point for customization
- [ch0.cfg](ch0.cfg) contains detailed comments on common [WaveDump][] settings
- [ch1.cfg](ch1.cfg) contains minimal comments on common [WaveDump][] setting blocks
- [ch2.cfg](ch2.cfg) contains only necessary settings without any comments

### Output file settings

Make sure that the following two options are set as follows in your [WaveDump][] configuration file:

```sh
# By default the WaveDump output file format is ASCII,
# we need to change it to BINARY to save disk space.
OUTPUT_FILE_FORMAT BINARY

# By default WaveDump does not attach a header to each waveform, 
# we need to enable it to know the size of a waveform data block.
OUTPUT_FILE_HEADER YES
```

According to the WaveDump [manual][], the header is simply six 32-bit integers:

1. event size in byte (HEADER size + sample size)
2. board Id
3. pattern a VME specific parameter
4. channel Id
5. event Id
6. trigger time tag

It is followed by waveform samples as 16 or 8-bit integers.

### Syntax highlighting

[cfg.vim](cfg.vim) enables [syntax highlighting][1] of a [WaveDump][] configuration file in [vim][]. It can be installed the following way in Linux or Mac:

```sh
$ mkdir -p ~/.vim/after/syntax
$ cp cfg.vim ~/.vim/after/syntax
```

A [WaveDump][] configuration file may look like the following in vim with [syntax highlighting][1] enabled:

![wavedump config file syntax highlighting in vim](vim.png)

[1]:https://en.wikipedia.org/wiki/Syntax_highlighting
[vim]:https://www.vim.org/
[CAEN]:https://www.caen.it/
[WaveDump]:https://www.caen.it/products/caen-wavedump/
[DT5751]:https://www.caen.it/products/dt5751/
[TOWARD]:https://github.com/jintonic/toward
[manual]:https://usermanual.wiki/Document/UM2091WaveDumpUserManualrev13.87092449/view
