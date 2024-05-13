#include <ReactCommon/CallInvokerHolder.h>
#include <jsi/jsi.h>

#ifndef HNSWLIB_H
#define HNSWLIB_H

namespace RNHnswlib {
void install(jsi::Runtime &runtime,
             std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker);
}

#endif /* HNSWLIB_H */
