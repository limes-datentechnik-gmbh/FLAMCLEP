# CLE/P makefile(objects)


OBJECTS = FLAMCLE.o FLAMCLP.o CLEPUTLC.o


INTERNAL_OBJS = CLPTST.o $(OBJECTS)

ALL_OBJS = $(INTERNAL_OBJS)
DEB_DEP =  $(addprefix $(BLDDIR_DEB)/, $(ALL_OBJS:.o=.d))
REL_DEP =  $(addprefix $(BLDDIR_REL)/, $(ALL_OBJS:.o=.d))
DEB_OBJS = $(addprefix $(BLDDIR_DEB)/, $(ALL_OBJS))
REL_OBJS = $(addprefix $(BLDDIR_REL)/, $(ALL_OBJS))


CLPTST_OBJS = CLPTST.o $(OBJECTS)
CLPTST_DEB_OBJS = $(addprefix $(BLDDIR_DEB)/, $(sort $(CLPTST_OBJS)))
CLPTST_REL_OBJS = $(addprefix $(BLDDIR_REL)/, $(sort $(CLPTST_OBJS)))

TARGETS = clptst

DEB_TARGETS = $(addprefix $(BINDIR_DEB)/, $(sort $(TARGETS)))
REL_TARGETS = $(addprefix $(BINDIR_REL)/, $(sort $(TARGETS)))

