#pragma once

#include "RnJsiContext.hpp"
#include <jsi/jsi.h>

#define JSI_HOST_OBJECT_CONSTRUCTOR(NAME)                                      \
  NAME(jsi::Runtime &runtime, const jsi::Object &thisValue,                    \
       const jsi::Value *args, size_t argc)                                    \
      : RnJSI::HostClass()

#define JSI_HOST_CLASS_DEFINE_METHODS(METHODS)                                 \
  std::unordered_map<std::string, jsi::Function> getMethods() const override { \
    return METHODS;                                                            \
  }

namespace RnJsi {
template <typename T> class HostClass : public facebook::jsi::HostObject {
public:
  HostClass() = default;

  static JSI_HOST_FUNCTION(constructor) {
    auto instance = std::make_shared<T>(runtime, thisValue, args, argc);
    auto prototype = jsi::Object(runtime);
    for (auto const &[key, value] : instance->getMethods()) {
      prototype.setProperty(runtime, key, value);
    }
    instance->setProperty(runtime, "prototype", prototype);
    return instance;
  }

  static jsi::Function getConstructor(jsi::Runtime &runtime) {
    return JSI_DEFINE_FUNCTION(runtime, constructor, 0,
                               JSI_BIND_METHOD(constructor));
  }

protected:
  virtual std::unordered_map<std::string, jsi::Function> getMethods() const = 0;
};
} // namespace RnJsi
