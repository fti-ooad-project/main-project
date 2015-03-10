# Desktop
_DESKTOP_LIBS = -g -lSDL2 -lGLEW -lGL
_DESKTOP_CFLAGS = -g
_DESKTOP_CXXFLAGS = -g
_DESKTOP_SOURCES = 

# Android
_ANDROID_LIBS = -g -llog -landroid  -lEGL -lGLESv2
_ANDROID_CFLAGS = -g
_ANDROID_CXXFLAGS = -g
_ANDROID_SOURCES = 

# Common
_CFLAGS = -Wall
_CXXFLAGS = -Wall -std=c++11
_SOURCES = source/graphics/graphics.c source/graphics/shader.c source/main.cpp source/media/android.c source/media/desktop.c source/unit.cpp source/object.cpp source/division.cpp source/processor.cpp
_HEADERS = source/4u/complex/complex.hpp source/4u/complex/quaternion.hpp source/4u/la/mat.hpp source/4u/la/vec.hpp source/4u/random/contrand.hpp source/4u/random/diskrand.hpp source/4u/random/rand.hpp source/4u/random/sphericrand.hpp source/4u/util/const.hpp source/4u/util/op.hpp source/graphics/graphics.h source/graphics/shader.h source/graphics/shader_source.h source/media/media.h source/unit.hpp source/view.hpp source/object.hpp source/division.hpp source/processor.hpp
