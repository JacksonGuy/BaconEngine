cc = g++

CFLAGS = -Iextern/raylib/src -Lextern/raylib/src
CFLAGS += -Iextern/box2d/include -Lextern/box2d/build/src

BUILD_DIR = bin

SOURCES = $(wildcard src/*.cpp src/**/*.cpp)
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

LINK = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LINK += -lbox2d

EXE = main

$(BUILD_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(cc) $(CFLAGS) -c -o $@ $<

all: $(OBJS)
	$(cc) -o $(EXE) $^ $(CFLAGS) $(LINK)

clean:
	rm -rf ./bin/src
	rm -rf ./bin/*.o
