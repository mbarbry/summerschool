COMP=gnu

ifeq ($(COMP),cray)
CC=cc
CCFLAGS=-O3 -I/appl/opt/libpng/include
LIBS=-L/appl/opt/libpng/lib -lpng -lz
endif

ifeq ($(COMP),gnu)
CC=/usr/bin/mpicc
CCFLAGS=-g -O0 -Wall -DMPI
LIBS=-lpng
endif

EXE=heat_equation
OBJS=pngwriter.o laplacian.o heat_equation.o 2d_array.o

all: $(EXE)

pngwriter.o: pngwriter.c pngwriter.h
laplacian.o: laplacian.c laplacian.h
2d_array.o: 2d_array.c laplacian.h 
heat_equation.o: heat_equation.c 

$(EXE): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE) a.out *.o *.png
