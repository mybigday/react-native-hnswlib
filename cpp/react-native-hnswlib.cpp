#include "react-native-hnswlib.h"
#include "HierarchicalNSW.hpp"
#include "RnJsiContext.hpp"

namespace RNHnswlib {
void install(jsi::Runtime *runtime,
             std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker) {
  RnJsi::Context::init(runtime, jsCallInvoker);
  auto hnswlib = jsi::Object(*runtime);
  hnswlib.setProperty(*runtime, "HierarchicalNSW",
                      HierarchicalNSW::getConstructor(runtime));
  runtime->global().setProperty(runtime, "hnswlib", hnswlib);
}
} // namespace RNHnswlib
