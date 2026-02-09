CXX      = g++
CXXFLAGS = -std=c++11 $(shell pkg-config --cflags sdl2 sdl2_image sdl2_ttf sdl2_mixer)
LDFLAGS  = $(shell pkg-config --libs sdl2 sdl2_image sdl2_ttf sdl2_mixer)

SRCS     = $(wildcard src/*.cc)
OBJS     = $(SRCS:.cc=.o)
TARGET   = app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
