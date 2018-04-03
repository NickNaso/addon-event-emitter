/*******************************************************************************
 * Copyright (c) 2018 Nicola Del Gobbo
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the license at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY
 * IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
 * MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 *
 * Contributors - initial API implementation:
 * Nicola Del Gobbo <nicoladelgobbo@gmail.com>
 ******************************************************************************/

#include <chrono>
#include <thread>
#include <iostream>

#include "native-emitter.h"

Napi::FunctionReference NativeEmitter::constructor;

Napi::Object NativeEmitter::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);
  
  Napi::Function func = DefineClass(env, "NativeEmitter", {
    InstanceMethod("callAndEmit", &NativeEmitter::CallAndEmit)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("NativeEmitter", func);
  return exports;
}

NativeEmitter::NativeEmitter(const Napi::CallbackInfo& info) 
: Napi::ObjectWrap<NativeEmitter>(info)  {
  // NOOP
}

Napi::Value NativeEmitter::CallAndEmit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    //Napi::Object _self =  info.This().As<Napi::Object>();
    Napi::Function emit = info.This().As<Napi::Object>().Get("emit").As<Napi::Function>();
    emit.Call(info.This(), { Napi::String::New(env, "start") });
    for(int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        emit.Call(info.This(), { Napi::String::New(env, "data"), Napi::String::New(env, "data ...") });
    }
    emit.Call(info.This(), { Napi::String::New(env, "end") });
    return Napi::String::New(env, "OK");
}