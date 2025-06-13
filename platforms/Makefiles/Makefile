CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I../../src -I../../external/YU2Engine -I../../external/SDL2 -I../../external
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

BUILD_DIR = ../../build
BIN_DIR = ../../bin

MAIN_SRC = ../../src/main.cpp
YU2ENGINE_SOURCES = $(wildcard ../../external/YU2Engine/core/*.cpp) \
                    $(wildcard ../../external/YU2Engine/graphics/*.cpp) \
                    $(wildcard ../../external/YU2Engine/input/*.cpp) \
                    $(wildcard ../../external/YU2Engine/resources/*.cpp)
GAME_SOURCES = $(wildcard ../../src/states/*.cpp) \
               $(wildcard ../../src/states/*/*.cpp)
EXTERNAL_SOURCES = ../../external/tinyxml2.cpp

ALL_SOURCES = $(MAIN_SRC) $(YU2ENGINE_SOURCES) $(GAME_SOURCES) $(EXTERNAL_SOURCES)

OBJECTS = $(ALL_SOURCES:../../%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/s2hdpp

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src
	mkdir -p $(BUILD_DIR)/src/states
	mkdir -p $(BUILD_DIR)/external/YU2Engine/core
	mkdir -p $(BUILD_DIR)/external/YU2Engine/graphics
	mkdir -p $(BUILD_DIR)/external/YU2Engine/input
	mkdir -p $(BUILD_DIR)/external/YU2Engine/resources
	mkdir -p $(BUILD_DIR)/external

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) $(LIBS) -o $@

$(BUILD_DIR)/%.o: ../../%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
