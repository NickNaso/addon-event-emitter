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
 * Mauro Doganieri <mauro.doganieri@gmail.com>
 ******************************************************************************/

#include<napi.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <uv.h>
#include <mutex>

/*class ProgressWorker : public Napi::AsyncWorker {
    public:
        ProgressWorker(const Napi::Function& callback) : Napi::AsyncWorker(callback), p(i) {
        }

        ~ProgressWorker(){
        }

        void Execute() {
        }

        void OnOk() {
            Napi::HandleScope scope(Env());
            Callback().Call({Napi::String::New(Env(), "data")});
        }

        int p;
};*/


/*void  on_progress(uv_async_t *async) {
    std::cout <<  "Called on worker thread" << std::endl;  
};*/

class EmitterWorker : public Napi::AsyncWorker {
    public:
        EmitterWorker(const Napi::Function& callback, const Napi::Function& emitter)
            : Napi::AsyncWorker(callback), emit(Napi::Persistent(emitter)) {
                uv_async_init(
                    uv_default_loop(),
                    &async,
                    OnProgress
                );
                async.data = this;
                
        }

        ~EmitterWorker() {
        }

        /*void  on_progress(uv_async_t *async) {
            std::cout <<  "Called on worker thread" << std::endl;  
        }*/

        inline static void OnProgress (uv_async_t *async) {
            std::cout << "OnProgress" << std::endl;
            EmitterWorker *worker = static_cast<EmitterWorker*>(async->data);
            //std::cout <<  "Called on worker thread " << worker->progress << std::endl; 
            Napi::HandleScope scope(worker->_env);
            worker->emit.Call({Napi::String::New(worker->_env, "data"), Napi::Number::New(worker->_env, 123)});
            worker->Decrease();
        }

        void Increase() {
            std::lock_guard<std::mutex> lk(async_lock);
            //uv_mutex_lock(&async_lock);
            progress += 1;
            //uv_mutex_unlock(&async_lock);
        }

        void Decrease() {
            std::lock_guard<std::mutex> lk(async_lock);
            //uv_mutex_lock(&async_lock);
            progress -= 1;
            //uv_mutex_unlock(&async_lock);
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
                //this->OnData(i); 
                Increase();
                uv_async_send(&async);
            }
            //uv_close((uv_handle_t*) &async, NULL);   
            /*end = true;
            while(!end) {
                std::cout << "Not close please" << std::endl;
            }*/
            //std::this_thread::sleep_for(std::chrono::seconds(3));
            //uv_close((uv_handle_t*) &async, NULL); 
            /*uv_mutex_lock(&async_lock);
            if (progress == 0) {
                End();
            }
            uv_mutex_unlock(&async_lock);*/
            //while (!IsStoppable()) {;}
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
            // emit.Reset();
            // emitter.MakeCallback(Env().Global(), {Napi::String::New(Env(), "end")});
            Callback().Call({Napi::String::New(Env(), "OK")});
        }


    private:
        Napi::FunctionReference emit;
        int progress = 0;
        //uv_mutex_t async_lock;
        Napi::Env _env = Env();
        uv_async_t async;
        std::mutex async_lock;
        //bool end = false;
        
};

Napi::Value CallAsyncEmit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function emitter = info[0].As<Napi::Function>();
    Napi::Function callback = info[1].As<Napi::Function>();
    emitter.Call({Napi::String::New(env, "start")});
    // emitter.Call({Napi::String::New(env, "start"), Napi::String::New(env, "data ...")});
    // emitter.Call({Napi::String::New(env, "end")});
    // return Napi::String::New(env, "OK");
    EmitterWorker* emitterWorker = new EmitterWorker(callback, emitter);
    //emitterWorker->Receiver().Set("emitter", emitter);
    emitterWorker->Queue();
    return env.Undefined();
}


// All work but it's not a good practice bacause all long running task should be
// executed out of the event loop
Napi::Value CallEmit(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Function emitter = info[0].As<Napi::Function>();
    emitter.Call({Napi::String::New(env, "start")});
    // Here some long running task and return piece of data exectuing some task
    for(int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        emitter.Call({Napi::String::New(env, "data"), Napi::String::New(env, "data ...")});
    }
    emitter.Call({Napi::String::New(env, "end")});
    return Napi::String::New(env, "OK");
}

// Init
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "callEmit"), Napi::Function::New(env, CallEmit));
    exports.Set(Napi::String::New(env, "callAsyncEmit"), Napi::Function::New(env, CallAsyncEmit));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);