# CLE/P makefile(procedures)

$(BLDDIR_DEB)/%.d: %.c
	@set -e; rm -f $@; \
	echo generate $@; \
	$(CC) $(INC_OPT) $(DEB_OPT) $(DEBGEN_FLAGS) $(@:.d=.o) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BLDDIR_REL)/%.d: %.c
	@set -e; rm -f $@; \
	echo generate $@; \
	$(CC) $(INC_OPT) $(REL_OPT) $(DEBGEN_FLAGS) $(@:.d=.o) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BLDDIR_DEB)/%.o : %.c
	@echo build $@
	@$(CC) -c $(DEB_OPT) -o $@ $<

$(BLDDIR_REL)/%.o : %.c
	@echo build $@
	@$(CC) -c $(REL_OPT) -o $@ $<

.PHONY: all release debug tools

all: debug release

release: $(REL_TARGETS)

debug: $(DEB_TARGETS)


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

include/CLEMAN.h: tools

# ----- DEBUG TARGETS ----------------------------------------------

$(BINDIR_DEB)/clptst$(BIN_EXT): $(CLPTST_DEB_OBJS)
	@echo linking $@
	$(LD) $(DEB_LDF) -o $@ $(CLPTST_DEB_OBJS) $(LIBS)


# ----- RELEASE TARGETS --------------------------------------------

$(BINDIR_REL)/clptst$(BIN_EXT): $(CLPTST_REL_OBJS)
	@echo linking $@
	$(LD) $(REL_LDF) -o $@ $(CLPTST_REL_OBJS) $(LIBS)
	@$(STRIP) $@


.PHONY: cleandep
cleandep:
	$(RM) -f $(DEB_DEP) $(REL_DEP)

.PHONY: clean
clean:
	$(RM) -f $(DEB_OBJS) $(DEB_TARGETS)
	$(RM) -f $(REL_OBJS) $(REL_TARGETS)

-include $(DEB_DEP) $(REL_DEP)
