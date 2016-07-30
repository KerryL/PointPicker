# makefile.inc (PointPicker)
# This file contains all of the environment definitions
# common to each of the makefiles within the project. 
#

# Include directories that are not already on the path
# DO NOT include the -I prefix to these paths - it will
# be added automatically
INCDIRS_TEMP = \
	$(CURDIR)/src/

INCDIRS = $(addprefix -I,$(INCDIRS_TEMP))

# For 3rd party files - will be included as system files so warnings
# are ignored.  These macros must be defined in your bashrc file.
LIB_INCDIRS_TEMP = \
	$(EIGEN)

LIB_INCDIRS = $(addprefix -isystem,$(LIB_INCDIRS_TEMP))

# Library directories that are not already on the path
# DO NOT include the -L prefix to these paths - it will
# be added automatically
LIBDIRS_TEMP = \
	

LIBDIRS = $(addprefix -L,$(LIBDIRS_TEMP))

# Libraries to link against
# DO NOT include the -l prefix to these libraries - it
# will be added automatically
LIBS_TEMP = \
	

LIBS = $(addprefix -l,$(LIBS_TEMP))

# Static libraries to be built before the executable
# MUST be listed in order of dependence (i.e. first
# library must not be needed by other libraries and
# it must need information contained in the following
# libraries).
PSLIB = \
	

# Compiler to use
CC = g++

# Archiver to use
AR = ar rcs
RANLIB = ranlib

# Library output directory
LIBOUTDIR = $(CURDIR)/lib/

# Compiler flags
CFLAGS = -Wall -Wextra $(LIB_INCDIRS) $(INCDIRS) -std=c++11 -pedantic -pthread `wx-config --version="3.1" --cflags`
CFLAGS_D = -g $(CFLAGS)

# Linker flags
LDFLAGS = $(PSLIB) $(SLIBS) $(LIBDIRS) $(LIBS) `wx-config --version="3.1" --libs all`

# Object file output directory
OBJDIR = $(CURDIR)/.obj/
OBJDIR_REL = $(OBJDIR)release/
OBJDIR_DBG = $(OBJDIR)debug/

# Binary file output directory
BINDIR = $(CURDIR)/bin/

# Method for creating directories
MKDIR = mkdir -p

# Method for removing files
RM = rm -f
