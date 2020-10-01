EMACS_ROOT ?= ../..
EMACS ?= emacs

CC = gcc
CXX = g++
LD = g++
CPPFLAGS = -I$(EMACS_ROOT)/src -Ideps/simdjson
CXXFLAGS = -O2 -std=gnu++11 -ggdb3 -Wall -fPIC $(CPPFLAGS)
LDFLAGS =

.PHONY : test clean format

all: simdjson-core.so

simdjson-core.so: simdjson-core.o simdjson.o
	$(CXX) -shared $(LDFLAGS) -o $@ $^

simdjson-core.o: simdjson-core.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

simdjson.o: deps/simdjson/simdjson.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test:
	$(EMACS) -Q -batch -L . \
		-l test/test-simdjson.el \
		-f ert-run-tests-batch-and-exit

format:
	-clang-format -i simdjson-core.cpp

clean:
	-rm -f simdjson-core.so simdjson-core.o simdjson.o
