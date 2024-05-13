#pragma once

#include <ReactCommon/CallInvokerHolder.h>
#include <exception>
#include <future>
#include <jsi/jsi.h>
#include <memory>

namespace RnJsi {

class Context {
public:
  Context(facebook::jsi::Runtime *runtime,
          std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
      : runtime_(runtime), jsCallInvoker_(jsCallInvoker) {}

  static inline void
  init(facebook::jsi::Runtime *runtime,
       std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker) {
    instance.reset(new Context(runtime, jsCallInvoker));
  }

  static std::future<facebook::jsi::Value>
  invoke(facebook::jsi::Function fn,
         std::initializer_list<facebook::jsi::Value> args) {
    std::promise<std::string> promise;
    std::future<facebook::jsi::Value> future = promise.get_future();
    if (instance && instance->jsCallInvoker) {
      instance->jsCallInvoker->invokeAsync(
          [&fn, &args, &promise, runtime = instance->runtime] {
            try {
              promise.set_value(fn.call(*runtime, args));
            } catch (...) {
              promise.set_exception(std::current_exception());
            }
          });
    } else {
      promise.set_exception(std::make_exception_ptr(
          std::runtime_error("Context not initialized")));
    }
    return future;
  }

private:
  static std::shared_ptr<Context> instance{nullptr};
  std::shared_ptr<facebook::jsi::Runtime> runtime_;
  std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker_;
};

} // namespace RnJsi
