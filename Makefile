cc = g++

CFLAGS = -ggdb
CFLAGS += -Iextern/raylib/src -Lextern/raylib/src
CFLAGS += -Iextern/box2d/include -Lextern/box2d/
CFLAGS += -Iextern/rlImGui
CFLAGS += -Iextern/imgui -Iextern/imgui/backends
CFLAGS += -Iextern/nativefiledialog-extended/src/include
#CFLAGS += -Lextern/nativefiledialog-extended/build/src/Debug
CFLAGS += -Iextern/json
CFLAGS += -Iextern/stb_image
CFLAGS += -Iextern/lua/src -Lextern/lua/src
CFLAGS += -Iextern/sol2/include

BUILD_DIR = bin

SOURCES = $(wildcard src/*.cpp src/**/*.cpp)
SOURCES += extern/nativefiledialog-extended/src/nfd_win.cpp
SOURCES += extern/rlImGui/rlImGui.cpp
SOURCES += extern/imgui/imgui.cpp 
SOURCES += extern/imgui/imgui_draw.cpp
SOURCES += extern/imgui/imgui_tables.cpp 
SOURCES += extern/imgui/imgui_widgets.cpp
SOURCES += extern/imgui/imgui_demo.cpp
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

LINK = -lraylib -lgdi32 -lwinmm
LINK += -lole32 -luuid -lshell32
LINK += -lbox2d -pthread -llua

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