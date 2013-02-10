CC      = clang++
CFLAGS  = -O0 -g3 -c -Wall -pedantic $(shell sdl-config --cflags) -std=c++11
CFLAGS  += -I/opt/local/include -DDEBUG

LDFLAGS  = -L/opt/local/lib -L/usr/local/lib $(shell sdl-config --libs)
LDFLAGS += -framework OpenGL -lassimp -ljpeg -ltga -lSDL_ttf -lpjson

SRCS  = main.cpp Model.cpp ImageLoader.cpp jpeg.cpp tga.cpp Camera.cpp Text.cpp
SRCS += Map.cpp Path.cpp
SRCS += Game.cpp
SRCS += KeyboardHandler.cpp MetaManager.cpp
SRCS += CreepManager.cpp Creep.cpp BoxCreep.cpp
SRCS += Math.cpp Vector2.cpp Vector3.cpp Matrix4.cpp

OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

BIN = atd

all: $(BIN)

# The actual build target.
# Dependency files are generated for each object file built.
# Also adds empty targets for real files so that a removal
# of any such file wont exit with failure (but rather rebuild the target)
%.o: %.cpp Makefile
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(BIN): $(OBJS) $(CWC_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(BIN)

-include $(DEPS)
