#pragma once

#include <future>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include "RnHostClass.hpp"
#include "RnJsiContext.hpp"
#include "helper.h"
#include "hnswlib/hnswlib.h"

namespace RNHnswlib {
  void normalize_vector(float* data) {
    float norm = 0.0f;
    for (int i = 0; i < sizeof(data); i++)
      norm += data[i] * data[i];
    norm = 1.0f / (sqrtf(norm) + 1e-30f);
    for (int i = 0; i < sizeof(data); i++)
      data[i] *= norm;
  }

  class HierarchicalNSW: public RnJSI::HostClass<HierarchicalNSW> {
  public:
    // constructor(spaceName: SpaceName, numDimensions: number);
    JSI_HOST_OBJECT_CONSTRUCTOR(HierarchicalNSW) {
      JSI_ASSERT(runtime, argc == 2, "Expected 2 arguments");
      JSI_ASSERT(runtime, args[0].isString(), "Expected string");
      JSI_ASSERT(runtime, args[1].isNumber(), "Expected number");

      std::string spaceName = args[0].getString(runtime).utf8(runtime);
      int numDimensions = args[1].getNumber();

      if (spaceName == "l2") {
        space_ = std::make_unique<hnswlib::L2Space>(static_cast<size_t>(numDimensions));
      } else if (spaceName == "ip") {
        space_ = std::make_unique<hnswlib::InnerProductSpace>(static_cast<size_t>(numDimensions));
      } else if (spaceName == "cosine") {
        space_ = std::make_unique<hnswlib::InnerProductSpace>(static_cast<size_t>(numDimensions));
        normalize_ = true;
      } else {
        throw facebook::jsi::JSError(runtime, "Invalid space name");
      }
    }

    // initIndex(maxElementsOrOpts: number | object, m?: number, efConstruction?: number, randomSeed?: number, allowReplaceDeleted?: boolean): void;
    JSI_HOST_FUNCTION(initIndex) {
      JSI_ASSERT(runtime, argc >= 1, "Expected at least 1 argument");
      JSI_ASSERT(runtime, args[0].isNumber() || args[0].isObject(), "Expected number or object");
      if (argc >= 2) JSI_ASSERT(runtime, args[1].isNumber(), "Expected number");
      if (argc >= 3) JSI_ASSERT(runtime, args[2].isNumber(), "Expected number");
      if (argc >= 4) JSI_ASSERT(runtime, args[3].isNumber(), "Expected number");
      if (argc >= 5) JSI_ASSERT(runtime, args[4].isBool(), "Expected boolean");

      const bool use_opts = args[0].isObject();

      if (use_opts)
        JSI_ASSERT(runtime, args[0].getObject(runtime).hasProperty(runtime, "maxElements"), "`maxElements` is required");

      const uint32_t max_elements = use_opts ? args[0].getObject(runtime).getProperty(runtime, "maxElements").getNumber() : args[0].getNumber();
      const uint32_t m = use_opts
        ? JSI_GET_PROPERTY_OR_DEFAULT(args[0].getObject(runtime), runtime, "m", 16).getNumber()
        : JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 1, 16).getNumber();

      const uint32_t ef_construction = use_opts
        ? JSI_GET_PROPERTY_OR_DEFAULT(args[0].getObject(runtime), runtime, "efConstruction", 200).getNumber()
        : JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 2, 200).getNumber();

      const uint32_t random_seed = use_opts
        ? JSI_GET_PROPERTY_OR_DEFAULT(args[0].getObject(runtime), runtime, "randomSeed", 100).getNumber()
        : JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 3, 100).getNumber();

      const bool allow_replace_deleted = use_opts
        ? JSI_GET_PROPERTY_OR_DEFAULT(args[0].getObject(runtime), runtime, "allowReplaceDeleted", false).getBool()
        : JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 4, false).getBool();

      if (index_ != nullptr) index_.reset();

      try {
        index_ = std::make_unique<hnswlib::HierarchicalNSW<float>>(space_.get(), max_elements, m, ef_construction, random_seed, allow_replace_deleted);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // readIndex(filename: string, allowReplaceDeleted?: boolean): Promise<boolean>;
    JSI_HOST_FUNCTION(readIndex) {
      JSI_ASSERT(runtime, argc >= 1, "Expected at least 1 argument");
      JSI_ASSERT(runtime, args[0].isString(), "Expected string");
      if (argc >= 2) JSI_ASSERT(runtime, args[1].isBool(), "Expected boolean");

      std::string filename = args[0].getString(runtime).utf8(runtime);
      bool allow_replace_deleted = JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 1, false).getBool();

      return JSI_CREATE_PROMISE(runtime, {
        if (index_ != nullptr) index_.reset();

        try {
          index_ = std::make_unique<hnswlib::HierarchicalNSW<float>>(space_.get(), filename, false, 0, allow_replace_deleted);
          resolve.call(runtime, true);
        } catch (const std::exception &e) {
          reject.call(runtime, e.what());
        }
      }, this, filename, allow_replace_deleted);
    }

    // readIndexSync(filename: string, allowReplaceDeleted?: boolean): void;
    JSI_HOST_FUNCTION(readIndexSync) {
      JSI_ASSERT(runtime, argc >= 1, "Expected at least 1 argument");
      JSI_ASSERT(runtime, args[0].isString(), "Expected string");
      if (argc >= 2) JSI_ASSERT(runtime, args[1].isBool(), "Expected boolean");

      const std::string filename = args[0].getString(runtime).utf8(runtime);
      const bool allow_replace_deleted = JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 1, false).getBool();

      if (index_ != nullptr) index_.reset();

      try {
        index_ = std::make_unique<hnswlib::HierarchicalNSW<float>>(space_.get(), filename, false, 0, allow_replace_deleted);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // writeIndex(filename: string): Promise<void>;
    JSI_HOST_FUNCTION(writeIndex) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isString(), "Expected string");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const std::string filename = args[0].getString(runtime).utf8(runtime);

      return JSI_CREATE_PROMISE(runtime, {
        try {
          index_->saveIndex(filename);
          resolve.call(runtime);
        } catch (const std::exception &e) {
          reject.call(runtime, e.what());
        }
      }, this, filename);
    }

    // writeIndexSync(filename: string): void;
    JSI_HOST_FUNCTION(writeIndexSync) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isString(), "Expected string");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const std::string filename = args[0].getString(runtime).utf8(runtime);

      try {
        index_->saveIndex(filename);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // resizeIndex(newMaxElements: number): void;
    JSI_HOST_FUNCTION(resizeIndex) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isNumber(), "Expected number");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const uint32_t new_max_elements = args[0].getNumber();

      try {
        index_->resizeIndex(new_max_elements);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // addPoint(point: number[], label: number, replaceDeleted?: boolean): void;
    JSI_HOST_FUNCTION(addPoint) {
      JSI_ASSERT(runtime, argc >= 2, "Expected at least 2 arguments");
      JSI_ASSERT(runtime, args[0].isArray(), "Expected array");
      JSI_ASSERT(runtime, args[1].isNumber(), "Expected number");
      if (argc >= 3) JSI_ASSERT(runtime, args[2].isBool(), "Expected boolean");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      std::vector<float> vector;
      auto point = args[0].asArray(runtime);
      for (size_t i = 0; i < point.length(runtime); i++) {
        vector.push_back(point.getValueAtIndex(runtime, i).getNumber());
      }
      const uint32_t label = args[1].getNumber();
      const bool replace_deleted = JSI_GET_ARGUMENT_OR_DEFAULT(args, runtime, 2, false).getBool();

      if (normalize_) {
        normalize_vector(vector.data());
      }

      try {
        index_->addPoint(vector.data(), label, replace_deleted);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // markDelete(label: number): void;
    JSI_HOST_FUNCTION(markDelete) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isNumber(), "Expected number");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const uint32_t label = args[0].getNumber();

      try {
        index_->markDelete(label);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // unmarkDelete(label: number): void;
    JSI_HOST_FUNCTION(unmarkDelete) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isNumber(), "Expected number");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const uint32_t label = args[0].getNumber();

      try {
        index_->unmarkDelete(label);
      } catch (const std::exception &e) {
        throw facebook::jsi::JSError(runtime, e.what());
      }

      return facebook::jsi::Value::undefined();
    }

    // searchKnn(queryPoint: number[], numNeighbors: number, filter?: FilterFunction): {distances: number[], neighbors: number[]};
    JSI_HOST_FUNCTION(searchKnn) {
      JSI_ASSERT(runtime, argc >= 2, "Expected at least 2 arguments");
      JSI_ASSERT(runtime, args[0].isArray(), "Expected array");
      JSI_ASSERT(runtime, args[1].isNumber(), "Expected number");
      if (argc >= 3) JSI_ASSERT(runtime, args[2].isObject(), "Expected object");

      std::vector<float> query_vector;
      auto query_point = args[0].asArray(runtime);
      for (size_t i = 0; i < query_point.length(runtime); i++) {
        query_vector.push_back(query_point.getValueAtIndex(runtime, i).getNumber());
      }
      const uint32_t num_neighbors = args[1].getNumber();

      hnswlib::BaseFilterFunctor *filter = nullptr;
      if (argc >= 3) {
        auto filter_function = args[2].getObject(runtime).asFunction(runtime);
        filter = new hnswlib::BaseFilterFunctor([filter_function](int label) {
          auto future = RnJsi::Context::invoke(filter_function, {label});
          return future.get().getBool();
        });
      }

      std::priority_queue<std::pair<float, hnswlib::labeltype>> result = index_->searchKnn(query_vector.data(), num_neighbors, filter);

      std::vector<float> distances;
      std::vector<hnswlib::labeltype> neighbors;
      while (!result.empty()) {
        distances.push_back(result.top().first);
        neighbors.push_back(result.top().second);
        result.pop();
      }

      if (filter != nullptr) delete filter;

      auto resultObject = jsi::Object(runtime);
      resultObject.setProperty(runtime, "distances", jsi::Array(runtime, distances));
      resultObject.setProperty(runtime, "neighbors", jsi::Array(runtime, neighbors));

      return resultObject;
    }

    // getIdsList(): number[];
    JSI_HOST_FUNCTION(getIdsList) {
      JSI_ASSERT(runtime, argc == 0, "Expected 0 arguments");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      std::vector<hnswlib::labeltype> ids_list = index_->getIdsList();

      return jsi::Array(runtime, ids_list);
    }

    // getPoint(label: number): number[];
    JSI_HOST_FUNCTION(getPoint) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isNumber(), "Expected number");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const uint32_t label = args[0].getNumber();

      std::vector<float> point = index_->getDataByLabel<float>(static_cast<size_t>(label));

      return jsi::Array(runtime, point);
    }

    // getMaxElements(): number;
    JSI_HOST_FUNCTION(getMaxElements) {
      JSI_ASSERT(runtime, argc == 0, "Expected 0 arguments");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      return index_->maxelements_;
    }

    // getCurrentCount(): number;
    JSI_HOST_FUNCTION(getCurrentCount) {
      JSI_ASSERT(runtime, argc == 0, "Expected 0 arguments");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      return index_->cur_element_count;
    }

    // getNumDimensions(): number;
    JSI_HOST_FUNCTION(getNumDimensions) {
      JSI_ASSERT(runtime, argc == 0, "Expected 0 arguments");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      return index_->data_size_ / sizeof(float);
    }

    // getEf(): number;
    JSI_HOST_FUNCTION(getEf) {
      JSI_ASSERT(runtime, argc == 0, "Expected 0 arguments");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      return index_->ef_;
    }

    // setEf(ef: number): void;
    JSI_HOST_FUNCTION(setEf) {
      JSI_ASSERT(runtime, argc == 1 && args[0].isNumber(), "Expected number");
      JSI_ASSERT(runtime, index_ == nullptr, "Index is not initialized");

      const uint32_t ef = args[0].getNumber();

      index_->ef_ = ef;

      return facebook::jsi::Value::undefined();
    }

    JSI_HOST_CLASS_DEFINE_METHODS({
      {"initIndex", JSI_BIND_METHOD(initIndex)},
      {"readIndex", JSI_BIND_METHOD(readIndex)},
      {"readIndexSync", JSI_BIND_METHOD(readIndexSync)},
      {"writeIndex", JSI_BIND_METHOD(writeIndex)},
      {"writeIndexSync", JSI_BIND_METHOD(writeIndexSync)},
      {"resizeIndex", JSI_BIND_METHOD(resizeIndex)},
      {"addPoint", JSI_BIND_METHOD(addPoint)},
      {"markDelete", JSI_BIND_METHOD(markDelete)},
      {"unmarkDelete", JSI_BIND_METHOD(unmarkDelete)},
      {"searchKnn", JSI_BIND_METHOD(searchKnn)},
      {"getIdsList", JSI_BIND_METHOD(getIdsList)},
      {"getPoint", JSI_BIND_METHOD(getPoint)},
      {"getMaxElements", JSI_BIND_METHOD(getMaxElements)},
      {"getCurrentCount", JSI_BIND_METHOD(getCurrentCount)},
      {"getNumDimensions", JSI_BIND_METHOD(getNumDimensions)},
      {"getEf", JSI_BIND_METHOD(getEf)},
      {"setEf", JSI_BIND_METHOD(setEf)}
    })

  private:
    std::unique_ptr<hnswlib::HierarchicalNSW<float>> index_ = nullptr;
    std::unique_ptr<hnswlib::SpaceInterface<float>> space_ = nullptr;
    bool normalize_ = false;
  };
}
