all: dtlb-timer.js

dtlb-timer.js: gen.py dtlb-timer-template.js sequence.wasm
	./gen.py dtlb-timer-template.js sequence.wasm > $@

sequence.wasm: sequence.wat
	wat2wasm sequence.wat -o sequence.wasm

clean: 
	rm -rf sequence.wasm dtlb-timer.js