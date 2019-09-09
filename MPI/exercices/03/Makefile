SOURCE = prog.c
TARGET = prog.out
BUFFER_SIZE = 1000
NUM_TESTS = 10
FILENAME = outfile.txt

all: $(TARGET)

$(TARGET): $(SOURCE)
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 2 $< $(NUM_TESTS) $(BUFFER_SIZE) $(FILENAME)

clean:
	find . -type f -not -name 'Makefile' -not -name $(SOURCE) -delete
