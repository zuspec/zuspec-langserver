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
#include "dmgr/impl/DebugMacros.h"
#include "nlohmann/json.hpp"
#include "TestTaskDeclaration.h"
#include "TaskDeclaration.h"
#include "TaskDidChange.h"
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

    enableDebug(false);

    initWorkspace(files);

    TextUtil doc(files.at(0).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("C"), -1);

    ASSERT_EQ(doc.lineno(), 6);
    ASSERT_EQ(doc.linepos(), 8);

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

    const std::vector<lls::IJsonUP> &rsps = m_client.getResponses();
    ASSERT_EQ(rsps.size(), 1);

    nlohmann::json rsp = rsps.at(0)->toJson();
    DEBUG("rsp: %s", rsp.dump().c_str());
    ASSERT_TRUE(rsp.find("range") != rsp.end());

}

TEST_F(TestTaskDeclaration, field_ref) {
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

    enableDebug(false);

    initWorkspace(files);

    TextUtil doc(files.at(0).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("c1"), -1);

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

    const std::vector<lls::IJsonUP> &rsps = m_client.getResponses();
    ASSERT_EQ(rsps.size(), 1);

    nlohmann::json rsp = rsps.at(0)->toJson();
    DEBUG("rsp: %s", rsp.dump().c_str());
    ASSERT_TRUE(rsp.find("range") != rsp.end());

}

TEST_F(TestTaskDeclaration, field_ref_cross_file) {
    std::vector<std::pair<std::string, std::string>> files = {
    {"test1.pss", R"(
    component C {

    }
    )"},
    {"test2.pss", R"(
    component pss_top {
        C           c1;
    }
    )"}
    };

    enableDebug(false);

    initWorkspace(files);

    TextUtil doc(files.at(1).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("c1"), -1);

    std::string uri = "file://" + m_testdir + "/test2.pss";

    jrpc::ITask *n;

    n = TaskDidOpen(
        m_ctxt.get(), 
        uri,
        files.at(1).second).run(0, true);

    ASSERT_FALSE(runTasks(100));

    n = TaskDeclaration(
        m_ctxt.get(),
        "msg-id",
        uri,
        doc.lineno(),
        doc.linepos()).run(0, true);

    ASSERT_TRUE((!n || n->done()));

    const std::vector<lls::IJsonUP> &rsps = m_client.getResponses();
    ASSERT_EQ(rsps.size(), 1);

    nlohmann::json rsp = rsps.at(0)->toJson();
    DEBUG("rsp: %s", rsp.dump().c_str());
    ASSERT_TRUE(rsp.find("range") != rsp.end());

}

TEST_F(TestTaskDeclaration, field_ref_cross_file_link_error) {
    std::vector<std::pair<std::string, std::string>> files = {
    {"test1.pss", R"(
    component C {

    }
    )"},
    {"test2.pss", R"(
    component pss_top {
        C               c1;
        MissingCompT    c2;
    }
    )"}
    };

    enableDebug(false);

    initWorkspace(files);

    TextUtil doc(files.at(1).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("c1"), -1);

    std::string uri = "file://" + m_testdir + "/test2.pss";

    jrpc::ITask *n;

    n = TaskDidOpen(
        m_ctxt.get(), 
        uri,
        files.at(1).second).run(0, true);

    ASSERT_FALSE(runTasks(100));

    n = TaskDeclaration(
        m_ctxt.get(),
        "msg-id",
        uri,
        doc.lineno(),
        doc.linepos()).run(0, true);

    ASSERT_TRUE((!n || n->done()));

    const std::vector<lls::IJsonUP> &rsps = m_client.getResponses();
    ASSERT_EQ(rsps.size(), 1);

    nlohmann::json rsp = rsps.at(0)->toJson();
    DEBUG("rsp: %s", rsp.dump().c_str());
    ASSERT_TRUE(rsp.find("range") != rsp.end());
}

TEST_F(TestTaskDeclaration, field_ref_cross_file_add_link_error) {
    std::vector<std::pair<std::string, std::string>> files = {
    {"test1.pss", R"(
    component C {

    }
    )"},
    {"test2.pss", R"(
    component pss_top {
        C               c1;
    }
    )"}
    };

    enableDebug(false);

    initWorkspace(files);

    TextUtil doc(files.at(1).second);
    ASSERT_NE(doc.find("pss_top"), -1);
    ASSERT_NE(doc.find("c1"), -1);

    std::string uri = "file://" + m_testdir + "/test2.pss";

    jrpc::ITask *n;

    n = TaskDidOpen(
        m_ctxt.get(), 
        uri,
        files.at(1).second).run(0, true);

    ASSERT_FALSE(runTasks(100));

    std::string file2_p = R"(
    component pss_top {
        C               c1;
        MissingCompT    c2;
    }
    )";

    n = TaskDidChange(
        m_ctxt.get(),
        uri,
        file2_p).run(0, true);

    n = TaskDeclaration(
        m_ctxt.get(),
        "msg-id",
        uri,
        doc.lineno(),
        doc.linepos()).run(0, true);

    ASSERT_TRUE((!n || n->done()));

    const std::vector<lls::IJsonUP> &rsps = m_client.getResponses();
    ASSERT_EQ(rsps.size(), 1);

    nlohmann::json rsp = rsps.at(0)->toJson();
    DEBUG("rsp: %s", rsp.dump().c_str());
    ASSERT_TRUE(rsp.find("range") != rsp.end());
}

}
}
