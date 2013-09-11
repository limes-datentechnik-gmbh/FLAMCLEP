# CLE/P makefile(linux)

BLDDIR_DEB = build/debug
BLDDIR_REL = build/release

BINDIR_DEB = bin/debug
BINDIR_REL = bin/release

INC_OPT = -I include

CFLAGS= -Wall -fPIC -m32
DEB_OPT = $(CFLAGS) -g -O0 -D__UNIX__ -D__XLIN__ -D__DEBUG__  $(INC_OPT)
REL_OPT = $(CFLAGS) -O3 -D__UNIX__ -D__XLIN__ -D__RELEASE__ $(INC_OPT)
DEBGEN_FLAGS = -MM -MG -MT

VPATH = src:include

DEB_LDF = -g -m32
REL_LDF = -m32

STRIP = strip
CC = gcc
LD = gcc

LIBS = -lm


include objects.mk

include procedures.mk
