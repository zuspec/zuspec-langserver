/*
 * TestOpenChange.cpp
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
#include "nlohmann/json.hpp"
#include "TestOpenChange.h"
#include "TaskDidOpen.h"
#include "TaskDidChange.h"
#include "TaskHover.h"


namespace zsp {
namespace ls {


TestOpenChange::TestOpenChange() {

}

TestOpenChange::~TestOpenChange() {

}

TEST_F(TestOpenChange, single_file_open_with_error) {
    std::vector<std::pair<std::string,std::string>> files = {
        {"file1.pss", R"(
            component C {

            }
        )"}
    };

    initWorkspace(files);

    m_client.clearDiagnostics();

    std::string open_content = R"(
            component C {
              abc;
            }
        )";

    std::string uri = "file://" + m_testdir + "/file1.pss";
    TaskDidOpen(
        m_ctxt.get(),
        uri,
        open_content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 1);

    m_client.clearDiagnostics();

    std::string change_content = R"(
            component C {

            }
        )";

    TaskDidChange(
        m_ctxt.get(),
        uri,
        change_content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 0);
}

TEST_F(TestOpenChange, single_file_hover_change_hover) {
    std::string content = R"(
            component C {
                action A {

                }
            }
        )";
    std::vector<std::pair<std::string,std::string>> files = {
        {"file1.pss", content}
    };

    enableDebug(true);

    initWorkspace(files);

    m_client.clearAll();

    // Open the file
    std::string uri = "file://" + m_testdir + "/file1.pss";
    TaskDidOpen(
        m_ctxt.get(),
        uri,
        content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 0);
    m_client.clearAll();

    TextUtil util(content);
    util.find("A");

    // Hover, and confirm that this works on initial file
    TaskHover(
        "hover-id",
        m_ctxt.get(),
        "file://" + m_testdir + "/file1.pss",
        util.lineno(),
        util.linepos()).run(0, true);
    
    ASSERT_EQ(m_client.getResponses().size(), 1);
    lls::IJson *rsp = m_client.getResponses().at(0).get();
    nlohmann::json jrsp = rsp->toJson();

    fprintf(stdout, "jrsp: %s", jrsp.dump().c_str());
    std::string value = jrsp["contents"]["value"];

    ASSERT_NE(value.find("action A"), -1);

    // Just append to the end
    std::string change_content = content + R"(

            component D {

            }
        )";

    TaskDidChange(
        m_ctxt.get(),
        uri,
        change_content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 0);
    m_client.clearAll();
    
    TaskHover(
        "hover-id",
        m_ctxt.get(),
        "file://" + m_testdir + "/file1.pss",
        util.lineno(),
        util.linepos()).run(0, true);
    ASSERT_EQ(m_client.getResponses().size(), 1);
    rsp = m_client.getResponses().at(0).get();
    jrsp = rsp->toJson();

    fprintf(stdout, "jrsp: %s", jrsp.dump().c_str());
    value = jrsp["contents"]["value"];

    ASSERT_NE(value.find("action A"), -1);

    util = TextUtil(change_content);
    util.find("D");

    m_client.clearAll();
    
    TaskHover(
        "hover-id",
        m_ctxt.get(),
        "file://" + m_testdir + "/file1.pss",
        util.lineno(),
        util.linepos()).run(0, true);

    ASSERT_EQ(m_client.getResponses().size(), 1);
    rsp = m_client.getResponses().at(0).get();
    jrsp = rsp->toJson();

    fprintf(stdout, "jrsp: %s", jrsp.dump().c_str());
    value = jrsp["contents"]["value"];

    ASSERT_NE(value.find("component D"), -1);
}

TEST_F(TestOpenChange, single_file_change_link_error) {
    std::vector<std::pair<std::string,std::string>> files = {
        {"file1.pss", R"(
            component C {

            }
        )"}
    };

    enableDebug(true);
    initWorkspace(files);

    m_client.clearDiagnostics();

    std::string open_content = R"(
            component C {

            }
        )";

    std::string uri = "file://" + m_testdir + "/file1.pss";
    TaskDidOpen(
        m_ctxt.get(),
        uri,
        open_content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 0);

    m_client.clearDiagnostics();

    std::string change_content = R"(
            component C {
                MyComponent     d;
            }
        )";

    TaskDidChange(
        m_ctxt.get(),
        uri,
        change_content).run(0, true);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);
    ASSERT_EQ(m_client.getDiagnostics().at(0)->getDiagnostics().size(), 1);
}

}
}
