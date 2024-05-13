#include <memory>
#include <jni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include "react-native-hnswlib.h"

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_hnswlib_HnswlibModule_nativeinstall(JNIEnv *env, jclass _, jlong jsiPtr, jobject jsCallInvokerHolder) {
  auto runtime { reinterpret_cast<facebook::jsi::Runtime*>(jsiPtr) };
  auto jsCallInvoker {
    facebook::jni::alias_ref<facebook::react::CallInvokerHolder::javaobject>{
      reinterpret_cast<facebook::react::CallInvokerHolder::javaobject>(jsCallInvokerHolder)
    }->cthis()->getCallInvoker()
  };

  RNHnswlib::install(runtime, std::move(jsCallInvoker));
}
