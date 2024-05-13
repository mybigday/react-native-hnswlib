import { NativeModules, Platform } from 'react-native';

const LINKING_ERROR =
  `The package 'react-native-hnswlib' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo Go\n';

// @ts-expect-error
const isTurboModuleEnabled = global.__turboModuleProxy != null;

const HnswlibModule = isTurboModuleEnabled
  ? require('./NativeHnswlib').default
  : NativeModules.Hnswlib;

const Hnswlib = HnswlibModule
  ? HnswlibModule
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      }
    );

if (typeof hnswlib === 'undefined') {
  Hnswlib.install();
}

export const { HierarchicalNSW, InnerProductSpace, L2Space, BruteforceSearch } =
  hnswlib;
