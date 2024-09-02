CXXFLAGS = -O3 -Wall -Wextra -pedantic -std=c++23
LDFLAGS =

all: b64fix

clean:
	rm -f b64fix

.PHONY: all clean
