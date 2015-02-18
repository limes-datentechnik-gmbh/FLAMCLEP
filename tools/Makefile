# CLE/P tools makefile

CFLAGS = -Wall

CLE_DOCF = main     about  delprop  getowner  grammar  lexem    setowner  syntax     version    config  genprop
CLE_DOCF += getprop  help   manpage  setprop   trace    gendocu  commands  functions  main_help  main_syntax
CLE_DOCF += appendix_lexem  appendix_grammar  appendix_properties  appendix_version  appendix_about  glossary
CLE_DOCF += setenv   getenv delenv   license  chgprop  appendix_returncodes  appendix_reasoncodes errors

CLE_DOCFILES = $(addsuffix .txt, $(CLE_DOCF))

CLE_DOCS = $(addprefix ../doc/man_cle_, $(CLE_DOCFILES))

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
