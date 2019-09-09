SOURCE = prog.c
TARGET = prog.out
FILENAME = outfile.txt

all: $(TARGET)

$(TARGET): $(SOURCE)
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 5 $< $(FILENAME)

clean:
	find . -type f -not -name 'Makefile' -not -name $(SOURCE) -delete
