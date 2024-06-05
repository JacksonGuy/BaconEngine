CC = g++
CFLAGS = -O3 -g
CLFAGS += -Iinclude/SFML/include/
CFLAGS += -Linclude/SFML/lib
CFLAGS += -DSFML_STATIC
CFLAGS += -Iinclude/imgui -Iinclude/imgui/backends

ifeq ($(target), editor)
	SOURCES += editor.cpp
else
	SOURCES += player.cpp
endif

SOURCES += $(wildcard src/*.cpp)
SOURCES += include/imgui/imgui.cpp 
SOURCES += include/imgui/imgui_draw.cpp
SOURCES += include/imgui/imgui_tables.cpp 
SOURCES += include/imgui/imgui_widgets.cpp
SOURCES += include/imgui/imgui-SFML.cpp
OBJS = $(addsuffix .o, $(basename $(SOURCES)))

UNAME = $(shell uname -s)
LIBS = 
EXE = 

ifeq ($(UNAME), Linux)
	LIBS += -lGL -lglfw -lsfml-graphics -lsfml-window -lsfml-system
	ifeq ($(target), editor)
		EXE = editor
	else
		EXE = player
	endif
endif

ifeq ($(OS), Windows_NT)
	LIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lglfw3 -lfreetype 
	ifeq ($(target), editor)
		EXE = editor.exe
	else
		EXE = player.exe
	endif
endif

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : libs/imgui/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)
	$(CC) -o $(EXE) $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf ./*.o
	rm -rf ./src/*.o
	rm -rf ./bin/*.exe
