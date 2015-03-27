# Desktop
_DESKTOP_LIBS = -g -lSDL2 -lGLEW -lGL -lfreetype
_DESKTOP_CFLAGS = -g $(shell freetype-config --cflags)
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
_SOURCES = source/main.cpp source/engine-source/division.cpp source/engine-source/object.cpp source/engine-source/processor.cpp source/engine-source/unit.cpp source/font/font.c source/gui/impl/button.cpp source/gui/impl/container.cpp source/gui/impl/object.cpp source/gui/impl/screen.cpp source/gui/eventtranslator.cpp source/gui/factory.cpp source/graphics/graphics.c source/graphics/shader.c source/media/android/android.c source/media/android/assets.c source/media/android/graphics.c source/media/common/common.c source/media/desktop/assets.c source/media/desktop/desktop.c source/media/desktop/graphics.c
_HEADERS = source/4u/complex/complex.hpp source/4u/complex/quaternion.hpp source/4u/la/mat.hpp source/4u/la/vec.hpp source/4u/random/contrand.hpp source/4u/random/diskrand.hpp source/4u/random/rand.hpp source/4u/random/sphericrand.hpp source/4u/util/const.hpp source/4u/util/op.hpp source/engine/engine.hpp source/view.hpp source/engine-source/division.hpp source/engine-source/object.hpp source/engine-source/processor.hpp source/engine-source/tree.hpp source/engine-source/unit.hpp source/font/font.h source/gui/impl/button.hpp source/gui/impl/container.hpp source/gui/impl/object.hpp source/gui/impl/screen.hpp source/gui/action.hpp source/gui/button.hpp source/gui/container.hpp source/gui/eventtranslator.hpp source/gui/factory.hpp source/gui/label.hpp source/gui/object.hpp source/gui/screen.hpp source/graphics/graphics.h source/graphics/shader.h source/graphics/shader_source.h source/media/android/android.h source/media/android/platform.h source/media/common/common.h source/media/desktop/desktop.h source/media/desktop/platform.h source/media/assets.h source/media/event.h source/media/input.h source/media/log.h source/media/media.h
