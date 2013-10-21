#!/bin/sh

set -e

RUNDIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
BUILDDIR=$RUNDIR/deps/build/
INSTALLDIR=$RUNDIR/deps/target/

BZIP2VER=1.0.6
JPEGVER=v9
SQLITE3VER=3080000
OGGVER=1.3.1
VORBISVER=1.3.3
SDLVER=1.2.15
FREETYPEVER=2.5.0.1
SDLTTFVER=2.0.11
SDLMIXERVER=1.2.12
SDLIMGVER=1.2.12
PJSONVER=0.2.1
ASSIMPVER=3.0.1270
DEVER=staging
DYLIBBUNDLERVER=0.4.3
ROCKETVER=c95b8f4a1895340e1b847cba2d29a529601bb151
BOOSTVER=1.54.0
SOCIVER=3.2.2

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

build_bzip2()
{
  NAME="bzip2-$BZIP2VER.tar.gz"
  fetch "http://www.bzip.org/$BZIP2VER/$NAME"

  tar -xzf $NAME
  cd bzip2-*
  make
  make install PREFIX=$INSTALLDIR
  cd -
}

build_jpeg()
{
  NAME="jpegsrc.$JPEGVER.tar.gz"
  fetch "http://www.ijg.org/files/$NAME"

  tar -xzf $NAME
  cd jpeg-*
  build
  cd -
}

build_sqlite3()
{
  NAME="sqlite-autoconf-$SQLITE3VER.tar.gz"
  fetch "http://www.sqlite.org/2013/$NAME"

  tar -xzf $NAME
  cd sqlite-*
  build
  cd -
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
    --enable-png=yes \
    --enable-png-shared=yes \
    --disable-bmp \
    --disable-gif \
    --disable-lbm \
    --disable-pcx \
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
  NAME="libde-$DEVER.tar.gz"
  fetch "https://github.com/noseglid/libde/archive/staging.tar.gz" $NAME

  tar -xzf $NAME
  cd libde-*
  ./autogen.sh
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

build_rocket()
{
  NAME="$ROCKETVER.zip"
  fetch "https://github.com/lloydw/libRocket/archive/$NAME"

  unzip -q -o $NAME
  ORIGDIR=$(pwd)
  cd libRocket-*/Build
  patch -N -p1 < $RUNDIR/deps/librocket.osx-build.patch
  mkdir build
  cd build
  cmake \
    -DCMAKE_INSTALL_PREFIX:PATH=$INSTALLDIR \
    -DCMAKE_OSX_ARCHITECTURES:STRING="x86_64;" \
    -DBUILD_PYTHON_BINDINGS=OFF \
    -DBUILD_SAMPLES=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    ..

  make -j8
  make install

  cd $ORIGDIR
}

build_boost()
{
  VUS="$(echo $BOOSTVER | tr '.' '_')"
  fetch "http://downloads.sourceforge.net/project/boost/boost/$BOOSTVER/boost_$VUS.tar.bz2?use_mirror=optimate" "boost_$VUS.tar.bz2"

  tar -xjf "boost_$VUS.tar.bz2"
  cd boost_$VUS
  ./bootstrap.sh --prefix=$INSTALLDIR
  ./b2 -j8 install
  cd -
}

build_soci()
{
  fetch "https://github.com/SOCI/soci/archive/$SOCIVER.tar.gz" "soci-$SOCIVER.tar.gz"

  tar -xzf soci-$SOCIVER.tar.gz
  cd soci-$SOCIVER/build
  cmake -G "Unix Makefiles" \
    -DCMAKE_INSTALL_PREFIX:PATH=$INSTALLDIR \
    -DSOCI_TESTS=OFF \
    -DWITH_BOOST=OFF \
    -DWITH_DB2=OFF \
    -DWITH_FIREBIRD=OFF \
    -DWITH_MYSQL=OFF \
    -DWITH_ODBC=OFF \
    -DWITH_ORACLE=OFF \
    -DWITH_POSTGRESQL=OFF \
    -DWITH_SQLITE3=ON \
    -DSQLITE3_INCLUDE_DIR=$INSTALLDIR/include \
    -DSQLITE3_LIBRARIES=$INSTALLDIR/lib/libsqlite3.dylib \
    ../src
  make -j8
  make install
  cd -
}

export CFLAGS="-arch x86_64"
export CXX="clang++"
export CXXFLAGS="-stdlib=libc++ -arch x86_64"
export PKG_CONFIG_PATH=${INSTALLDIR}lib/pkgconfig
export CPPFLAGS="-I${INSTALLDIR}include"
export LDFLAGS="-L${INSTALLDIR}lib"
export PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/local/bin:/usr/local/bin:/opt/X11/bin

PACKAGES="bzip2 sqlite3 ogg vorbis pjson assimp de freetype sdl sdl_ttf sdl_mixer sdl_image dylibbundler rocket soci"

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
