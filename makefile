SHELL := bash

SOURCES := $(wildcard src/*.c)
DLLS := $(patsubst src/%.c,build/%.dll,$(SOURCES))

all: build $(DLLS)

build:
	mkdir -p build

build/%.dll: src/%.c | build
	gcc -shared -o $@ $<

clean:
	rm -rf build

.PHONY: all clean