CC = gcc
INCLUDES = -I/usr/include/readline
LIBS = -lgdbm -lreadline -ltermcap
FLAGS = -Wall -O

DICTS = cmudict.0.6-2 cmudict.patch
TEXTFILES = words.txt rhymes.txt multiple.txt
DBFILES = words.db rhymes.db multiple.db

# RHYMEPATH = /usr/share/rhyme
# BINPATH = /usr/bin
# MANPATH = /usr/share/man/man1

RHYMEPATH = .
BINPATH = ./bin
MANPATH = ./man

OBJECTS = rhyme.o setup.o alloc.o getopt1.o getopt.o

all: words.db rhymes.db multiple.db rhyme

install: all
	install -m 755 rhyme $(BINPATH)
	install -m 755 -d $(RHYMEPATH)
	install -m 644 words.db $(RHYMEPATH)
	install -m 644 rhymes.db $(RHYMEPATH)
	install -m 644 multiple.db $(RHYMEPATH)
	install -m 644 rhyme.1 $(MANPATH)

uninstall:
	rm -f $(BINPATH)/rhyme
	rm -f $(MANPATH)/rhyme.1
	rm -rf $(RHYMEPATH)

clean:
	rm -f rhyme compile pronounce makedb merge fullmultiple *.o *.db

cleanall: clean
	rm -f *~

test: all
	python test.py < words.txt

compile: compile.c alloc.o longline.o
	$(CC) $(FLAGS) -o compile compile.c alloc.o longline.o -lgdbm

merge: merge.c merge.h longline.o alloc.o
	$(CC) $(FLAGS) -o merge merge.c alloc.o longline.o

getopt1.o: getopt1.c
	$(CC) $(FLAGS) -c getopt1.c

%.o: %.c %.h
	$(CC) $(FLAGS) -c $*.c

makedb: makedb.h makedb.c longline.o dict.o parts.o alloc.o
	$(CC) $(FLAGS) -o makedb makedb.c longline.o dict.o parts.o alloc.o

%.db: makedb compile merge $(DICTS)
	./merge $(DICTS) | ./makedb words.db rhymes.db multiple.db

#This is a program for easy lookup of pronunciations.
#It is meant to aid in building new pronunciations and is not used by
#the rhyming dictionary itself.
pronounce: pronounce.c pronounce.h rhyme.o pronounce.db fullmultiple.db
	$(CC) $(FLAGS) -o pronounce pronounce.c rhyme.o alloc.o \
$(INCLUDES) $(LIBS)

pronounce.db: $(DICTS) compile merge
	./merge $(DICTS) | ./compile - pronounce.db

fullmultiple: fullmultiple.c longline.o dict.o alloc.o parts.o
	$(CC) $(FLAGS) -o fullmultiple fullmultiple.c longline.o dict.o \
parts.o alloc.o

fullmultiple.db: $(DICTS) compile merge fullmultiple
	./merge $(DICTS) | ./fullmultiple | ./compile - fullmultiple.db

setup.o: setup.h setup.c
	$(CC) $(FLAGS) -c setup.c $(INCLUDES) -DDEFAULT_RHYMEPATH="\"$(RHYMEPATH)\""

rhyme.o: rhyme.h rhyme.c print.h print.c
	$(CC) $(FLAGS) -c rhyme.c $(INCLUDES)

rhyme: main.c main.h $(OBJECTS)
	$(CC) $(FLAGS) -o rhyme main.c $(OBJECTS) $(INCLUDES) $(LIBS)
