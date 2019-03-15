use std::env;
use std::error::Error;
use std::fmt;
use std::fs::File;
use std::io;
use std::io::prelude::*;

enum Instruction {
    Halt,
    Out(i32),
    BranchIfPlus(i32, i32),
    Subtract(i32, i32),
    InByte(i32),
}

impl fmt::Display for Instruction {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use Instruction::*;
        match self {
            Halt => write!(f, "HLT"),
            Out(a) => write!(f, "OUT [{}]", a),
            BranchIfPlus(a, b) => write!(f, "BIP {}, [{}]", a, b),
            Subtract(a, b) => write!(f, "SUB [{}], [{}]", a, b),
            InByte(a) => write!(f, "IN [{}]", a),
        }
    }
}

struct Machine {
    mem: Vec<u8>,
    pc: i32,
}

impl Machine {
    fn from_file(programfile: &str) -> Result<Machine, Box<Error>> {
        let mut memfile = File::open(programfile)?;

        let mut mem = Vec::new();
        memfile.read_to_end(&mut mem)?;

        Ok(Machine { mem, pc: 0 })
    }

    fn fetch_pointer(&mut self) -> i32 {
        let addr = &self.mem[(self.pc as usize)..];
        let ptr: i32 = (addr[0] as i32)
            | ((addr[1] as i32) << 8)
            | ((addr[2] as i32) << 16)
            | ((addr[3] as i32) << 24);
        self.pc += 4;
        ptr
    }

    fn next_instruction(&mut self) -> Result<Instruction, String> {
        let inst = self.mem[self.pc as usize];
        self.pc += 1;

        use Instruction::*;
        match inst {
            0 => Ok(Halt),
            1 => Ok(Out(self.fetch_pointer())),
            2 => Ok(BranchIfPlus(self.fetch_pointer(), self.fetch_pointer())),
            3 => Ok(Subtract(self.fetch_pointer(), self.fetch_pointer())),
            4 => Ok(InByte(self.fetch_pointer())),
            _ => Err(format!("Unknown instruction {}", inst)),
        }
    }

    fn disassemble(&mut self) -> Result<(), Box<Error>> {
        while (self.pc as usize) < self.mem.len() {
            let inst_pc = self.pc;
            // Just ignore everything that is not a valid instruction
            if let Ok(inst) = self.next_instruction() {
                println!("{}    {}", inst_pc, inst);
            }
        }
        Ok(())
    }

    fn run(&mut self) -> Result<(), Box<Error>> {
        loop {
            let inst = self.next_instruction()?;

            use Instruction::*;
            match inst {
                Halt => {
                    std::process::exit(0);
                }
                Out(ptr) => {
                    let ch = self.mem[ptr as usize] as char;
                    print!("{}", ch);
                }
                BranchIfPlus(jmpptr, srcptr) => {
                    if self.mem[srcptr as usize] < 128 {
                        self.pc = jmpptr;
                    }
                }
                Subtract(dstptr, srcptr) => {
                    let dst_val = self.mem[dstptr as usize];
                    let src_val = self.mem[srcptr as usize];
                    self.mem[dstptr as usize] = dst_val.wrapping_sub(src_val);
                }
                InByte(dstptr) => {
                    let mut buffer = [0];
                    io::stdin().read_exact(&mut buffer)?;
                    self.mem[dstptr as usize] = buffer[0];
                }
            }
        }
    }
}

fn main() -> Result<(), Box<Error>> {
    let args: Vec<_> = env::args().collect();
    if args.len() == 2 {
        let mut machine = Machine::from_file(&args[1])?;
        machine.run()?;
    } else if args.len() == 3 && args[1] == "disassemble" {
        let mut machine = Machine::from_file(&args[2])?;
        machine.disassemble()?;
    } else {
        eprintln!("Usage: {} [disassemble] <FILE>", args[0]);
    }

    Ok(())
}
