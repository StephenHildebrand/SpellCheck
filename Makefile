# Compiler for the default rule to use.
CC = gcc
# Compile options for the default rule.
CFLAGS = -Wall -std=c99

# Build the spellcheck program as the default target and
spellcheck: spellcheck.o text.o
spellcheck.o: spellcheck.c text.h # spellcheck.o dependencies.
text.o: text.c text.h # text.o dependencies.

# Target that deletes temporary files made during build or by tests.
clean:  # Only run when explicitly called on command line as a target.
	rm -f *.o
	rm -f spellcheck
	rm -f output.txt
