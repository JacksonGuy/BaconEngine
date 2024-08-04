CC = g++

CFLAGS = 
CFLAGS += -Iinclude/SFML/include/
CFLAGS += -Linclude/SFML/lib
CFLAGS += -DSFML_STATIC
CFLAGS += -Iinclude/imgui -Iinclude/imgui/backends
CFLAGS += -Iinclude/lua -Linclude/lua
CFLAGS += -Iinclude/nativefiledialog/src/include
CFLAGS += -Linclude/nativefiledialog/build/lib/Release/x64
#CFLAGS += -Iinclude/tracy

BUILD_DIR = bin

ifeq ($(target), editor_release)
	CFLAGS += -O3
	SOURCES += Program.cpp
else ifeq ($(target), editor_debug)
	CFLAGS += -g -Wall -Wextra -pedantic-errors
	SOURCES += Program.cpp
else ifeq ($(target), test)
	SOURCES += Test.cpp
else
	SOURCES += player.cpp
endif

SOURCES += $(wildcard src/*.cpp src/**/*.cpp)
SOURCES += include/imgui/imgui.cpp 
SOURCES += include/imgui/imgui_draw.cpp
SOURCES += include/imgui/imgui_tables.cpp 
SOURCES += include/imgui/imgui_widgets.cpp
SOURCES += include/imgui/imgui-SFML.cpp
SOURCES += include/imgui/imgui_demo.cpp
#SOURCES += include/tracy/TracyClient.cpp
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

UNAME = $(shell uname -s)
LIBS = 
EXE = 

ifeq ($(UNAME), Linux)
	LIBS += -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
	LIBS += -llua -lGL -lglfw
	LIBS += -lopenal32 -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg
	ifeq ($(target), editor_debug)
		EXE = editor
	else ifeq ($(target), editor_release)
		EXE = editor
	else ifeq ($(target), test)
		EXE = test
	else
		EXE = player
	endif
endif

ifeq ($(OS), Windows_NT)
	LIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s
	LIBS += -lwinmm -lgdi32 -lopengl32 -lglfw3 -lfreetype  -llua -lnfd -lcomctl32 -lole32 -luuid
	LIBS += -lopenal32 -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg
	ifeq ($(target), editor_debug)
		EXE = editor.exe
	else ifeq ($(target), editor_release)
		EXE = editor.exe
	else ifeq ($(target), test)
		EXE = test.exe
	else
		EXE = player.exe
	endif
endif

$(BUILD_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o : libs/imgui/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)
	@echo $(OBJS) 
	$(CC) -o $(EXE) $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf ./bin/src
	rm -rf ./*.exe