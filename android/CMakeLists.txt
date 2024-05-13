cmake_minimum_required(VERSION 3.4.1)
project(Hnswlib)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)

file(GLOB LIBRN_DIR "${PREBUILT_DIR}/${ANDROID_ABI}")
file(GLOB libfbjni_link_DIRS "${build_DIR}/fbjni-*.aar/jni/${ANDROID_ABI}")
file(GLOB libfbjni_include_DIRS "${build_DIR}/fbjni-*-headers.jar/")

if(${REACT_NATIVE_VERSION} LESS 66)
  file(
    TO_CMAKE_PATH
    "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
    INCLUDE_JSI_CPP
  )
endif()

if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
  # We need to find packages since from RN 0.71 binaries are prebuilt
  find_package(fbjni REQUIRED CONFIG)
  find_package(ReactAndroid REQUIRED CONFIG)
endif()

if(${REACT_NATIVE_VERSION} LESS 66)
  # JSI lib didn't exist on RN 0.65 and before. Simply omit it.
  set(JSI_LIB "")
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
  # RN 0.71 distributes prebuilt binaries.
  set(JSI_LIB ReactAndroid::jsi)
else()
  # RN 0.66 distributes libjsi.so, can be used instead of compiling jsi.cpp manually.
  find_library(
    JSI_LIB
    jsi
    PATHS ${LIBRN_DIR}
    NO_CMAKE_FIND_ROOT_PATH
  )
endif()

if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
  # RN 0.71 distributes prebuilt binaries.
  set(REACT_LIB ReactAndroid::react_nativemodule_core)
else()
  find_library(
    REACT_LIB
    react_nativemodule_core
    PATHS ${LIBRN_DIR}
    NO_CMAKE_FIND_ROOT_PATH
  )
endif()

if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
  # RN 0.71 distributes prebuilt binaries.
  set (FBJNI_LIBRARY fbjni::fbjni)
else()
  find_library(
    FBJNI_LIBRARY
    fbjni
    PATHS ${libfbjni_link_DIRS}
    NO_CMAKE_FIND_ROOT_PATH
  )
endif()

if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
  # RN 0.71 distributes prebuilt binaries.
  set (TURBOMODULES_LIB "ReactAndroid::turbomodulejsijni")
else()
  find_library(
    TURBOMODULES_LIB
    turbomodulejsijni
    PATHS ${LIBRN_DIR}
    NO_CMAKE_FIND_ROOT_PATH
  )
endif()

find_library(LOG_LIB log)

add_library(react-native-hnswlib            SHARED
            ../cpp/react-native-hnswlib.cpp
            cpp-adapter.cpp
            ${LOG_LIB}
            ${FBJNI_LIBRARY}
            ${REACT_LIB}
            ${JSI_LIB}
            ${TURBOMODULES_LIB}
            android
)

# Specifies a path to native header files.
include_directories(../cpp ../hnswlib)
