cc = g++

CFLAGS = -ggdb
CFLAGS += -Iextern/raylib/src -Lextern/raylib/src
CFLAGS += -Iextern/box2d/include -Lextern/box2d/
CFLAGS += -Iextern/rlImGui
CFLAGS += -Iextern/imgui -Iextern/imgui/backends

BUILD_DIR = bin

SOURCES = $(wildcard src/*.cpp src/**/*.cpp)
# SOURCES += $(wildcard extern/box2d/src/*.c)
SOURCES += extern/rlImGui/rlImGui.cpp
SOURCES += extern/imgui/imgui.cpp 
SOURCES += extern/imgui/imgui_draw.cpp
SOURCES += extern/imgui/imgui_tables.cpp 
SOURCES += extern/imgui/imgui_widgets.cpp
SOURCES += extern/imgui/imgui_demo.cpp
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

LINK = -lraylib -lgdi32 -lwinmm
LINK += -lbox2d -pthread

EXE = editor

$(BUILD_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(cc) $(CFLAGS) -c -o $@ $<

all: $(OBJS)
	$(cc) -o $(EXE) $^ $(CFLAGS) $(LINK)

clean:
	rm -rf ./bin/src
	rm -rf ./bin/*.o
	rm -rf ./editor
	rm -rf ./editor.exe

cleanall:
	rm -rf ./bin
	rm -rf ./editor
	rm -rf ./editor.exe