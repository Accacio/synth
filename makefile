##
# Synth
#
# @file
# @version 0.1
#
SOURCES = main.cpp
# SOURCES += midi.cc

EXE = synth
SOURCES += mixer.cpp instrument.cpp aux.cpp
SOURCES += imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl2.cpp
SOURCES += imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

INCLUDES = -Iimgui/ -Iimgui/examples

LIBS =-lGL -ldl `sdl2-config --libs`
INCLUDES += `sdl2-config --cflags`

%.o:%.cpp
	@echo Compiling $@
	@$(CXX) $(INCLUDES) -c -o $@ $<

%.o:imgui/%.cpp
	@echo Compiling $@
	@$(CXX) $(INCLUDES) -c -o $@ $<

%.o:imgui/examples/%.cpp
	@echo Compiling $@
	@$(CXX) $(INCLUDES) -c -o $@ $<

all: $(EXE)
	@echo Build complete

$(EXE): $(OBJS)
	@echo Linking
	@$(CXX) -o $@ $^ $(LIBS) $(INCLUDES)

clean:
	rm -f $(EXE) $(OBJS)

midi:
	g++ midi.cc -o midi

midiRun:
	./midi
midiClean:
	rm midi

run:
	@./$(EXE)
