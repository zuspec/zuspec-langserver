/**
 * TestTaskBase.h
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
#include "jrpc/ITaskQueue.h"
#include "Context.h"
#include "SourceFileCollection.h"
#include "TestBase.h"
#include "TestClient.h"

namespace zsp {
namespace ls {



class TestTaskBase : public TestBase {
public:
    TestTaskBase();

    virtual ~TestTaskBase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    void initWorkspace(
        const std::vector<std::pair<std::string,std::string>>   &files);

    bool runTasks(int32_t max);

protected:
    jrpc::ITaskQueueUP          m_queue;
    ContextUP                   m_ctxt;
    TestClient                  m_client;

};

}
}


