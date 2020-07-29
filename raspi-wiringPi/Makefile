CC = g++
CFLAGS = -I ..

all: rxdec

Decoder.o: ../Decoder.cpp ../Decoder.h
	$(CC) $(CFLAGS) -c $< -o $@

rxdec.o: rxdec.cpp ../Decoder.h
	$(CC) $(CFLAGS) -c $< -o $@

rxdec: rxdec.o Decoder.o
	$(CC) $(CFLAGS) -o $@ $^ -lwiringPi

clean:
	rm -f *.o core *~ 
