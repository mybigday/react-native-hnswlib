#pragma once

#define JSI_HOST_FUNCTION(NAME)                                                \
  facebook::jsi::Value NAME(facebook::jsi::Runtime &runtime,                   \
                            const facebook::jsi::Value &thisValue,             \
                            const facebook::jsi::Value *args, size_t argc)

#define JSI_DEFINE_FUNCTION(RUNTIME, NAME, ARGC, FUNCTION)                     \
  facebook::jsi::Function::createFromHostFunction(                             \
      RUNTIME, facebook::jsi::PropNameID::forAscii(RUNTIME, #NAME), ARGC,      \
      FUNCTION)

#define JSI_EXPOSE_FUNCTION(RUNTIME, NAME, ARGC, FUNCTION)                     \
  {                                                                            \
    auto NAME = JSI_DEFINE_FUNCTION(RUNTIME, NAME, ARGC, FUNCTION);            \
    RUNTIME.global().setProperty(RUNTIME, #NAME, std::move(NAME));             \
  }

#define JSI_BIND_METHOD(METHOD)                                                \
  std::bind(&METHOD, this, std::placeholders::_1, std::placeholders::_2,       \
            std::placeholders::_3, std::placeholders::_4)

#define JSI_ASSERT(RUNTIME, CONDITION, MESSAGE)                                \
  if (!(CONDITION))                                                            \
  throw facebook::jsi::JSError(RUNTIME, MESSAGE)

#define JSI_GET_PROPERTY_OR_DEFAULT(OBJECT, RUNTIME, PROPERTY, DEFAULT)        \
  OBJECT.getProperty(RUNTIME, PROPERTY).isUndefined()                          \
      ? DEFAULT                                                                \
      : OBJECT.getProperty(RUNTIME, PROPERTY)

#define JSI_GET_ARGUMENT_OR_DEFAULT(ARGS, RUNTIME, INDEX, DEFAULT)             \
  ARGS.size() <= INDEX ? DEFAULT : ARGS[INDEX]

#define JSI_CREATE_PROMISE(RUNTIME, EXECUTOR, ...)                             \
  RUNTIME.global()                                                             \
      .getPropertyAsFunction(RUNTIME, "Promise")                               \
      .callAsConstructor(                                                      \
          RUNTIME,                                                             \
          facebook::jsi::Function::createFromHostFunction(                     \
              RUNTIME,                                                         \
              facebook::jsi::PropNameID::forAscii(RUNTIME, "executor"), 2,     \
              [__VA_ARGS__](facebook::jsi::Runtime &runtime,                   \
                            const facebook::jsi::Value &thisValue,             \
                            const facebook::jsi::Value *args,                  \
                            size_t argc) -> facebook::jsi::Value {             \
                auto resolve = args[0].getObject(runtime).asFunction(runtime); \
                auto reject = args[1].getObject(runtime).asFunction(runtime);  \
                EXECUTOR                                                       \
              }))
