var fs = require("fs");

function get_sync_stdin() {
    try {
        return fs.openSync('/dev/stdin', 'rs');
    } catch (e) {
        return process.stdin.fd;
    }
}

class Machine {
    constructor(memory, pc) {
        this.mem = memory;
        this.pc = pc;
        this.buf = Buffer(1);
        this.stdin = get_sync_stdin();
    }

    fetch_pointer() {
        var ptr = 0;
        for (var i = 0; i < 4; ++i) {
            ptr += this.mem[this.pc++] << (i*8);
        }
        return ptr;
    }

    halt() {
        process.exit();
    }

    output_byte() {
        this.buf[0] = this.mem[this.fetch_pointer()];
        process.stdout.write(this.buf);
    }

    branch_if_plus() {
        var jmpptr = this.fetch_pointer();
        var srcptr = this.fetch_pointer();
        if (this.mem[srcptr] < 128) {
            this.pc = jmpptr;
        }
    }

    subtract() {
        var dstptr = this.fetch_pointer();
        var srcptr = this.fetch_pointer();
        this.mem[dstptr] -= this.mem[srcptr];
    }

    input_byte() {
        var dstptr = this.fetch_pointer();
        fs.readSync(this.stdin, this.buf, 0, 1, null);
        this.mem[dstptr] = this.buf[0]; 
    }

    run() {
        var ops = {
            0: this.halt.bind(this),
            1: this.output_byte.bind(this),
            2: this.branch_if_plus.bind(this),
            3: this.subtract.bind(this),
            4: this.input_byte.bind(this)
        }

        while (true) {
            var inst = this.mem[this.pc++];
            var func = ops[inst];
            if (!func) {
                throw new Error("Unknown instruction " + inst)
            }
            ops[inst]();
        }        
    }
}

var memory = fs.readFileSync(process.argv[2]);
var vm = new Machine(memory, 0);
vm.run();
