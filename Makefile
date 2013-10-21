SUBDIRS = resources/levels/

CXX     = clang++
CFLAGS  = -O0 -g3 -c -Wall -pedantic $(shell deps/target/bin/sdl-config --cflags)
CFLAGS += -DDEBUG=1 -std=c++11 -stdlib=libc++
CFLAGS += -Ideps/target/include -Ideps/target/include/soci -I.

LDFLAGS  = $(shell deps/target/bin/sdl-config --libs)
LDFLAGS += -Ldeps/target/lib -stdlib=libc++ -headerpad_max_install_names
LDFLAGS += -Wl,-framework,OpenGL -lfreetype -lpjson -lassimp -lsqlite3
LDFLAGS += -lsoci_core -lsoci_sqlite3
LDFLAGS += -lSDL_ttf -lSDL_mixer -lSDL_image
LDFLAGS += -lRocketControls -lRocketDebugger -lRocketCore

DB = $(shell pwd)/resources/offline.db

SRCS  = main.cpp
SRCS += Player.cpp User.cpp Game.cpp Map.cpp Path.cpp
SRCS += KeyboardHandler.cpp MetaManager.cpp GLTransform.cpp GLShapes.cpp

# Engine
SRCS += engine/Engine.cpp engine/Video.cpp

# Data access layer
SRCS += dal/Dal.cpp dal/Offline.cpp

# Core
SRCS += Mobile.cpp Model.cpp Camera.cpp Text.cpp Audio.cpp

# UI
SRCS += ui/UI.cpp ui/System.cpp ui/Renderer.cpp ui/LevelDatabase.cpp
SRCS += ui/Menu.cpp ui/TitleMenu.cpp ui/LevelSelectMenu.cpp ui/OptionsMenu.cpp
SRCS += ui/PauseMenu.cpp ui/SetUserMenu.cpp

# Hud
SRCS += hud/HUD.cpp hud/Bar.cpp hud/ButtonBar.cpp hud/InfoBar.cpp hud/InfoBox.cpp
SRCS += hud/Button.cpp

# Creep
SRCS += CreepManager.cpp Creep.cpp DummyCreep.cpp

# Tower
SRCS += TowerManager.cpp Tower.cpp
SRCS += Projectile.cpp

# Math
SRCS += math/Math.cpp math/Vector2.cpp math/Vector3.cpp math/Matrix4.cpp

# Utils
SRCS += utils/Color.cpp ImageLoader.cpp

OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean $(SUBDIRS)

BIN = atd

all: $(DB) $(BIN) $(SUBDIRS)

$(SUBDIRS): $(DB)
	$(MAKE) DB=$(DB) -C $@

# The actual build target.
# Dependency files are generated for each object file built.
# Also adds empty targets for real files so that a removal
# of any such file wont exit with failure (but rather rebuild the target)
%.o: %.cpp Makefile
	$(CXX) $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(BIN): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@
	install_name_tool -change libassimp.3.dylib $(shell pwd)/deps/target/lib/libassimp.3.dylib $(BIN)

$(DB): resources/create_db.sh
	   ./resources/create_db.sh $(DB)

release_osx: $(BIN)
	mkdir -p $(BIN).app
	mkdir -p $(BIN).app/Contents
	mkdir -p $(BIN).app/Contents/MacOS
	mkdir -p $(BIN).app/Contents/libs
	mkdir -p $(BIN).app/Contents/Resources
	cp osx/icon.icns $(BIN).app/Contents/Resources/
	cp osx/Info.plist $(BIN).app/Contents
	cp osx/launch.sh $(BIN).app/Contents/MacOS
	cp $(BIN) $(BIN).app/Contents/MacOS
	DYLD_LIBRARY_PATH=deps/target/lib ./deps/target/bin/dylibbundler -od -b -x $(BIN).app/Contents/MacOS/atd -d $(BIN).app/Contents/libs/
	cp -r resources $(BIN).app/Contents/Resources/

release_clean_osx:
	rm -rf $(BIN).app

clean: release_clean_osx
	rm -f $(OBJS) $(DEPS) $(BIN) $(DB)

-include $(DEPS)
