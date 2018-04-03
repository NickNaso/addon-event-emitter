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

#include<napi.h>

#include <chrono>
#include <thread>
#include <iostream>

// All work but it's not a good practice bacause all long running task should be
// executed out of the event loop
Napi::Value CallEmit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function emit = info[0].As<Napi::Function>();
    emit.Call({Napi::String::New(env, "start")});
    // Here some long running task and return piece of data exectuing some task
    for(int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        emit.Call({Napi::String::New(env, "data"), Napi::String::New(env, "data ...")});
    }
    emit.Call({Napi::String::New(env, "end")});
    return Napi::String::New(env, "OK");
}

// Init
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "callEmit"), Napi::Function::New(env, CallEmit));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);