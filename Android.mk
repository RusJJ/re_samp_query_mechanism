LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE := squery
LOCAL_SRC_FILES := main.cpp jni_interface.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
