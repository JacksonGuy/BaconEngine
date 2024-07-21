CC = g++
CFLAGS = -O3 -g
CLFAGS += -Iinclude/SFML/include/
CFLAGS += -Linclude/SFML/lib
CFLAGS += -DSFML_STATIC
CFLAGS += -Iinclude/imgui -Iinclude/imgui/backends
CFLAGS += -Iinclude/lua -Linclude/lua
CFLAGS += -Iinclude/nativefiledialog/src/include
CFLAGS += -Linclude/nativefiledialog/build/lib/Release/x64

BUILD_DIR = bin

ifeq ($(target), editor)
	SOURCES += Program.cpp
else ifeq ($(target), test)
	SOURCES += Test.cpp
else
	SOURCES += player.cpp
endif

SOURCES += $(wildcard src/*.cpp src/**/*.cpp)
#SOURCES += $(wildcard src/Lua/*.cpp)
SOURCES += include/imgui/imgui.cpp 
SOURCES += include/imgui/imgui_draw.cpp
SOURCES += include/imgui/imgui_tables.cpp 
SOURCES += include/imgui/imgui_widgets.cpp
SOURCES += include/imgui/imgui-SFML.cpp
SOURCES += include/imgui/imgui_demo.cpp
# OBJS = $(addsuffix .o, $(basename $(SOURCES)))
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

UNAME = $(shell uname -s)
LIBS = 
EXE = 

ifeq ($(UNAME), Linux)
	LIBS += -lGL -lglfw -lsfml-graphics -lsfml-window -lsfml-system
	ifeq ($(target), editor)
		EXE = editor
	else ifeq ($(target), test)
		EXE = test
	else
		EXE = player
	endif
endif

ifeq ($(OS), Windows_NT)
	LIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lglfw3 -lfreetype  -llua -lnfd -lcomctl32 -lole32 -luuid
	ifeq ($(target), editor)
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
	rm -rf ./bin
	rm -rf ./*.exe
