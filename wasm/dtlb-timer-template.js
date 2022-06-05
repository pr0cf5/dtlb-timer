const PGSIZE = 0x1000;
const TLB_SETS = 0x10;
const CACHE_LINESIZE = 0x40;

const wasmBytes = new Uint8Array(/*--WASMCODE--*/);

class DTLBTimer {
    constructor() {
        // WASM pages are 64KB
        // We need 9 identical pages that map to the same dTLB set, so 9 * 16 KiB pages are needed
        // But just allocate a ton of pages 
        this.memory = new WebAssembly.Memory({initial: 8192, maximum: 8192});
        this.wasm = new WebAssembly.Instance(new WebAssembly.Module(wasmBytes), {
            env: {
                mem: this.memory,
            }
        });
    }

    time_without_x(reps) {
        let rv = [];
        for (let i = 0; i < reps; i++) { 
            let t = performance.now();
            this.wasm.exports.run_without_x(0x10000);
            t = performance.now() - t;
            rv.push(t);
        }
        return rv;
    }

    time_with_x(reps) {
        let rv = [];
        for (let i = 0; i < reps; i++) {
            let t = performance.now();
            this.wasm.exports.run_with_x(0x10000);
            t = performance.now() - t;
            rv.push(t);
        }
        return rv;
    }

    warmup() {
        for (let i = 0; i < 0x1000; i++) {
            this.wasm.exports.run_without_x(0x100);
            this.wasm.exports.run_with_x(0x100);
        }
    }
}

function tlb_get_data() {
    let timer = new DTLBTimer();
    timer.warmup();
    let time_without_x = timer.time_without_x(100);
    let time_with_x = timer.time_with_x(100);
    return [time_without_x, time_with_x];
}