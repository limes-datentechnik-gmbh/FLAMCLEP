
ZIPDIR = ../DOCZIP
PDFDIR = ../DOCPDF
SUBDIRS = CLEPAPI FLAMCLEP

DOXYFILES = $(addsuffix .doxy, $(SUBDIRS))
PDFFILES = $(addsuffix .pdf, $(SUBDIRS))
ZIPFILES = $(addsuffix .zip, $(SUBDIRS))
ALLZIPS = $(addprefix $(ZIPDIR)/, $(ZIPFILES))
ALLPDFS = $(addprefix $(PDFDIR)/, $(PDFFILES))

$(ZIPDIR)/%.zip: %.doxy
	@echo generate zipped docs in $@ ...
	doxygen $<
	zip -r $@ $(<:.doxy=)/html


.PHONY: all clean

all: $(ALLZIPS)

$(ALLZIPS): | $(ZIPDIR) $(PDFDIR)

$(ZIPDIR):
	mkdir -p $@

$(PDFDIR):
	mkdir -p $@

clean:
	rm -rf $(SUBDIRS) $(ALLZIPS) $(ALLPDFS)
	-rmdir $(ZIPDIR) $(PDFDIR)

$(ZIPDIR)/CLEPAPI.zip: ../include/FLAMCLE.h ../include/FLAMCLP.h ../include/CLEDEF.h ../include/CLPDEF.h ../include/CLEMSG.h ../include/CLPMAC.h ../include/CLEPUTL.h
