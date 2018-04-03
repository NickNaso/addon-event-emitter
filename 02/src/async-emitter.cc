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
#include <uv.h>
#include <mutex>

//#include "JSCaller"

class EmitterWorker : public Napi::AsyncWorker {
    public:
        EmitterWorker(const Napi::Function& callback, const Napi::Function& emitter)
            : Napi::AsyncWorker(callback), emit(Napi::Persistent(emitter)) {
                // Not use this code but wait that this PR will be landed
                // https://github.com/nodejs/node/pull/17887
                //JSCaller caller = new JSCaller();
                
                uv_async_init(
                    uv_default_loop(),
                    &async,
                    OnProgress
                );
                async.data = this;
                
        }

        ~EmitterWorker() {
        }

        inline static void OnProgress (uv_async_t *async) {
            EmitterWorker *worker = static_cast<EmitterWorker*>(async->data);
            Napi::HandleScope scope(worker->_env);
            worker->emit.Call({Napi::String::New(worker->_env, "data"), Napi::Number::New(worker->_env, worker->tasks)});
            worker->Decrease();
        }

        void Increase() {
            std::lock_guard<std::mutex> lk(async_lock);
            progress += 1;
        }

        void Decrease() {
            std::lock_guard<std::mutex> lk(async_lock);
            progress -= 1;
        }

        bool IsStoppable() {
            std::lock_guard<std::mutex> lk(async_lock);
            if ( progress == 0) {
                return true;
            }
            return false;
        }

        void WaitLoop() {
             while (!IsStoppable()) {;}
        }

        void End() {
            uv_close((uv_handle_t*) &async, NULL);
        }

        void Execute() {
            // Here some long running task and return piece of data exectuing some task
            for(int i = 0; i < 3; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                Increase();
                uv_async_send(&async);
            }
            WaitLoop();
            End();
        }

        void OnData(int progress) {
            std::cout <<  "Progress on the native side: " << progress << std::endl;  
            //emit.Call({Napi::String::New(Env(), "data"), Napi::Number::New(Env(), progress)});
            //emit.MakeCallback(Env().Global(), {Napi::String::New(Env(), "data"), Napi::Number::New(Env(), progress)});
        }

        void OnOK() {
            std::cout << "OnOK" << std::endl; 
            Napi::HandleScope scope(Env());
            emit.Call({Napi::String::New(Env(), "end")});
            Callback().Call({Napi::String::New(Env(), "OK")});
        }


    private:
        Napi::FunctionReference emit;
        int progress = 0;
        int tasks = 0;
        Napi::Env _env = Env();
        uv_async_t async;
        std::mutex async_lock;
};

Napi::Value CallAsyncEmit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function emitter = info[0].As<Napi::Function>();
    Napi::Function callback = info[1].As<Napi::Function>();
    emitter.Call({ Napi::String::New(env, "start") });
    EmitterWorker* emitterWorker = new EmitterWorker(callback, emitter);
    emitterWorker->Queue();
    return env.Undefined();
}

// Init
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "callAsyncEmit"), Napi::Function::New(env, CallAsyncEmit));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);