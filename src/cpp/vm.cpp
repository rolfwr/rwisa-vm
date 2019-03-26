#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include "overload.hpp"

using namespace std::literals;

struct Halt
{};

struct Out
{
  uint32_t src;
};

struct BPL
{
  uint32_t jmp;
  uint32_t src;
};

struct Sub
{
  uint32_t dst;
  uint32_t src;
};

struct In
{
  uint32_t dst;
};

using Instruction = std::variant<Halt, Out, BPL, Sub, In>;

std::ostream&
operator<<(std::ostream& os, const Instruction& instruction)
{
  std::visit(
    overload([&](const Halt& v) { os << "HLT\n"; },
             [&](const Out& v) { os << "OUT {src: " << v.src << "}\n"; },
             [&](const BPL& v) {
               os << "BPL {jmp: " << v.jmp << ", src: " << v.src << "}\n";
             },
             [&](const Sub& v) {
               os << "SUB {dst: " << v.dst << ", src: " << v.src << "}\n";
             },
             [&](const In& v) { os << "IN  {dst: " << v.dst << "}\n"; }),
    instruction);
  return os;
}

class Machine
{
  std::string memory;
  uint32_t pc;

public:
  Machine(std::string&& memory, int pc)
    : memory(std::move(memory))
    , pc(pc)
  {}

  uint8_t fetchByteFromMemory(uint32_t pointer)
  {
    if (pointer > memory.size()) {
      std::stringstream s;
      s << "Pointer pointing to invalid memory: " << pointer
        << ", memory size is: " << memory.size();
      throw std::runtime_error(s.str());
    }

    return static_cast<uint8_t>(memory[pointer]);
  }

  uint32_t fetchPointerAtPC()
  {
    uint32_t pointer = 0;
    for (const auto i : { 0, 1, 2, 3 }) {
      const uint8_t value = fetchByteFromMemory(pc);
      pointer = pointer + (static_cast<uint32_t>(value) << (i * 8));
      ++pc;
    }

    return pointer;
  }

  std::optional<Instruction> nextInstruction()
  {
    const uint8_t opcode = fetchByteFromMemory(pc);
    pc += 1;

    switch (opcode) {
      case 0:
        return Halt{};
      case 1:
        return Out{ fetchPointerAtPC() };
      case 2: {
        auto jmp = fetchPointerAtPC();
        auto src = fetchPointerAtPC();
        return BPL{ jmp, src };
      }
      case 3: {
        auto dst = fetchPointerAtPC();
        auto src = fetchPointerAtPC();
        return Sub{ dst, src };
      }
      case 4:
        return In{ fetchPointerAtPC() };
    }

    return std::nullopt;
  }

  void printMemory()
  {
    for (std::optional<Instruction> instruction = nextInstruction();
         instruction.has_value();
         instruction = nextInstruction()) {
      std::cout << instruction.value();
    }
  }

  void run()
  {
    while (true) {
      const Instruction next = nextInstruction().value();
      std::visit(overload([](const Halt& v) { std::exit(0); },
                          [this](const Out& v) {
                            const uint8_t val = fetchByteFromMemory(v.src);
                            std::cout << val;
                            std::cout.flush();
                          },
                          [this](const BPL& v) {
                            const uint8_t byte = fetchByteFromMemory(v.src);
                            if (byte <= 127) {
                              pc = v.jmp;
                            }
                          },
                          [this](const Sub& v) {
                            uint8_t dstByte = fetchByteFromMemory(v.dst);
                            const uint8_t srcByte = fetchByteFromMemory(v.src);

                            dstByte -= srcByte;
                            memory[v.dst] = dstByte;
                          },
                          [this](const In& v) {
                            // have to use std::cin.read(), as trying to use
                            // stream-operators yields formatted input, which
                            // results in all whitespace vanishing. This would
                            // be a bug as the whitespace is to be parsed and
                            // interpreted as part of task
                            uint8_t val;
                            std::cin.read(reinterpret_cast<char*>(&val),
                                          sizeof val);
                            memory[v.dst] = val;
                          }),
                 next);
    }
  }
};

static std::string
read_file(std::ifstream& file)
{
  std::ostringstream s;
  s << file.rdbuf();
  return s.str();
}

int
main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "not enough arguments\n";
    return 1;
  }

  const std::string programFile = argv[1];
  std::cout << "Executing program in file: " << programFile << "\n";

  std::ifstream infile;
  infile.open(programFile, std::ios::binary);
  if (!infile.is_open()) {
    std::cerr << "failed to open file\n";
    return 1;
  }
  std::string file = read_file(infile);

  Machine machine{ std::move(file), 0 };

  if (argc >= 3 && (argv[2] == "print"s))
    machine.printMemory();
  else
    machine.run();

  return 0;
}
