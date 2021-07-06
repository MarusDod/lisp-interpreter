CC=gcc
cflags=-Wall -g -I
PROGRAM=prog
all=$(PROGRAM)

SRCDIR=.
OBJDIR=.

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
INCLUDES := $(wildcard $(SRCDIR)/*.h)


$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJECTS) 
	@echo "linking complete"

$(OBJECTS): $(SOURCES) $(INCLUDES)
	$(CC) -c $(SOURCES) $(CFLAGS)
	@echo "compiled successfully"$(OBJECTS)

clean:
	rm $(OBJECTS) $(PROGRAM)
	@echo "Cleanup complete"
