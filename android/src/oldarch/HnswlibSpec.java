package com.hnswlib;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.Promise;

abstract class HnswlibSpec extends ReactContextBaseJavaModule {
  HnswlibSpec(ReactApplicationContext context) {
    super(context);
  }

  public abstract boolean install();
}
