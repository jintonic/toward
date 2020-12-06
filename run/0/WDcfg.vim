" TOWARD parameters
syn match DigitizerFamily '\d\d\d'
syn match TOWARDpar '^#\s\+Digitizer:' nextgroup=DigitizerFamily skipwhite

" CAEN WaveDump configuration
syn keyword WaveDumpSetting OPEN EXTERNAL_TRIGGER ENABLE_INPUT WRITE_REGISTER OUTPUT_FILE_FORMAT OUTPUT_FILE_HEADER FPIO_LEVEL TEST_PATTERN CHANNEL_TRIGGER

syn match IntOpt '\d\+'
syn keyword WaveDumpSetting RECORD_LENGTH POST_TRIGGER PULSE_POLARITY BASELINE_LEVEL TRIGGER_THRESHOLD DECIMATION_FACTOR nextgroup=IntOpt skipwhite

syn keyword BoolOpt YES NO POSITIVE NEGATIVE
syn keyword StrOpt DISABLED ACQUISITION_ONLY ACQUISITION_AND_TRGOUT USB PCI TTL NIM BINARY ASCII

hi def link WaveDumpSetting Keyword
hi def link BoolOpt Boolean
hi def link IntOpt Number
hi def link StrOpt String
hi def link TOWARDpar Todo
hi def link DigitizerFamily Number
