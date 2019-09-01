TARGET = prog.out
FILENAME = outfile.txt

all: $(TARGET)

$(TARGET): prog.c
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 8 $< 10 $(FILENAME)

clean: $(TARGET) $(FILENAME)
	rm $^
