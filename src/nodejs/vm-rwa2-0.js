var fs = require("fs");

var memory = fs.readFileSync(process.argv[2]);
var pc = 0;

while (true) {
    var inst = memory[pc++];
    switch (inst) {
        case 0: // Halt
            process.exit();
        default:
            throw new Error("Unknown instruction " + inst)
    }
}