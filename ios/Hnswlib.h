#ifdef __cplusplus
#import "react-native-hnswlib.h"
#endif

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNHnswlibSpec.h"

@interface Hnswlib : NSObject <NativeHnswlibSpec>
#else
#import <React/RCTBridgeModule.h>

@interface Hnswlib : NSObject <RCTBridgeModule>
#endif

@end
