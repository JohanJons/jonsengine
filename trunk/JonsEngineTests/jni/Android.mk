# Testing library
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= jonsenginetests
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../Core/Memory/ \
					$(LOCAL_PATH)/../Core/Utils/ \
					$(LOCAL_PATH)/../Core/
					
LOCAL_SRC_FILES :=	../TestManager.cpp \
					../Core/Memory/MemoryTestManager.cpp \
					../Core/GameObject/GameObjectTestManager.cpp \
					../TestClass1.cpp

LOCAL_CFLAGS :=
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_STATIC_LIBRARIES := jonsengine
LOCAL_LDLIBS    :=-llog

include $(BUILD_SHARED_LIBRARY)