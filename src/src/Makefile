# CONVERT program
# first written by Paul Rayson 1993, last updated 2009

# program name
PROGRAM = convert

# change this to add/remove debugging reports
# CFLAGS = -g
CFLAGS = -Ddebug -g

# Directories for my standard header files EDIT THESE THREE LINES
HEADIR = ../include
LIBDIR = ../lib
BINDIR = ../bin

# Compile command
COMPILE = gcc $(CFLAGS) -I$(HEADIR)

LIBOBJS =	$(LIBDIR)/genutil.o $(LIBDIR)/claws_out.o \
	$(LIBDIR)/semtag_text.o $(LIBDIR)/bnc_text.o $(LIBDIR)/sgml_text.o

OBJS =	regexp.o bnc_text.o horiz.o init.o lemma.o map.o parse.o semantic.o util.o vertical.o xkwic.o main.o

$(PROGRAM):	$(OBJS) $(LIBOBJS) 
	$(COMPILE) -o $(PROGRAM) $(OBJS) $(LIBOBJS) 

$(OBJS):	$(PROGRAM).h $(HEADIR)/util.h
	$(COMPILE) -o $*.o -c $*.c

$(LIBOBJS):	$(HEADIR)/util.h
	$(COMPILE) -o $*.o -c $*.c

