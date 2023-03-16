#####################################
# Source and output
#####################################
# Output binary
BIN := main

# Source directory
SRCDIR := .

# Output directory
BUILDDIR := bin

# Intermediate directory for generated object files
OBJDIR := $(BUILDDIR)/.obj

# Intermediate directory for generated dependency files
DEPDIR := $(BUILDDIR)/.dep

# Source files
SRCS := $(shell find $(SRCDIR) -type f \( -name \*.c -o -name \*.cpp \))

# Object files, auto generated from source files
OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))

# Dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

# Create the object and dependencies directories
$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)


#####################################
# Compiler settings
#####################################
# C compiler
CC := gcc
# C++ compiler
CXX := g++
# linker
LD := g++

# C flags
CFLAGS := -std=c11
# C++ flags
CXXFLAGS := -std=c++17
# C/C++ flags
CPPFLAGS := -Wall -Wextra -pedantic -g -DDEBUG -O0

# Linker flags
LDFLAGS :=
# Linker flags: libraries to link (e.g. -lfoo)
LDLIBS :=
# Flags required for dependency generation
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td


#####################################
# Compilation macros
#####################################
# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link object files to binary
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) -o $@
# precompile step
PRECOMPILE =
# postcompile step
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d


#####################################
# Targets
#####################################
$(BUILDDIR)/$(BIN): $(OBJS)
	$(LINK.o) $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cc
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cxx
$(OBJDIR)/%.o: %.cxx $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

run: $(BUILDDIR)/$(BIN)
	$<

-include $(DEPS)