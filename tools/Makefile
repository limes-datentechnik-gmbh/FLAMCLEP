# CLE/P tools makefile

CFLAGS = -Wall

include docFiles.mk

CLE_DOCS = $(addprefix ../doc/man_clep_, $(CLE_DOCFILES))

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
