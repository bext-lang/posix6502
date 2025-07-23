# posix6502

Simple 6502 emulator that integrates with POSIX environment. Based on [fake6502](http://rubbermallet.org/fake6502.c) by Mike Chambers. Extracted from [B compiler in Crust](https://github.com/tsoding/b) project.

## Quick Start

```console
$ cc -o nob nob.c
$ ./nob
$ ./build/posix6502 ./examples/fizzbuzz.6502
```

The [Fizz buzz](https://en.wikipedia.org/wiki/Fizz_buzz) binary is produced by the [B compiler](https://github.com/tsoding/b) we extracted this project from.

## Supported POSIX routines

- `$FFEF` - [putchar](https://www.man7.org/linux/man-pages/man3/putchar.3p.html) located at the same address as wozmon `ECHO` routine
- `$0000` - [exit](https://www.man7.org/linux/man-pages/man3/exit.3.html)

*It's not much, but feel free to add more.*
