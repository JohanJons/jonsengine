LOCAL_PATH:= $(call my-dir)
TOP_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE    := jonsengine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/ \
					$(LOCAL_PATH)/../include/Core/ \
					$(LOCAL_PATH)/../interface/ \
					$(LOCAL_PATH)/../include/Render/ \
					$(LOCAL_PATH)/../include/Core/Utils/ \
					$(LOCAL_PATH)/../include/Core/Logging/ \
					$(LOCAL_PATH)/../include/Core/GameObject/ \
					$(LOCAL_PATH)/../include/Core/Containers/ \
					$(LOCAL_PATH)/../include/Core/Memory/ \
					$(LOCAL_PATH)/../../Thirdparty/dlmalloc/ \
					$(LOCAL_PATH)/../../Thirdparty/boost_1_49_0/
				
# Core
LOCAL_SRC_FILES :=	../src/Core/Engine.cpp \
					../src/Core/EngineSettings.cpp
					
# Threading
LOCAL_SRC_FILES +=	../src/Core/Threading/Thread.cpp

# Rendering
LOCAL_SRC_FILES +=	../src/Render/RenderManagerImpl.cpp

# Utils
LOCAL_SRC_FILES +=	../src/Core/Utils/JonsTime.cpp

# Memory
LOCAL_SRC_FILES +=	../src/Core/Memory/MemoryManagerImpl.cpp \
					../src/Core/Memory/HeapAllocator.cpp
					
# GameObject
LOCAL_SRC_FILES +=	../src/Core/GameObject/GameObjectManagerImpl.cpp

# Logging
LOCAL_SRC_FILES +=	../src/Core/Logging/LogManagerImpl.cpp \
					../src/Core/Logging/JonsStreamBuf.cpp \
					../src/Core/Logging/JonsOutputStream.cpp
					
# Thirdparty
LOCAL_SRC_FILES +=	../../Thirdparty/dlmalloc/dlmalloc.c
					

LOCAL_CFLAGS := -DSTRUCT_MALLINFO_DECLARED
LOCAL_LDLIBS := -lGLESv2 -llog
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_LDLIBS := $(LOCAL_LDLIBS)

include $(BUILD_STATIC_LIBRARY)


# include the testing mk
include $(TOP_PATH)/../../JonsEngineTests/jni/Android.mk