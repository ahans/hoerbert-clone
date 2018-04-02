# hoerbert-clone
Audio player for children inspired by the `Hoerbert' device (https://hoerbert.com).

Currently this is a WIP and has only some loose parts: a WAV reader class, some resistor ladder code to be able to read a couple of buttons, and some code to talk to a MAX98357A-based DAC board.

## Converting files

SoX command line recommended for Hoerbert:
```
sox --buffer 131072 --multi-threaded --no-glob <input_file> --clobber -r 32000 -b 16 -e signed-integer --no-glob <output>.wav remix - gain -n -1.5 bass +1 loudness -1 pad 0 0;
```
This doesn't work well with the MAX98357A-based setup, there's lots of clipping noise.

Instead, this one is better:
```
sox --buffer 131072 --multi-threaded --no-glob <input_file> --clobber -r 32000 -b 16 -e signed-integer --no-glob <output>.wav remix - gain -n -10 bass +1 loudness -1;
```
So in essence, reduce the base level quite considerably.