CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 
PROJECTNAME = project.exe
MODULENAME = blockbyte.so
OUTPUT_DIR = bin

# Define Box2D paths
BOX2D_INCLUDE_DIR = thirdParty/Box2D/include
BOX2D_LIB_DIR = thirdParty/Box2D/build/bin

INCLUDE_DIR = -ISDL3 -Iinclude -IthirdParty/imgui/include -I$(BOX2D_INCLUDE_DIR)
LIB_DIRS = -Llib -L$(BOX2D_LIB_DIR)
LIBS = -lSDL3 -lbox2d `python3 -m pybind11 --includes`
SRC = $(wildcard src/*.cpp) $(wildcard thirdParty/imgui/src/*.cpp)

all: $(MODULENAME)

$(PROJECTNAME): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUTPUT_DIR)/$(PROJECTNAME) $(INCLUDE_DIR) $(LIB_DIRS) $(LIBS)
	install_name_tool -change @rpath/libSDL3.1.0.0.dylib /usr/local/lib/pkgconfig/../../lib/libSDL3.dylib $(OUTPUT_DIR)/$(PROJECTNAME)  

$(MODULENAME): $(SRC)
	$(CXX) $(CXXFLAGS) -undefined dynamic_lookup -shared -fPIC $(INCLUDE_DIR) $(LIB_DIRS) $(LIBS) -o ./bin/$(MODULENAME) $(SRC)
	install_name_tool -change @rpath/libSDL3.1.0.0.dylib /usr/local/lib/pkgconfig/../../lib/libSDL3.dylib $(OUTPUT_DIR)/$(MODULENAME) 

.PHONY: clean
clean:
	find $(OUTPUT_DIR) -type f ! -name '*.py' -delete

.PHONY: run
run: $(PROJECTNAME)
	./$(OUTPUT_DIR)/$(PROJECTNAME)