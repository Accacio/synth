##
# Synth
#
# @file
# @version 0.1
#
CXX=clang++
SOURCES = main.cpp
# SOURCES += midi.cc

EXE = build/synth
SOURCES += src/mixer.cpp src/instrument.cpp src/aux.cpp src/instrumentCollec.cpp
SOURCES += imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl2.cpp
SOURCES += imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
OBJS = $(addprefix build/,$(addsuffix .o, $(basename $(notdir $(SOURCES)))))

INCLUDES = -Iimgui/ -Iimgui/examples -Ilib/

LIBS =-lGL -ldl `sdl2-config --libs`
INCLUDES += `sdl2-config --cflags`
build/%.o:src/%.cpp
	@echo Compiling $@
	@$(CXX) -MJ $@.json $(INCLUDES) -c -o $@ $<

build/%.o:imgui/%.cpp
	@echo Compiling $@
	@$(CXX) -MJ $@.json $(INCLUDES) -c -o $@ $<

build/%.o:imgui/examples/%.cpp
	@echo Compiling $@
	@$(CXX) -MJ $@.json $(INCLUDES) -c -o $@ $<

all: $(EXE)
	@echo Build complete

$(EXE): $(OBJS)
	@echo Linking
	@sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' build/*.o.json > compile_commands.json
	@$(CXX) -o $@ $^ $(LIBS) $(INCLUDES)

clean:
	rm -f $(EXE) $(OBJS)

midi:
	g++ src/midi.cc -o build/midi

midiClean:
	rm midi

run:
	@./$(EXE)
