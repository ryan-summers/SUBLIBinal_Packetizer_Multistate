#This is the makefile for the multistate packetizer.

CC = g++
CFLAGS = -g -Wall 
SRC = src/
INC = ./

all : Serial.o Packetizer.o

Serial.o: src/Serial.cpp
	$(CC) -c -I$(INC) $(CFLAGS) $^ -o $@

Packetizer.o : src/Packetizer.cpp	
	$(CC) -c -I$(INC) $(CFLAGS) $^ -o $@
