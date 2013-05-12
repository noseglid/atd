CC      = clang++
CFLAGS  = -O0 -g3 -c -Wall -pedantic $(shell sdl-config --cflags)
CFLAGS += -DDEBUG -std=c++11 -stdlib=libc++
CFLAGS += -I/Users/noseglid/opt/include/
CFLAGS += -DSFXVOL=32 -DMUSICVOL=16

LDFLAGS  = $(shell /Users/noseglid/opt/bin/sdl-config --libs)
LDFLAGS += -L/Users/noseglid/opt/lib/
LDFLAGS += -lassimp -ljpeg -ltga -lSDL_ttf -lSDL_mixer -lpjson -framework OpenGL -stdlib=libc++

SRCS  = main.cpp
SRCS += Player.cpp Game.cpp Map.cpp Path.cpp
SRCS += KeyboardHandler.cpp MetaManager.cpp GLTransform.cpp

# Core
SRCS += Mobile.cpp Model.cpp Camera.cpp Text.cpp HUD.cpp Audio.cpp

# Creep
SRCS += CreepManager.cpp Creep.cpp SphereCreep.cpp DummyCreep.cpp

# Tower
SRCS += TowerManager.cpp Tower.cpp
SRCS += Projectile.cpp

# Math
SRCS += math/Math.cpp math/Vector2.cpp math/Vector3.cpp math/Matrix4.cpp

# Image
SRCS += ImageLoader.cpp jpeg.cpp tga.cpp

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
