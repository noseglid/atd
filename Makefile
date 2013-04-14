CC      = clang++
CFLAGS  = -O0 -g3 -c -Wall -pedantic $(shell sdl-config --cflags)
CFLAGS  += -DDEBUG -std=c++11 -stdlib=libc++
CFLAGS  += -I/Users/noseglid/opt/include/

LDFLAGS  = $(shell /Users/noseglid/opt/bin/sdl-config --libs)
LDFLAGS += -L/Users/noseglid/opt/lib/
LDFLAGS += -lassimp -ljpeg -ltga -lSDL_ttf -lpjson -framework OpenGL -stdlib=libc++

SRCS  = main.cpp Model.cpp ImageLoader.cpp jpeg.cpp tga.cpp Camera.cpp Text.cpp
SRCS += Player.cpp Game.cpp Map.cpp Projectile.cpp
SRCS += Path.cpp TowerManager.cpp Tower.cpp
SRCS += CreepManager.cpp Creep.cpp SphereCreep.cpp DummyCreep.cpp
SRCS += KeyboardHandler.cpp MetaManager.cpp HUD.cpp GLTransform.cpp

# Math related sources
SRCS += math/Math.cpp math/Vector2.cpp math/Vector3.cpp math/Matrix4.cpp

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

$(BIN): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(BIN)

-include $(DEPS)
