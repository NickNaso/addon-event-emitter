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

#include <napi.h>
#include <uv.h>
#include <mutex>
#include <iostream>

class JSEmitter {
    public:
        JSEmitter(Napi::AsyncWorker* data, uv_async_cb cb) {
            uv_async_init(
                uv_default_loop(),
                &async,
                cb
            );
            async.data = data;
        }
        void Exec() {
            this->IncrementEvents();
            uv_async_send(&async);
            this->DecrementEvents();
        }
        bool IsStoppable() {
            std::lock_guard<std::mutex> lk(async_lock);
            if ( events == 0) {
                return true;
            }
            return false;
        }
        void WaitLoop() {
             while (!IsStoppable()) {;}
        }
        void End() {
            WaitLoop();
            uv_close((uv_handle_t*) &async, NULL);
        }
    private:
        void DecrementEvents() {
            std::lock_guard<std::mutex> lk(async_lock);
            events -= 1;
        }
        void IncrementEvents() {
            std::lock_guard<std::mutex> lk(async_lock);
            events += 1;
        }
        uv_async_t async;
        std::mutex async_lock;
        int events = 0;
};