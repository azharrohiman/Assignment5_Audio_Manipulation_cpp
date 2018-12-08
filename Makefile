CC=g++

samp: Audio.h driver.cpp
	$(CC) -o samp driver.cpp -std=c++11 -g

clean:
	rm -rf *.o samp
