#!/bin/sh
##compile first

SRCDIR	= src
OBJDIR	= obj
BINDIR	= bin

CXX 	= g++

FLAGS   = -std=c++1z -O3

## all the source files 
ALLSRC	= $(sort $(wildcard ${SRCDIR}/*.cpp))

## all the main function
UTILSRC = $(sort $(wildcard ${SRCDIR}/main_*.cpp))

## get all the names of the programs 
UTILS   = $(patsubst ${SRCDIR}/main_%.cpp, %, ${UTILSRC})

## substract the main functions from all the source files
SOURCES	= $(filter-out ${UTILSRC}, ${ALLSRC})

OBJECTS	= $(patsubst ${SRCDIR}/%.cpp, ${OBJDIR}/%.o, ${SOURCES})

all: ${UTILS}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	${CXX} -c $< -o $@ ${FLAGS}
		
$(addprefix ${BINDIR}/, ${UTILS}): ${OBJECTS}
	$(eval PADSRC = $(shell printf "%-32s" "${SRCDIR}/main_$(@F).cpp"))
	$(eval PADOBJ = $(shell printf "%-23s" "$@"))
	${CXX} ${PADSRC} ${OBJECTS} -o ${PADOBJ} ${FLAGS}

.SECONDEXPANSION:

${UTILS}: ${BINDIR}/$$@

clean:
	rm -f ${OBJDIR}/*
	rm -f ${BINDIR}/*

rebuild: clean all