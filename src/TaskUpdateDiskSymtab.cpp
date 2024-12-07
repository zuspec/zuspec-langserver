/*
 * TaskUpdateDiskSymtab.cpp
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
#include "jrpc/impl/TaskLockRead.h"
#include "jrpc/impl/TaskLockWrite.h"
#include "zsp/parser/impl/TaskClearLinks.h"
#include "TaskLinkAst.h"
#include "TaskUpdateDiskSymtab.h"
#include "TaskPublishDiagnostics.h"


namespace zsp {
namespace ls {


TaskUpdateDiskSymtab::TaskUpdateDiskSymtab(Context *ctxt, bool lock) :
    TaskBase(ctxt->getQueue()), m_ctxt(ctxt), m_lock(lock), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskUpdateDiskSymtab", ctxt->getDebugMgr());
}

TaskUpdateDiskSymtab::~TaskUpdateDiskSymtab() {

}

jrpc::ITask *TaskUpdateDiskSymtab::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx++;

            if (m_lock) {
                // Need to acquire lock to the source collection to prevent
                // changes to the filelist
                // Yield this task after startup
                jrpc::ITask *n = jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true);

                if (n && !n->done()) {
                    DEBUG("Waiting for SourceCollection Read lock");
                    break;
                }
            }
        }

        case 1: {
            // Need to acquire a write lock to the root symbol table
            // since we're about to update it
            m_idx++;

            jrpc::ITask *n = jrpc::TaskLockWrite(m_queue, &m_ctxt->getSourceFiles()->getRoot()).run(this, true);

            if (n && !n->done()) {
                DEBUG("Waiting for Root Symbol Table Write lock");
                break;
            }
        }

        case 2: {
            m_idx++;

            // Now, we can link the files
            std::vector<ast::IGlobalScope *> files;

            zsp::parser::ILinkerUP linker(m_ctxt->getParserFactory()->mkAstLinker());

            for (std::vector<SourceFileDataUP>::const_iterator
                it=m_ctxt->getSourceFiles()->getFiles().begin();
                it!=m_ctxt->getSourceFiles()->getFiles().end(); it++) {
                files.push_back((*it)->getDiskAst());

                // Clear links within the file
                zsp::parser::TaskClearLinks(m_ctxt->getDebugMgr()).clear((*it)->getDiskAst());

                // Clear link markers on each file
                (*it)->clearLinkMarkers(false);
            }

            jrpc::ITask *n = TaskLinkAst(m_ctxt, files, false).run(this, true);

            if (n && !n->done()) {
                DEBUG("Waiting for AST Linking to complete");
                break;
            }
        }

        case 3: {
            m_idx++;

            // Get the root symbol table from the result
            ast::IRootSymbolScope *root = TaskLinkAst::getResult(moveResult());
            if (m_ctxt->getSourceFiles()->getRoot().getData()) {
                delete m_ctxt->getSourceFiles()->getRoot().getDataT<ast::IRootSymbolScope>();
            }
            m_ctxt->getSourceFiles()->getRoot().unlock_write_data(root);

            // Finally, publish diagnostics
            for (std::vector<SourceFileDataUP>::const_iterator
                f_it=m_ctxt->getSourceFiles()->getFiles().begin();
                f_it!=m_ctxt->getSourceFiles()->getFiles().end(); f_it++) {
                DEBUG("Publishing diagnostics for %s", (*f_it)->getUri().c_str());
                TaskPublishDiagnostics(
                    m_ctxt, 
                    f_it->get(),
                    (*f_it)->isOpen()).run(0, true);
            }

            // And, release the source-collection lock (if held)
            if (m_lock) {
                // Release lock
                m_ctxt->getSourceFiles()->getLock()->unlock_write();
            }

            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskUpdateDiskSymtab::m_dbg = 0;

}
}
