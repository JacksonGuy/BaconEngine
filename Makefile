CC = g++

CFLAGS = -I. -L.
CFLAGS += -Iinclude/SFML/include/
CFLAGS += -Linclude/SFML/lib
CFLAGS += -DSFML_STATIC
CFLAGS += -Iinclude/imgui -Iinclude/imgui/backends
CFLAGS += -Iinclude/lua -Linclude/lua
CFLAGS += -Iinclude/nativefiledialog/src/include
CFLAGS += -Linclude/nativefiledialog/build/lib/Release/x64

# Linux 
ifeq ($(UNAME), Linux)
	CFLAGS += -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/harfbuzz 
	CFLAGS += -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1
	CFLAGS += -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/x86_64-linux-gnu -I/usr/include/webp -I/usr/include/gio-unix-2.0 -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0
	CFLAGS += -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include
endif

BUILD_DIR = bin

ifeq ($(target), editor_release)
	CFLAGS += -O3
	SOURCES += Program.cpp
else ifeq ($(target), editor_debug)
	CFLAGS += -ggdb -Wall -Wextra -pedantic-errors
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
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

UNAME = $(shell uname -s)
LIBS = 
EXE = 

ifeq ($(UNAME), Linux)
	LIBS += -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
	LIBS += -llua -lGL -lglfw -lnfd
	LIBS += -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg
	LIBS += -pthread -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
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
	rm -rf ./bin/*.o
	rm -rf ./*.exe

cleanall:
	rm -rf ./bin
	rm -rf ./*.exe