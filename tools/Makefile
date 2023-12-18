# CLE/P tools makefile

CFLAGS = -Wall

CLE_DOCS ::= $(shell ls ../doc/man_clep_*.txt)

CINC = ../include/CLEMAN.h

HEADER = $(CINC)

PROGS = text2cman

CFLAGS = -Wall

all: $(HEADER)
.PHONY: all


../include/CLEMAN.h: $(CLE_DOCS) text2cman
	@echo generate $@
	@./text2cman -q -o $@ $(CLE_DOCS)


text2cman$(BIN_EXT): text2cman.c
	@echo build tool $@
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f $(HEADER) $(PROGS)
