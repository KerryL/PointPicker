# makefile (PointPicker)
#
# Include the common definitions
include makefile.inc

# Name of the executable to compile and link
TARGET = pointPicker
TARGET_D = $(TARGET)_debug

# Directories in which to search for source files
DIRS = \
	src \

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
VERSION_FILE = src/gitHash.cpp

# Object files
TEMP_OBJS = $(addprefix $(OBJDIR_REL),$(SRC:.cpp=.o))
TEMP_OBJS_D = $(addprefix $(OBJDIR_DBG),$(SRC:.cpp=.o))
VERSION_FILE_OBJ = $(OBJDIR_REL)$(VERSION_FILE:.cpp=.o)
VERSION_FILE_OBJ_D = $(OBJDIR_DBG)$(VERSION_FILE:.cpp=.o)
OBJS = $(filter-out $(VERSION_FILE_OBJ),$(TEMP_OBJS))
OBJS_D = $(filter-out $(VERSION_FILE_OBJ_D),$(TEMP_OBJS_D))

.PHONY: all clean

all: $(TARGET_D)
release: $(TARGET)

$(TARGET_D): $(OBJS_D) version_dbg
	$(MKDIR) $(BINDIR)
	$(CC) $(OBJS_D) $(VERSION_FILE_OBJ_D) $(LDFLAGS) -o $(BINDIR)$@

$(TARGET): $(OBJS) version_rel
	$(MKDIR) $(BINDIR)
	$(CC) $(OBJS) $(VERSION_FILE_OBJ) $(LDFLAGS) -o $(BINDIR)$@

$(OBJDIR_DBG)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_D) -c $< -o $@

$(OBJDIR_REL)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR_TST)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

version_rel:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ))
	$(CC) $(CFLAGS) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ)

version_dbg:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ_D))
	$(CC) $(CFLAGS_D) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ_D)

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(BINDIR)$(TARGET_D)
	$(RM) $(BINDIR)$(TARGET)
	$(RM) $(VERSION_FILE)

