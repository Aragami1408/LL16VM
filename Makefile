
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL2
#

CC = gcc
CXX = g++
#CXX = clang++

EMULATOR = ll16vm_emulator
ASSEMBLER = ll16vm_assembler

INC_DIR = src
ASM_DIR = src/assembler
IMGUI_DIR = externals/imgui/

SOURCES = $(INC_DIR)/main.cpp $(INC_DIR)/cpu.c $(INC_DIR)/memory_mapper.c $(INC_DIR)/ram_device.c $(INC_DIR)/screen_device.c
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

ASM_SOURCES = $(ASM_DIR)/assembler.c $(ASM_DIR)/common_parser.c $(ASM_DIR)/expr_evaluator.c $(ASM_DIR)/expr_parser.c $(ASM_DIR)/instruction_evaluator.c $(ASM_DIR)/instruction_parser.c $(ASM_DIR)/label_evaluator.c $(ASM_DIR)/mpc.c $(ASM_DIR)/utils.c

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
ASM_OBJS = $(addsuffix .o, $(basename $(notdir $(ASM_SOURCES))))
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CFLAGS = -ggdb -Wall -Wformat -Wcast-align -Wconversion

CXXFLAGS = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(INC_DIR)
CXXFLAGS += -ggdb -Wall -Wformat -Wcast-align -Wconversion -std=c++11
LIBS =

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2
## If you're on a Raspberry Pi and want to use the legacy drivers,
## use the following instead:
# LINUX_GL_LIBS = -L/opt/vc/lib -lbrcmGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) -ldl `sdl2-config --libs`

	CXXFLAGS += `sdl2-config --cflags`
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib

	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
endif

ifeq ($(OS), Windows_NT)
    ECHO_MESSAGE = "MinGW"
    LIBS += -lgdi32 -lopengl32 -limm32 `pkg-config --static --libs sdl2`

	CXXFLAGS += -mconsole
    CXXFLAGS += `pkg-config --cflags sdl2`
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:$(ASM_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:$(INC_DIR)/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(INC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EMULATOR) $(ASSEMBLER)
	@echo Build complete for $(ECHO_MESSAGE)

$(EMULATOR): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(ASSEMBLER): $(ASM_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	

clean:
	rm -f $(EMULATOR) $(ASSEMBLER) $(OBJS)
