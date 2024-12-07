/**
 * TaskSchedulerFixture.h
 *
 * Copyright 2023 Matthew Ballance and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may 
 * not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * Created on:
 *     Author: 
 */
#pragma once
#include <vector>
#include "jrpc/ITaskScheduler.h"

namespace zsp {
namespace ls {



class TaskSchedulerFixture :
    public jrpc::ITaskScheduler {
public:
    TaskSchedulerFixture();

    virtual ~TaskSchedulerFixture();

    virtual void scheduleTask(jrpc::ITask *task, uint64_t n_us) override;

    virtual void cancelSchedule(jrpc::ITask *task) override;

    bool advance();

private:
    uint64_t                                        m_time_us;
    std::vector<std::pair<jrpc::ITask *, uint64_t>> m_queue;

};

}
}


