CC = clang++
CFLAGS = -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda

EXECUTABLE = main

SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

clean:
	rm -f $(EXECUTABLE)

all:
	clear
	echo "compile started"
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)
	echo "compile done"
	echo "Executing program"
	/usr/bin/time ./$(EXECUTABLE)

run:
	./$(EXECUTABLE)

compile:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)


compile-debug:
	$(CC) $(CFLAGS) -g $(SOURCES) -o $(EXECUTABLE)
