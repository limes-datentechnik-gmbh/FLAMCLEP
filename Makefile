# CLE/P makefile(linux)

BLDDIR_DEB = build/debug
BLDDIR_REL = build/release

BINDIR_DEB = bin/debug
BINDIR_REL = bin/release

INC_OPT = -I include

CFLAGS= -Wall -fPIC -std=c99
DEB_OPT = $(CFLAGS) -g -O0 -D__UNIX__ -D__XLIN__ -D__DEBUG__  $(INC_OPT)
REL_OPT = $(CFLAGS) -O3 -D__UNIX__ -D__XLIN__ -D__RELEASE__ $(INC_OPT)
DEPF = -MMD -MF
DEPO = -MT

VPATH = src:include

DEB_LDF = -g
REL_LDF =

STRIP = strip
ifeq ($(COMPILER),gnu)
CC = gcc
else
CC = clang
endif
LD = $(CC)

LIBS = -lm


include objects.mk

include procedures.mk
