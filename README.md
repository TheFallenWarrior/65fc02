# 65FC02

65FC02 is a minimal, experimental emulator for the MOS 6502 processor family. It was built to run on an NES with its limitations in mind. Compiled, the emulator and the cc65 runtime library add up to only ~11 KiB of ROM space.

## Motivation

Why not?

## Building

If you have `cc65` installed, just run `make`. `make run` builds and runs it with `mesen`.

## Limitations
 - **Compile-time setup:** The emulated ROM is included within the NES ROM and can't be changed without rebuilding the project.
 - **No MMIO:** 65FC02 essentially emulates the CPU only; its simplified memory model does not support hardware registers with read/write callbacks.
 - **No per-cycle emulation:** Emulation is done only at the instruction level due to hardware limitations and the emulator's reduced scope.
 - **Sloooow:** It simply isn't possible to emulate a 6502 on a 6502 at anywhere near real-time; if outputting the trace log, it only runs ~20 instructions per second.

## Credits

Code by Anna Jaqueline.

Based on the documentation in Joseph J. Carr's *6502 User's Manual* and the *MOS MCS6500 Microprocessors Data Sheet*. 
