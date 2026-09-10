LOCAL_PATH := $(call my-dir)
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := ikuradroid

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(APP_SUBDIR) \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../sdl/include \
	$(LOCAL_PATH)/../sdl_ttf/include \
	$(LOCAL_PATH)/../sdl_image \
	$(LOCAL_PATH)/../sdl_gfx \
	$(LOCAL_PATH)/engine \
	$(LOCAL_PATH)/dialogs \
	$(LOCAL_PATH)/widgets \
	$(LOCAL_PATH)/common \
	$(LOCAL_PATH)/tlove
		
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/*.cpp)\
	$(wildcard $(LOCAL_PATH)/common/*.cpp)\
    $(wildcard $(LOCAL_PATH)/crowd/*.cpp) \
    $(wildcard $(LOCAL_PATH)/crowd/tokimeki/*.cpp) \
    $(wildcard $(LOCAL_PATH)/crowd/xchange1/*.cpp) \
    $(wildcard $(LOCAL_PATH)/crowd/xchange3/*.cpp) \
    $(wildcard $(LOCAL_PATH)/cware/*.cpp) \
    $(wildcard $(LOCAL_PATH)/cware/dividead/*.cpp) \
	$(wildcard $(LOCAL_PATH)/dialogs/*.cpp)\
    $(wildcard $(LOCAL_PATH)/engine/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/catgirl/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/Heartdr/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/virgin/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/crescendo/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/hitomi/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/idols/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/kana/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/kanaoka/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/sagara/*.cpp) \
    $(wildcard $(LOCAL_PATH)/ikura/snow/*.cpp) \
	$(wildcard $(LOCAL_PATH)/jast/*.cpp)\
	$(wildcard $(LOCAL_PATH)/media/*.cpp)\
	$(wildcard $(LOCAL_PATH)/res/*.cpp)\
	$(wildcard $(LOCAL_PATH)/res/archives/*.cpp)\
	$(wildcard $(LOCAL_PATH)/res/converters/*.cpp)\
	$(wildcard $(LOCAL_PATH)/testing/*.cpp)\
	$(wildcard $(LOCAL_PATH)/tlove/*.cpp)\
	$(wildcard $(LOCAL_PATH)/widgets/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/critical/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/lmm/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/yume/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/princess/*.cpp)\
	$(wildcard $(LOCAL_PATH)/will/starry/*.cpp)\
	$(wildcard $(LOCAL_PATH)/windy/*.cpp)\
	$(wildcard $(LOCAL_PATH)/windy/mayclub/*.cpp)\
	$(wildcard $(LOCAL_PATH)/windy/nocturnal/*.cpp))

LOCAL_STATIC_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_gfx SDL2_mixer
LOCAL_LDLIBS := -ldl -llog -lz -lGLESv1_CM -lGLESv2
LOCAL_CFLAGS := -DVILE_ARCH_LINUX

include $(BUILD_SHARED_LIBRARY)


