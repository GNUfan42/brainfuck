PREFIX := bf_
all: interpreter.c
	CELL_TYPE=uint8_t make $(SOMETHING)once
	CELL_TYPE=int8_t make $(SOMETHING)once
	CELL_TYPE=uint16_t make $(SOMETHING)once
	CELL_TYPE=int16_t make $(SOMETHING)once
	CELL_TYPE=uint32_t make $(SOMETHING)once
	CELL_TYPE=int32_t make $(SOMETHING)once
	CELL_TYPE=uint64_t make $(SOMETHING)once
	CELL_TYPE=int64_t make $(SOMETHING)once
	CELL_TYPE=float make $(SOMETHING)once
	CELL_TYPE=double make $(SOMETHING)once

once: interpreter.c
	cc interpreter.c -DT=$(CELL_TYPE) -o out/$(PREFIX)$(CELL_TYPE)

test: all
	echo >log
	printf "\tinternal:\n" >>log
	SOMETHING=test make
	printf "\tcell_size.bf:\n" >>log
	SOMETHING=test TEST_FILE=tests/cell_size.bf make
	cat log

testonce:
	printf "$(PREFIX)$(CELL_TYPE):\n" >>log
	out/$(PREFIX)$(CELL_TYPE) $(TEST_FILE)>>log
	printf "\n" >>log

clean:
	rm out/*
	rm log*
