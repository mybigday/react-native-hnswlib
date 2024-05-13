package com.hnswlib;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;

public class HnswlibModule extends HnswlibSpec {
  public static final String NAME = "Hnswlib";

  HnswlibModule(ReactApplicationContext context) {
    super(context);
  }

  @Override
  @NonNull
  public String getName() {
    return NAME;
  }

  static {
    System.loadLibrary("react-native-hnswlib");
  }

  private static native void nativeInstall(long jsiPtr, CallInvokerHolderImpl jsCallInvokerHolder);

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      ReactApplicationContext context = getReactApplicationContext();
      CallInvokerHolderImpl holder = (CallInvokerHolderImpl) context.getCatalystInstance().getJSCallInvokerHolder();
      nativeInstall(context.getJavaScriptContextHolder().get(), holder);
      return true;
    } catch (Exception exception) {
      return false;
    }
  }
}
