import sys

def read_pointer(mem, offset):
    ptr = 0
    for i in range(4):
        ptr = ptr + (mem[offset] << (i * 8))
        offset = offset + 1
    return ptr

class Machine:
    def __init__(self, memory, pc):
        self.mem = memory
        self.pc = pc

    def fetch_pointer(self):
        ptr = read_pointer(self.mem, self.pc)
        self.pc = self.pc + 4
        return ptr

    def halt(self):
        sys.exit(0)
    
    def outputbyte(self):
        c = vm.mem[self.fetch_pointer()]
        sys.stdout.buffer.write(bytes([c]))
        sys.stdout.buffer.flush()

    def branchifplus(self):
        jmpptr = self.fetch_pointer()
        srcptr = self.fetch_pointer()
        if self.mem[srcptr] < 128:
            self.pc = jmpptr

    def subtract(self):
        dstptr = self.fetch_pointer()
        srcptr = self.fetch_pointer()
        self.mem[dstptr] = (self.mem[dstptr] - self.mem[srcptr]) & 0xFF

    def inputbyte(self):
        dstptr = self.fetch_pointer()
        self.mem[dstptr] = sys.stdin.buffer.read(1)[0]

    def run(self):
        ops = {
            0: self.halt,
            1: self.outputbyte,
            2: self.branchifplus,
            3: self.subtract,
            4: self.inputbyte
        }
        while True:
            inst = self.mem[self.pc]
            self.pc = self.pc + 1
            ops[inst]()

programfile = sys.argv[1]
with open(programfile, mode='rb') as file:
    memory = bytearray(file.read())

assert memory[0:2] == b"RW"

isa_revision = memory[2] - 97
pointer_size = 1 << (memory[3] - 48)

assert isa_revision == 1
assert pointer_size == 4

eof = read_pointer(memory, 4)
eom = read_pointer(memory, 4 + pointer_size)
fullmem = memory[0:eof] + bytearray(eom - eof)
pc = 4 + 2 * pointer_size
vm = Machine(fullmem, pc)
vm.run()