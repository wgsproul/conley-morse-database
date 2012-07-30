# makefile for CMDP project 
include makefile.config

COMPUTE_MORSE_SETS := yes
COMPUTE_CONLEY_INDEX := no
PARAM_SPACE_METHOD := PATCHMETHOD

CXXFLAGS += -D $(PARAM_SPACE_METHOD)

.PHONY: all
all: Conley_Morse_Database

# pattern rule for compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# List of targets
all: Conley_Morse_Database

DATABASE := ./build/structures/Database.o
DATABASE += ./build/program/Conley_Morse_Database.o 

ifeq ($(COMPUTE_MORSE_SETS),yes)
	DATABASE += ./build/program/MorseProcess.o 
	CXXFLAGS += -D COMPUTE_MORSE_SETS
endif

ifeq ($(COMPUTE_CONLEY_INDEX),yes)
	DATABASE += ./build/program/ConleyProcess.o
	CXXFLAGS += -D COMPUTE_CONLEY_INDEX
endif


Conley_Morse_Database: $(DATABASE)
	$(CC) $(LDFLAGS) $(DATABASE) -o $@ $(LDLIBS)

EXPORTXML := ./build/program/ExportXML.o
EXPORTXML += ./build/structures/Database.o
EXPORTXML += ./build/structures/XMLExporter.o

ExportXML: $(EXPORTXML)
	$(CC) $(LDFLAGS) $(EXPORTXML) -o $@ $(LDLIBS)

SINGLECMG := ./build/test/SingleCMG.o 
SINGLECMG += ./build/tools/picture.o 
SINGLECMG += ./build/tools/lodepng/lodepng.o
SingleCMG: $(SINGLECMG)
	$(CC) $(LDFLAGS) $(SINGLECMG) -o $@ $(LDLIBS)

HENONCMG := ./build/test/HenonCMG.o
HENONCMG += ./build/tools/picture.o
HENONCMG += ./build/tools/lodepng/lodepng.o
HenonCMG: $(HENONCMG)
	$(CC) $(LDFLAGS) $(HENONCMG) -o $@ $(LDLIBS)

NEWTONCMG := ./build/test/NewtonCMG.o
NEWTONCMG += ./build/tools/picture.o
NEWTONCMG += ./build/tools/lodepng/lodepng.o
NewtonCMG: $(NEWTONCMG)
	$(CC) $(LDFLAGS) $(NEWTONCMG) -o $@ $(LDLIBS)

SUBMAPTEST := ./build/test/SubdividedMapTest.o
SUBMAPTEST += ./build/tools/picture.o
SUBMAPTEST += ./build/tools/lodepng/lodepng.o
SubdividedMapTest: $(SUBMAPTEST)
	$(CC) $(LDFLAGS) $(SUBMAPTEST) -o $@ $(LDLIBS)

# Cleanup
 .PHONY: clean
clean:
	find ./build -name "*.o" -delete
	find ./build -name "*.so" -delete
	rm -f Conley_Morse_Database
	rm -f SingleCMG
	rm -f ExportXML
	rm -f NewtonCMG


# Create build directories
.PHONY: build-dirs
dirs:
	mkdir build
	mkdir build/program
	mkdir build/structures
	mkdir build/test
	mkdir build/tools
	mkdir build/tools/lodepng
