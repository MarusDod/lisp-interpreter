CC=gcc
cflags=-Wall -g -I 
LIBS=-ldl -lreadline -export-dynamic
PROGRAM=prog
BUILDDIR=build
all=$(PROGRAM)

SRCDIR=.

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
INCLUDES := $(wildcard $(SRCDIR)/*.h)


$(BUILDDIR)/$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS)  $^ -o $@ $(LIBS)
	@echo "linking complete"

$(BUILDDIR)/%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $<  -o $@

clean:
	rm -f $(OBJECTS) *.o $(BUILDDIR)/$(PROGRAM)
	@echo "Cleanup complete"
