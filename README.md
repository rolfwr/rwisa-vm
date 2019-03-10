RW ISA VM examples
==================

Example imlementations Virtual Machines (VMs) for the "RW" Instruction Set
Architecture.

These VMs can be used to run the example executables from
http://rolfwr.net/rwisa/rwisa-exe.zip

Companion presentation slides at http://rolfwr.net/tarpit/

The RW instuction set can be summarized as:

| Opcode           | Description    | C-like implementation              |
| ---------------- | -------------- | ---------------------------------- |
| 00               | Halt           | exit(0)                            |
| 01 srcptr        | Output Byte    | putchar(mem[srcptr])               |
| 02 jmpptr srcptr | Branch If Plus | if (mem[srcptr] < 128) pc = jmpptr |
| 03 dstptr srcptr | Subtract       | mem[dstptr] -= mem[srcptr]         |
| 04 dstptr        | Input Byte     | mem[dstptr] = getchar()            |

The trailing bytes following an opcode byte indicated by srcptr, dstptr and
jmpptr represent little endian encoded unsigned integer values that refer
to memory locations. For ISA identifiers suffixed by "2" such as RWa2 and RWb2,
the these are four byte little endian unsigned integers. I.e. this format uses
32-bit pointers.

The RWa2 executable image is headerless, meaning that the first byte of an
executable image contains the first opcode to be executed.

The RWb executable image has a simple header which which allows for both a
.text section for code and data, and a .bss section for zero initialized
working memory.

The RWb executable image format can be summarized as follows:

| Offset | Size    | What                                          |
| ------ | ------- | --------------------------------------------- |
| 0      | 2       | Magic 'RW'                                    |
| 2      | 1       | ISA revision + 97                             |
| 3      | 1       | Pointer size (ps): Log2(ps) + 48              |
| 4      | ps      | End of file (eof) pointer, .BSS segment start |
| 4+ps   | ps      | End of memory (eom) pointer, .BSS segment end |
| 4+2×ps | ps      | .TEXT segment, executable code and data       |
| eof    | eom-eof | Zero initialized runtime memory (.BSS)        |

The executable image file format and the runtime memory layout are identical,
with the exception that the last part consisting of the zero initialized
runtime memory .BSS segment is not stored as part of the file.

By convention, executable image files with headers are given a file name
extension that matches the lower case ASCII repesentation of the first
four bytes of the file. E.g. an image file with ISA revision 2 and pointer
size 4 is has extension ".rwb2", and if we change the pointer size to 8,
then the extension becomes ".rwb3". Headerless executable images implies
that the ISA revision is 1, and the pointer size is 4. These executable
image files are given the extension ".rwa2" by convention.

The following ISA revisions have been defined:

| Rev# | Description                                        |
| ---- | -------------------------------------------------- |
| 1    | Executable image without header, opcodes 0–4 used. |
| 2    | Executable image with header, opcodes 0–4 used.    |
| 3    | Executable image with header, opcodes 0–7 used.    |

The instructions of opcodes 5–7 defined by revision 3 can be synthesized
using existing instructions, but allow for faster execution when used as
distinct opcodes.

| Opcode           | Description    | C-like implementation                                              |
| ---------------- | -------------- | ------------------------------------------------------------------ |
| 05 dstptr srcptr | Move Byte      | mem[dstptr] = mem[srcptr]                                          |
| 06 jmpptr srcptr | Branch If Zero | if (mem[srcptr] == 0) pc = jmpptr                                  |
| 07 dstptr srcptr | Add Pointers   | writeptr(mem, dstptr) = readptr(mem, dstptr) + readptr(mem.srcptr) |
