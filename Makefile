C-FLAGS=-Wall -Wextra -Wpedantic -O3
SRC=./src
DST=./bin

all: build

build: | bin
	gcc $(C-FLAGS) $(SRC)/main.c -o $(DST)/turing

bin:
	mkdir ./bin
