# prog: objs
# 	g++ -o ./out/trombone ./src/main.cpp ./out/obj/* -lportaudio

# objs: global lipmodel lowpass lowpass trombone tube MainComponent

# global: ./src/global.cpp
# 	g++ ./src/global.cpp -c -o ./out/obj/global.o

# lipmodel: ./src/lipmodel.cpp
# 	g++ ./src/lipmodel.cpp -c -o ./out/obj/lipmodel.o

# lowpass: ./src/lowpass.cpp
# 	g++ ./src/lowpass.cpp -c -o ./out/obj/lowpass.o

# trombone: ./src/trombone.cpp
# 	g++ ./src/trombone.cpp -c -o ./out/obj/trombone.o

# tube: ./src/tube.cpp
# 	g++ ./src/tube.cpp -c -o ./out/obj/tube.o

# MainComponent: ./src/MainComponent.cpp
# 	g++ ./src/MainComponent.cpp -c -o ./out/obj/MainComponent.o

# main: ./src/main.cpp
# 	g++ ./src/main.cpp -c -o ./out/obj/main.o

# clean:
# 	rm -r -f ./out/

# Variables
CXX = g++
CXXFLAGS = -Wall -g
SRC_DIR = src
OBJ_DIR = out/obj
BIN_DIR = out/bin
SOURCES = $(wildcard $(SRC_DIR)/*.cpp gui/*.cpp)
SRCS = $(filter-out src/main.cpp gui/main.cpp, $(SOURCES))
OBJECTS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
INCLUDES = $(wildcard /usr/include/x86_64-linux-gnu/qt5/*)
INCLUDES_GCC = $(addprefix -I, $(INCLUDES))

# Default target
all: directories program

ui: directories ui-program

# Rule to make the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to make the program
program: $(OBJECTS) src/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@ -lportaudio

ui-program: $(OBJECTS) gui/main.cpp
	$(CXX) $(CXXFLAGS) -fPIC -I$(INCLUDES_GCC) -I/usr/include/x86_64-linux-gnu/qt5/ $^ -o $(BIN_DIR)/$@ -lportaudio -lQt5Core -lQt5Gui -lQt5Widgets -lQt5PrintSupport -lfftw3

# Rule to make the necessary directories
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Rule to clean the build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories clean