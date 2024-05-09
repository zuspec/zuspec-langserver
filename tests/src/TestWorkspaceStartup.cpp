/*
 * TestWorkspaceStartup.cpp
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
#include <string>
#include <vector>
#include "TestWorkspaceStartup.h"
#include "TaskWorkspaceStartup.h"


namespace zsp {
namespace ls {


TestWorkspaceStartup::TestWorkspaceStartup() {

}

TestWorkspaceStartup::~TestWorkspaceStartup() {

}

TEST_F(TestWorkspaceStartup, smoke) {
    createTree({
        {"file1.pss", R"(
            component C1 {

            }
        )"},
        {"file2.pss", R"(
            component C2 {

            }
        )"},
    });

    // Need a test run directory
    enableDebug(true);
    m_rm_testdir = false;

    // Need a source of data to copy to the run directory

    // Need to create some standard infrastructure to mock out the
    // language server
    // - TaskQueue
    // - SourceFileCollection

    std::vector<std::string> roots;

    roots.push_back(m_testdir);

    // Execute TaskWorkspaceStartup
    TaskWorkspaceStartup task(m_ctxt.get(), roots);
    jrpc::ITask *ret = task.run(0, true);

    // For now, assume no blocking
    ASSERT_FALSE(ret->done());

    bool pend;
    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < 4; i++) {
        ;
    }

    ASSERT_FALSE(pend);

    // Check post-conditions
    // - Files discovered
    // - Index state


}

}
}
