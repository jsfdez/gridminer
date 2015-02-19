TEMPLATE = lib

include(../config.pri)
CONFIG += staticlib

CONFIG(debug, debug|release) {
    DESTDIR = ../debug
    OBJECTS_DIR = libsdl2_debug_obj
} else {
    DESTDIR = ../release
    OBJECTS_DIR = libsdl2_release_obj
}

DEFINES += _SCL_SECURE_NO_WARNINGS
DEFINES += _WINDOWS
DEFINES += BUILD_WINDOWS
DEFINES += _WIN32

LIBS += winmm.lib

INCLUDEPATH += libsdl2/include
INCLUDEPATH += libsdl2/include/SDL

HEADERS += $$files(libsdl2/include/*.h)

libsdl_precompiled {
    OTHER_FILES += $$files(libsdl2/lib/*.*, true)
} else {
    SOURCES += libsdl2/src/dynapi/SDL_dynapi.c
    SOURCES += libsdl2/src/core/windows/SDL_windows.c

    SOURCES += libsdl2/src/events/SDL_quit.c
    SOURCES += libsdl2/src/events/SDL_touch.c
    SOURCES += libsdl2/src/events/SDL_mouse.c
    SOURCES += libsdl2/src/events/SDL_events.c
    SOURCES += libsdl2/src/events/SDL_gesture.c
    SOURCES += libsdl2/src/events/SDL_keyboard.c
    SOURCES += libsdl2/src/events/SDL_dropevents.c
    SOURCES += libsdl2/src/events/SDL_windowevents.c
    SOURCES += libsdl2/src/events/SDL_clipboardevents.c


    SOURCES += libsdl2/src/libm/e_log.c
    SOURCES += libsdl2/src/libm/e_pow.c
    SOURCES += libsdl2/src/libm/k_cos.c
    SOURCES += libsdl2/src/libm/k_sin.c
    SOURCES += libsdl2/src/libm/s_cos.c
    SOURCES += libsdl2/src/libm/s_sin.c
    SOURCES += libsdl2/src/libm/e_sqrt.c
    SOURCES += libsdl2/src/libm/s_atan.c
    SOURCES += libsdl2/src/libm/s_fabs.c
    SOURCES += libsdl2/src/libm/e_atan2.c
    SOURCES += libsdl2/src/libm/s_floor.c
    SOURCES += libsdl2/src/libm/s_scalbn.c
    SOURCES += libsdl2/src/libm/e_rem_pio2.c
    SOURCES += libsdl2/src/libm/k_rem_pio2.c
    SOURCES += libsdl2/src/libm/s_copysign.c

    SOURCES += libsdl2/src/render/direct3d/SDL_render_d3d.c
    SOURCES += libsdl2/src/render/direct3d11/SDL_render_d3d11.c

    SOURCES += libsdl2/src/render/opengl/SDL_render_gl.c
    SOURCES += libsdl2/src/render/opengl/SDL_shaders_gl.c
    SOURCES += libsdl2/src/render/opengles2/SDL_render_gles2.c
    SOURCES += libsdl2/src/render/opengles2/SDL_shaders_gles2.c

    SOURCES += libsdl2/src/render/SDL_render.c
    SOURCES += libsdl2/src/render/SDL_yuv_sw.c
    SOURCES += libsdl2/src/render/SDL_yuv_mmx.c
    SOURCES += libsdl2/src/render/SDL_d3dmath.c

    SOURCES += libsdl2/src/render/software/SDL_rotate.c
    SOURCES += libsdl2/src/render/software/SDL_drawline.c
    SOURCES += libsdl2/src/render/software/SDL_drawpoint.c
    SOURCES += libsdl2/src/render/software/SDL_render_sw.c
    SOURCES += libsdl2/src/render/software/SDL_blendline.c
    SOURCES += libsdl2/src/render/software/SDL_blendpoint.c
    SOURCES += libsdl2/src/render/software/SDL_blendfillrect.c

    SOURCES += libsdl2/src/SDL.c
    SOURCES += libsdl2/src/SDL_log.c
    SOURCES += libsdl2/src/SDL_hints.c
    SOURCES += libsdl2/src/SDL_error.c
    SOURCES += libsdl2/src/SDL_assert.c

    SOURCES += libsdl2/src/atomic/SDL_atomic.c
    SOURCES += libsdl2/src/atomic/SDL_spinlock.c

    SOURCES += libsdl2/src/audio/SDL_wave.c
    SOURCES += libsdl2/src/audio/SDL_mixer.c
    SOURCES += libsdl2/src/audio/SDL_audio.c
    SOURCES += libsdl2/src/audio/SDL_audiocvt.c
    SOURCES += libsdl2/src/audio/SDL_audiodev.c
    SOURCES += libsdl2/src/audio/winmm/SDL_winmm.c
    SOURCES += libsdl2/src/audio/SDL_audiotypecvt.c
    SOURCES += libsdl2/src/audio/disk/SDL_diskaudio.c
    SOURCES += libsdl2/src/audio/xaudio2/SDL_xaudio2.c
    SOURCES += libsdl2/src/audio/dummy/SDL_dummyaudio.c
    SOURCES += libsdl2/src/audio/directsound/SDL_directsound.c

    SOURCES += libsdl2/src/video/SDL_bmp.c
    SOURCES += libsdl2/src/video/SDL_egl.c
    SOURCES += libsdl2/src/video/SDL_blit.c
    SOURCES += libsdl2/src/video/SDL_rect.c
    SOURCES += libsdl2/src/video/SDL_shape.c
    SOURCES += libsdl2/src/video/SDL_video.c
    SOURCES += libsdl2/src/video/SDL_blit_0.c
    SOURCES += libsdl2/src/video/SDL_blit_1.c
    SOURCES += libsdl2/src/video/SDL_blit_A.c
    SOURCES += libsdl2/src/video/SDL_blit_N.c
    SOURCES += libsdl2/src/video/SDL_pixels.c
    SOURCES += libsdl2/src/video/SDL_surface.c
    SOURCES += libsdl2/src/video/SDL_stretch.c
    SOURCES += libsdl2/src/video/SDL_fillrect.c
    SOURCES += libsdl2/src/video/SDL_RLEaccel.c
    SOURCES += libsdl2/src/video/SDL_blit_auto.c
    SOURCES += libsdl2/src/video/SDL_blit_copy.c
    SOURCES += libsdl2/src/video/SDL_blit_slow.c
    SOURCES += libsdl2/src/video/SDL_clipboard.c
    SOURCES += libsdl2/src/video/dummy/SDL_nullvideo.c
    SOURCES += libsdl2/src/video/dummy/SDL_nullevents.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsmodes.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsmouse.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsshape.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsvideo.c
    SOURCES += libsdl2/src/video/windows/SDL_windowswindow.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsevents.c
    SOURCES += libsdl2/src/video/dummy/SDL_nullframebuffer.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsopengl.c
    SOURCES += libsdl2/src/video/windows/SDL_windowskeyboard.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsopengles.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsclipboard.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsmessagebox.c
    SOURCES += libsdl2/src/video/windows/SDL_windowsframebuffer.c

    SOURCES += libsdl2/src/cpuinfo/SDL_cpuinfo.c

    SOURCES += libsdl2/src/stdlib/SDL_iconv.c
    SOURCES += libsdl2/src/stdlib/SDL_qsort.c
    SOURCES += libsdl2/src/stdlib/SDL_stdlib.c
    SOURCES += libsdl2/src/stdlib/SDL_string.c
    SOURCES += libsdl2/src/stdlib/SDL_getenv.c
    SOURCES += libsdl2/src/stdlib/SDL_malloc.c

    SOURCES += libsdl2/src/haptic/SDL_haptic.c
    SOURCES += libsdl2/src/haptic/windows/SDL_syshaptic.c

    SOURCES += libsdl2/src/joystick/SDL_joystick.c
    SOURCES += libsdl2/src/joystick/SDL_gamecontroller.c
    SOURCES += libsdl2/src/joystick/windows/SDL_dxjoystick.c
    SOURCES += libsdl2/src/joystick/windows/SDL_mmjoystick.c

    SOURCES += libsdl2/src/power/SDL_power.c
    SOURCES += libsdl2/src/power/windows/SDL_syspower.c

    SOURCES += libsdl2/src/file/SDL_rwops.c

    SOURCES += libsdl2/src/thread/SDL_thread.c
    SOURCES += libsdl2/src/thread/windows/SDL_syssem.c
    SOURCES += libsdl2/src/thread/windows/SDL_systls.c
    SOURCES += libsdl2/src/thread/generic/SDL_syscond.c
    SOURCES += libsdl2/src/thread/windows/SDL_sysmutex.c
    SOURCES += libsdl2/src/thread/windows/SDL_systhread.c

    SOURCES += libsdl2/src/filesystem/windows/SDL_sysfilesystem.c

    SOURCES += libsdl2/src/loadso/windows/SDL_sysloadso.c

    SOURCES += libsdl2/src/timer/SDL_timer.c
    SOURCES += libsdl2/src/timer/windows/SDL_systimer.c

    SOURCES += libsdl2/src/main/windows/SDL_windows_main.c
}
