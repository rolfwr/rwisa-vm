import sys

class Machine:
    def __init__(self, memory, pc):
        self.mem = memory
        self.pc = pc

    def fetch_pointer(self):
        ptr = 0
        for i in range(4):
            ptr = ptr + (self.mem[self.pc] << (i * 8))
            self.pc = self.pc + 1
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

    def run(self):
        ops = {
            0: self.halt,
            1: self.outputbyte,
            2: self.branchifplus
        }
        while True:
            inst = self.mem[self.pc]
            self.pc = self.pc + 1
            ops[inst]()

programfile = sys.argv[1]
with open(programfile, mode='rb') as file:
    memory = bytearray(file.read())
vm = Machine(memory, 0)
vm.run()