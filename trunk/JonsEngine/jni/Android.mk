LOCAL_PATH:= $(call my-dir)
TOP_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
# Main engine
LOCAL_MODULE    := jonsengine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/ \
					$(LOCAL_PATH)/../include/Core/ \
					$(LOCAL_PATH)/../interface/ \
					$(LOCAL_PATH)/../include/Render/ \
					$(LOCAL_PATH)/../include/Core/Utils/ \
					$(LOCAL_PATH)/../include/Core/Logging/ \
					$(LOCAL_PATH)/../include/Core/Object/ \
					$(LOCAL_PATH)/../include/Core/Memory/
					
# Core
LOCAL_SRC_FILES :=	../src/Core/Engine.cpp

# Rendering
LOCAL_SRC_FILES +=	../src/Render/RenderManagerImpl.cpp

# Utils
LOCAL_SRC_FILES +=	../src/Core/Utils/JonsTime.cpp \
					../src/Core/Utils/JonsThread.cpp

# Memory
LOCAL_SRC_FILES +=	../src/Core/Memory/MemoryManagerImpl.cpp \
					../src/Core/Memory/dlmalloc.c
					
# Object
LOCAL_SRC_FILES +=	../src/Core/Object/ObjectManagerImpl.cpp

# Logging
LOCAL_SRC_FILES +=	../src/Core/Logging/LogManagerImpl.cpp \
					../src/Core/Logging/JonsStreamBuf.cpp \
					../src/Core/Logging/JonsOutputStream.cpp
					
					

LOCAL_CFLAGS := -DSTRUCT_MALLINFO_DECLARED
LOCAL_LDLIBS := -lGLESv2 -llog
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_LDLIBS := $(LOCAL_LDLIBS)

include $(BUILD_STATIC_LIBRARY)



# Testing library
include $(CLEAR_VARS)

LOCAL_MODULE    := jonsenginetests
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../Tests/ \
					$(LOCAL_PATH)/../Tests/Core/Memory/ \
					$(LOCAL_PATH)/../Tests/Core/Utils/ \
					$(LOCAL_PATH)/../Tests/Core/
					
LOCAL_SRC_FILES :=	../Tests/TestManager.cpp \
					../Tests/Core/Memory/MemoryTestManager.cpp \
					../Tests/Core/GameObject/GameObjectTestManager.cpp \
					../Tests/TestClass1.cpp

LOCAL_CFLAGS :=
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_STATIC_LIBRARIES := jonsengine
LOCAL_LDLIBS    :=-llog

include $(BUILD_SHARED_LIBRARY)
