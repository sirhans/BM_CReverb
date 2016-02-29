CC=c99
CFLAGS=-lm

ODIR=obj
LDIR =../lib

LIBS=-lm

DEPS = BMCReverb.h BMCrossPlatformVDSP.h
#DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o BMCReverb.o BMCrossPlatformVDSP.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

creverb: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
