TOPNAME = Top

VERILATOR = verilator

VERILATOR_CFLAGS += --cc --exe --build --trace \
					-O3 --x-assign fast --x-initial fast --noassert

VERILATOR_INPUT = generated/Top.sv sim/main.cc sim/sim.cc

BUILD_DIR = ./build

PRJ = playground

test:
	mill -i $(PRJ).test

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i $(PRJ).runMain Elaborate --target-dir $(BUILD_DIR)

help:
	mill -i $(PRJ).runMain Elaborate --help

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: test verilog help reformat checkformat clean

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@rm -rf logs 
	@rm -rf $(OBJ_DIR)
	@echo "--------VERILATE & COMPILE(BUILD)--------"
	@$(VERILATOR) $(VERILATOR_CFLAGS) $(VERILATOR_INPUT) 
	@echo "--------DONE------------"
	$(BIN)

-include ../Makefile

