/*
 * TestTaskDeclaration.cpp
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
#include "TestTaskDeclaration.h"
#include "TaskDeclaration.h"
#include "TaskDidOpen.h"


namespace zsp {
namespace ls {


TestTaskDeclaration::TestTaskDeclaration() {

}

TestTaskDeclaration::~TestTaskDeclaration() {

}

TEST_F(TestTaskDeclaration, smoke) {
    std::vector<std::pair<std::string, std::string>> files = {
        {"test1.pss", R"(
    component C {

    }

    component pss_top {
        C           c1;
    }
        )"
        }
    };

    enableDebug(true);

    initWorkspace(files);

    TextUtil doc(files.at(0).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("C"), -1);

    ASSERT_EQ(doc.lineno(), 7);
    ASSERT_EQ(doc.linepos(), 9);

    std::string uri = "file://" + m_testdir + "/test1.pss";

    jrpc::ITask *n;

    n = TaskDidOpen(
        m_ctxt.get(), 
        uri,
        files.at(0).second).run(0, true);

    ASSERT_FALSE(runTasks(100));

    n = TaskDeclaration(
        m_ctxt.get(),
        "msg-id",
        uri,
        doc.lineno(),
        doc.linepos()).run(0, true);

    ASSERT_TRUE((!n || n->done()));

}

}
}
