CXXFLAGS = -O3 -Wall -pedantic -std=c++17
LDFLAGS =

all: b64fix

clean:
	rm -f b64fix

.PHONY: all clean
