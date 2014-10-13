prefix  = /usr/local
bindir  = $(prefix)/bin
man1dir = $(prefix)/share/man/man1

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -g
LD      = gcc
LDFLAGS =
INSTALL = install

objects = mcp3008.o cli.o
target  = mcp3008

$(target): $(objects)
	$(LD) $(LDFLAGS) -o $@ $^

install:
	$(INSTALL) -m755 -t $(bindir) $(target)
	$(INSTALL) -m644 -t $(man1dir) mcp3008.1

clean:
	rm -f $(objects) $(target)
