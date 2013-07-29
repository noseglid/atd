#!/bin/sh

set -e

RUNDIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
BUILDDIR=$RUNDIR/deps/build/
INSTALLDIR=$RUNDIR/deps/target/

OGGVER=1.3.1
VORBISVER=1.3.3
SDLVER=1.2.15
FREETYPEVER=2.5.0.1
SDLTTFVER=2.0.11
SDLMIXERVER=1.2.12
SDLIMGVER=1.2.12
PJSONVER=0.2.1
ASSIMPVER=3.0.1270
DEVER=0.1.0
DYLIBBUNDLERVER=0.4.3

build()
{
  ./configure --prefix=$INSTALLDIR $*
  make -j8
  make install
}

info()
{
  echo $1
}

fetch()
{
  FLAGS="-O"
  [ -z $2 ] || FLAGS="-o $2"

  info "Retrieving '$1'"
  curl -L $1 $FLAGS
}

build_ogg()
{
  NAME="libogg-$OGGVER.tar.xz"
  fetch "http://downloads.xiph.org/releases/ogg/$NAME"

  tar -xJf $NAME
  cd libogg-*
  build
  cd -
}

build_vorbis()
{
  NAME="libvorbis-$VORBISVER.tar.xz"
  fetch "http://downloads.xiph.org/releases/vorbis/$NAME"

  tar -xJf $NAME
  cd libvorbis-*
  build
  cd -
}

build_sdl()
{
  NAME="SDL-$SDLVER.tar.gz"
  fetch "http://www.libsdl.org/release/$NAME"

  tar -xzf $NAME
  cd SDL-*
  build
  cd -
}

build_freetype()
{
  NAME="freetype-$FREETYPEVER.tar.gz"
  fetch "http://download.savannah.gnu.org/releases/freetype/$NAME"

  tar -xzf $NAME
  cd freetype-*
  build
  cd -
}

build_sdl_ttf()
{
  NAME="SDL_ttf-$SDLTTFVER.tar.gz"
  fetch "http://www.libsdl.org/projects/SDL_ttf/release/$NAME"

  tar -xzf $NAME
  cd SDL_ttf-*
  build
  cd -
}

build_sdl_mixer()
{
  NAME="SDL_mixer-$SDLMIXERVER.tar.gz"
  fetch "http://www.libsdl.org/projects/SDL_mixer/release/$NAME"

  tar -xzf $NAME
  cd SDL_mixer-*
  build \
    --enable-music-mod=no \
    --enable-music-ogg=yes \
    --enable-music-ogg-shared=no \
    --enable-music-fluidsynth-midi=no \
    --enable-music-flac=no \
    --disable-smpegtest
  cd -
}

build_sdl_image()
{
  NAME="SDL_image-$SDLIMGVER.tar.gz"
  fetch "http://www.libsdl.org/projects/SDL_image/release/$NAME"

  tar -xzf $NAME
  cd SDL_image-*
  build \
    --enable-jpg=yes \
    --enable-jpg-shared=yes \
    --disable-bmp \
    --disable-gif \
    --disable-lbm \
    --disable-pcx \
    --disable-png \
    --disable-png-shared \
    --disable-pnm \
    --disable-tga \
    --disable-tif \
    --disable-tif-shared \
    --disable-xcf \
    --disable-xpm \
    --disable-xv \
    --disable-webp \
    --disable-webp-shared

  cd -
}

build_pjson()
{
  NAME="pjson-v$PJSONVER.tar.gz"
  fetch "https://github.com/noseglid/pjson/releases/v0.2.1/2158/$NAME"

  tar -xzf $NAME
  cd pjson-*
  build
  cd -
}

build_assimp()
{
  NAME="assimp--$ASSIMPVER-source-only.zip"
  fetch "http://sourceforge.net/projects/assimp/files/assimp-3.0/$NAME/download" $NAME

  unzip -q -o $NAME
  ORIGDIR=$(pwd)
  cd assimp-*
  patch -N -p1 < $RUNDIR/deps/assimp.osx-build.patch

  mkdir -p build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX:PATH=$INSTALLDIR ..
  make -j8
  make install

  cd $ORIGDIR
}

build_de()
{
  NAME="libde-v$DEVER.tar.gz"
  fetch "https://github.com/noseglid/libde/releases/v0.1.0/2153/$NAME"

  tar -xzf $NAME
  cd libde-*
  build
  cd -
}

build_dylibbundler()
{
  NAME="dylibbundler-$DYLIBBUNDLERVER.zip"
  fetch "http://sourceforge.net/projects/macdylibbundler/files/macdylibbundler/$DYLIBBUNDLERVER/$NAME/download" $NAME

  unzip -q -o $NAME
  cd dylibbundler*
  make
  # Install is not very good in dylibbundler, install manually
  install -d $INSTALLDIR/bin/
  install dylibbundler $INSTALLDIR/bin/
  cd -
}

export CXX=clang++
export CXXFLAGS=-stdlib=libc++
export PKG_CONFIG_PATH=${INSTALLDIR}lib/pkgconfig
export CPPFLAGS=-I${INSTALLDIR}include
export LDFLAGS=-L${INSTALLDIR}lib
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/local/bin:/usr/local/bin:/opt/X11/bin

PACKAGES="ogg vorbis pjson assimp de freetype sdl sdl_ttf sdl_mixer sdl_image dylibbundler"

case "$1" in
  build)
    info "Creating directory"
    mkdir -p $BUILDDIR
    cd $BUILDDIR

    if [ ! -z $2 ]; then
      info "Building single dependency: $2"
      build_$2
      exit
    fi

    for p in $PACKAGES
    do
      build_$p
    done
    ;;

  clean)
    DIRS="$INSTALLDIR $BUILDDIR"
    for d in $DIRS
    do
      info "Removing: $d"
      rm -rf $d
    done
    ;;

  *)
    info "Available packages: $PACKAGES\n"
    info "Usage:"
    info "$0 clean"
    info "$0 build [package]"
    ;;
esac
