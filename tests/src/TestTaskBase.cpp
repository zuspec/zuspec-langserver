/*
 * TestTaskBase.cpp
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
#include "TestTaskBase.h"
#include "TaskWorkspaceStartup.h"


namespace zsp {
namespace ls {


TestTaskBase::TestTaskBase() {

}

TestTaskBase::~TestTaskBase() {

}

void TestTaskBase::SetUp() {
    TestBase::SetUp();
    m_queue = jrpc::ITaskQueueUP(m_jrpc_factory->mkTaskQueue(0));
    m_client.init(m_dmgr);
    m_ctxt = ContextUP(new Context(
        m_dmgr,
        m_queue.get(),
        m_lls_factory,
        &m_client,
        m_zsp_factory
    ));
}

void TestTaskBase::TearDown() {
    TestBase::TearDown();

}

void TestTaskBase::initWorkspace(
        const std::vector<std::pair<std::string,std::string>>   &files) {
    createTree(files);

    std::vector<std::string> roots;

    // for (std::vector<std::pair<std::string,std::string>>::const_iterator
    //     it=files.begin();
    //     it!=files.end(); it++) {
    //     roots.push_back("file://" + m_testdir + "/" + it->first);
    // }
    roots.push_back(m_testdir);

    jrpc::ITask *n = TaskWorkspaceStartup(m_ctxt.get(), roots).run(0, true);

    /**
    ASSERT_TRUE(n && !n->done());

    ASSERT_FALSE(runTasks(100));
     */

}

bool TestTaskBase::runTasks(int32_t max) {
    bool pend;
    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < max; i++) { }

    return pend;
}

}
}
