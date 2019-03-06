#! /bin/env python3
import sys

programfile = sys.argv[1]

with open(programfile, mode='rb') as file:
    mem = bytearray(file.read())

pc = 0
while True:
    inst = mem[pc]
    pc = pc + 1
    if inst == 0: # Halt (HLT)
        sys.exit(0)
    raise NotImplementedError("Unknown instruction " + str(inst))
