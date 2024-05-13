#import "Hnswlib.h"

@implementation Hnswlib

@synthesize bridge = _bridge;

RCT_EXPORT_MODULE()

- (void)invalidate {
  _manager.reset();
  _bridge = nil;
}

- (void)setBridge:(RCTBridge *)bridge {
  _bridge = bridge;
}

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

void installApi(
  std::shared_ptr<facebook::react::CallInvoker> callInvoker,
  facebook::jsi::Runtime *runtime
) {
  RNHnswlib::install(runtime, callInvoker);
}

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)_bridge;
  if (cxxBridge.runtime != nullptr) {
    auto callInvoker = cxxBridge.jsCallInvoker;
    facebook::jsi::Runtime *jsRuntime =
        (facebook::jsi::Runtime *)cxxBridge.runtime;

    installApi(callInvoker, jsRuntime);
    return @(true);
  }
  return @(false);
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params
{
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)_bridge;
  auto callInvoker = cxxBridge.jsCallInvoker;
  facebook::jsi::Runtime *jsRuntime = (facebook::jsi::Runtime *)cxxBridge.runtime;

  installApi(callInvoker, jsRuntime);

  return std::make_shared<facebook::react::NativeJsiUdpSpecJSI>(params);
}
#endif

@end
