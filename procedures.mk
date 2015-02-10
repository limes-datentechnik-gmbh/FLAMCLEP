# CLE/P makefile(procedures)

$(BLDDIR_DEB)/%.o : %.c $(CLEMAN)
	@echo build $@
	@$(CC) -c $(DEB_OPT) $(DEPF) $(@:.o=.d) $(DEPO) $@ -o $@ $<

$(BLDDIR_REL)/%.o : %.c $(CLEMAN)
	@echo build $@
	@$(CC) -c $(REL_OPT) $(DEPF) $(@:.o=.d) $(DEPO) $@ -o $@ $<

.PHONY: any all release debug tools doc

any: debug

all: debug release doc

release: tools $(REL_TARGETS)

debug: tools $(DEB_TARGETS)


$(DEB_TARGETS) : | $(BINDIR_DEB)

$(BINDIR_DEB):
	mkdir -p $@

$(REL_TARGETS) : | $(BINDIR_REL)

$(BINDIR_REL):
	mkdir -p $@

$(DEB_OBJS) $(DEB_DEP): | $(BLDDIR_DEB)

$(BLDDIR_DEB):
	mkdir -p $@

$(REL_OBJS) $(REL_DEP): | $(BLDDIR_REL)

$(BLDDIR_REL):
	mkdir -p $@

tools:
	+make -C tools

$(CLEMAN): tools

# ----- DEBUG TARGETS ----------------------------------------------

$(BINDIR_DEB)/clptst$(BIN_EXT): $(CLPTST_DEB_OBJS)
	@echo linking $@
	@$(LD) $(DEB_LDF) -o $@ $(CLPTST_DEB_OBJS) $(LIBS)


# ----- RELEASE TARGETS --------------------------------------------

$(BINDIR_REL)/clptst$(BIN_EXT): $(CLPTST_REL_OBJS)
	@echo linking $@
	@$(LD) $(REL_LDF) -o $@ $(CLPTST_REL_OBJS) $(LIBS)
	@$(STRIP) $@

doc:
	+make -C doc


.PHONY: cleandep
cleandep:
	$(RM) -f $(DEB_DEP) $(REL_DEP)

.PHONY: clean
clean:
	$(RM) -f $(DEB_OBJS) $(DEB_TARGETS)
	$(RM) -f $(REL_OBJS) $(REL_TARGETS)

.PHONY: distclean
distclean:
	rm -rf bin build
	rm -f include/CLEMAN.h
	rm -f tools/text2cman

-include $(DEB_DEP) $(REL_DEP)
