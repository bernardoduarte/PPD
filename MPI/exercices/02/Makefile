TARGET = prog.out
FILENAME = outfile.txt

all: $(TARGET)

$(TARGET): prog.c
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 5 $< $(FILENAME)

clean: $(TARGET) $(wildcard $(FILENAME))
	rm $^
