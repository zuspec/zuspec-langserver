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
#include "TaskDidChange.h"
#include "TaskDidOpen.h"


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

    // Confirm that some diagnostics were sent
    ASSERT_EQ(m_client.getDiagnostics().size(), 2);

    uint32_t n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);

    // Check post-conditions
    // - Files discovered
    // - Index state


}

TEST_F(TestWorkspaceStartup, didOpen) {
    std::vector<std::pair<std::string, std::string>> files = {
        {"file1.pss", R"(
            component C1 {

            }
        )"},
        {"file2.pss", R"(
            component C2 {

            }
        )"},
    };
    createTree(files);

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

    for (std::vector<SourceFileDataUP>::const_iterator
        it=m_ctxt->getSourceFiles()->getFiles().begin();
        it!=m_ctxt->getSourceFiles()->getFiles().end(); it++) {
        ASSERT_TRUE((*it)->getStaticAst());
    }

    // Confirm that some diagnostics were sent
    ASSERT_EQ(m_client.getDiagnostics().size(), 2);

    uint32_t n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);

    m_client.clearDiagnostics();

    std::string path = "file://" + m_testdir + "/file2.pss";
    TaskDidOpen(m_ctxt.get(), path, R"(
            component C2 {
                
            }
        )").run(0, true);

    // Check post-conditions
    // - Files discovered
    // - Index state


    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < 4; i++) {
        ;
    }

    ASSERT_FALSE(pend);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);

    n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);

    SourceFileData *file = m_ctxt->getSourceFiles()->getFile(path);
    ASSERT_TRUE(file);

    // No symtab, since there are errors
    ASSERT_TRUE(file->getFileSymtab());
}

TEST_F(TestWorkspaceStartup, didOpenErr) {
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

    // Confirm that some diagnostics were sent
    ASSERT_EQ(m_client.getDiagnostics().size(), 2);

    uint32_t n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);

    m_client.clearDiagnostics();

    std::string path = "file://" + m_testdir + "/file2.pss";
    TaskDidOpen(m_ctxt.get(), path, R"(
            component C2 {
                a; // syntax error
            }
        )").run(0, true);

    // Check post-conditions
    // - Files discovered
    // - Index state


    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < 4; i++) {
        ;
    }

    ASSERT_FALSE(pend);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);

    n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_NE(n_diagnostics, 0);

    SourceFileData *file = m_ctxt->getSourceFiles()->getFile(path);
    ASSERT_TRUE(file);

    // No symtab, since there are errors
    ASSERT_FALSE(file->getFileSymtab());
}

TEST_F(TestWorkspaceStartup, didChange) {
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

    // Confirm that some diagnostics were sent
    ASSERT_EQ(m_client.getDiagnostics().size(), 2);

    uint32_t n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);

    m_client.clearDiagnostics();

    std::string path = "file://" + m_testdir + "/file2.pss";
    TaskDidOpen(m_ctxt.get(), path, R"(
            component C2 {
                a; // syntax error
            }
        )").run(0, true);

    // Check post-conditions
    // - Files discovered
    // - Index state


    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < 4; i++) {
        ;
    }

    ASSERT_FALSE(pend);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);

    n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_NE(n_diagnostics, 0);

    m_client.clearDiagnostics();

    path = "file://" + m_testdir + "/file2.pss";
    TaskDidChange(m_ctxt.get(), path, R"(
            component C2 {
                
            }
        )").run(0, true);

    // Check post-conditions
    // - Files discovered
    // - Index state


    for (uint32_t i=0; (pend=m_queue->runOneTask()) && i < 4; i++) {
        ;
    }

    ASSERT_FALSE(pend);

    ASSERT_EQ(m_client.getDiagnostics().size(), 1);

    n_diagnostics = 0;
    for (std::vector<lls::IPublishDiagnosticsParamsUP>::const_iterator
        it=m_client.getDiagnostics().begin();
        it!=m_client.getDiagnostics().end(); it++) {
        n_diagnostics += (*it)->getDiagnostics().size();
    }
    ASSERT_EQ(n_diagnostics, 0);
}

}
}
