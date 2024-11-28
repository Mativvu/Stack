CXXFLAGS := -fsanitize=address,alignment -D _DEBUG -ggdb3 -Werror=vla \
		    -std=c++17  -O0  -Wall  -Wextra  -Weffc++  -Wc++14-compat \
		    -Wmissing-declarations   -Wcast-qual    -Wchar-subscripts \
		    -Wconversion  -Wctor-dtor-privacy   -Wempty-body   -Wfloat-equal \
		    -Wformat-nonliteral   -Wformat-security   -Wformat=2    -Winline \
		    -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith \
		    -Winit-self   -Wredundant-decls    -Wshadow    -Wsign-conversion \
		    -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default \
		    -Wswitch-enum -Wundef -Wunreachable-code -Wunused  -Wvariadic-macros \
		    -Wno-missing-field-initializers  -Wno-narrowing  -Wno-old-style-cast \
		    -Wno-varargs  -Wstack-protector   -fcheck-new   -fsized-deallocation \
		    -fstack-protector  -fstrict-overflow  -fno-omit-frame-pointer  -fPIE
CXX := gcc

BIN        := bin
SRC        := src
EXE        := main
INCLUDE    := include
OBJDIR     := build/obj
DEPDIR     := build/depends

CPP := $(SRC)/main.cpp       \
	   $(SRC)/Stack.cpp      \
	   $(SRC)/FlagParser.cpp \
	   $(SRC)/Commands.cpp   \
	   $(SRC)/Errors.cpp     \
	   $(SRC)/Color.cpp

OBJ := $(CPP:$(SRC)/%.cpp=$(OBJDIR)/%.o)
DEP := $(CPP:$(SRC)/%.cpp=$(DEPDIR)/%.d)

.PHONY: all run clean

all: $(BIN)/$(EXE)

run: clean all
	@echo "Executing..."
	./$(BIN)/$(EXE)

$(BIN)/$(EXE): $(OBJ)
	@echo "Building..."
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE) $^ -o $@

$(OBJDIR)/%.o: $(SRC)/%.cpp
	@$(CXX) -I$(INCLUDE) -c $< -o $@

-include $(DEP)

$(DEPDIR)/%.d: $(SRC)/%.cpp
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE) -MM -MT $(patsubst %.d,%.o,$(OBJDIR)/$*.o) -MF $@ $<

clean:
	@echo "Clearing..."
	-rm -f $(BIN)/* $(OBJ) $(DEP)

