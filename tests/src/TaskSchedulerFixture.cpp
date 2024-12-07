/*
 * TaskSchedulerFixture.cpp
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
#include "TaskSchedulerFixture.h"


namespace zsp {
namespace ls {


TaskSchedulerFixture::TaskSchedulerFixture() : m_time_us(0) {

}

TaskSchedulerFixture::~TaskSchedulerFixture() {

}

void TaskSchedulerFixture::scheduleTask(jrpc::ITask *task, uint64_t n_us) {
    // Check whether this task is being scheduled for the first
    // time, or being rescheduled
    for (std::vector<std::pair<jrpc::ITask*,uint64_t>>::const_iterator
        it=m_queue.begin();
        it!=m_queue.end(); it++) {
        if (it->first == task) {
            m_queue.erase(it);
            break;
        }
    }

    // Now, insert the task based on its target time
    uint64_t target_time_us = m_time_us + n_us;
    bool inserted = false;

    for (uint32_t i=0; i<m_queue.size(); i++) {
        if (m_queue.at(i+1).second > target_time_us) {
            m_queue.insert(m_queue.begin()+i, {task, target_time_us});
        }
    }

    if (!inserted) {
        m_queue.push_back({task, target_time_us});
    }

    m_time_us++;
}

void TaskSchedulerFixture::cancelSchedule(jrpc::ITask *task) {
    for (std::vector<std::pair<jrpc::ITask*,uint64_t>>::const_iterator
        it=m_queue.begin();
        it!=m_queue.end(); it++) {
        if (it->first == task) {
            m_queue.erase(it);
            break;
        }
    }
}

bool TaskSchedulerFixture::advance() {
    if (m_queue.size()) {
        uint64_t target_time = m_queue.front().second;

        // Advance time to match tasks we're about to awaken
        // Note that sub-schedules will advance m_time_us
        m_time_us = target_time;
        while (m_queue.size() && m_queue.front().second == target_time) {
            jrpc::ITask *task = m_queue.front().first;
            m_queue.erase(m_queue.begin());
            task->queue();
        }

    } else {
        return false;
    }
}

}
}
