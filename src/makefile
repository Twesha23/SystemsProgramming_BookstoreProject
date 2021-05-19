# you can put include files in this directory
IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

# directory where object files will be created
ODIR=obj
# you can put library files in this directory
LDIR =../lib
# looks for library libm.so or libm.a
LIBS=-lm

# create string with ../include/<header filename> in our case ../include/myheader.h
_DEPS = myheader.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# create string with obj/<object filename> in our case obj/book.o and obj/publication.o and obj/myfunctions.o (obj folder will be created under current i.e. source directory)
_LIBOBJ = book.o publication.o myfunctions.o
LIBOBJ = $(patsubst %,$(ODIR)/%,$(_LIBOBJ))
LIBSRC = book.c publication.c myfunctions.o


$(ODIR)/book.o: book.c
	$(CC) -c -fPIC -o $@ $< $(CFLAGS)

$(ODIR)/publication.o: publication.c
	$(CC) -c -fPIC -o $@ $< $(CFLAGS)
	
$(ODIR)/myfunctions.o: myfunctions.c
	$(CC) -c -fPIC -o $@ $< $(CFLAGS)

$(LDIR)/libmybookstore.so:$(LIBOBJ)
	$(CC) -shared -o $@ $^

$(ODIR)/server.o: server.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(ODIR)/client.o: client.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(ODIR)/localBookManagement.o: localBookManagement.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
server.out: $(ODIR)/server.o $(LDIR)/libmybookstore.so
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDIR)/libmybookstore.so -pthread
	
client.out: $(ODIR)/client.o $(LDIR)/libmybookstore.so
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDIR)/libmybookstore.so
	
localBookManagement.out: $(ODIR)/localBookManagement.o $(LDIR)/libmybookstore.so
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDIR)/libmybookstore.so
	
.PHONY: clean

# removes object files and temporary files created with ~ at the end of file name in current i.e. source and include directory
clean: 
	rm -f $(ODIR)/.o *~ core $(INCDIR)/~
	rm -f server.out client.out localBookManagement.out $(LDIR)/libmybookstore.so
	
.PHONY: all
all: $(LDIR)/libmybookstore.so server.out client.out localBookManagement.out
