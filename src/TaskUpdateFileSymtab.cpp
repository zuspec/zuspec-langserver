/*
 * TaskUpdateFileSymtab.cpp
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
#include "jrpc/impl/TaskLockWrite.h"
#include "TaskUpdateFileSymtab.h"
#include "TaskLinkAst.h"


namespace zsp {
namespace ls {


TaskUpdateFileSymtab::TaskUpdateFileSymtab(
    Context             *ctxt,
    const std::string   &uri) : TaskBase(ctxt->getQueue()),
        m_ctxt(ctxt), m_file(0), m_uri(uri), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskUpdateFileSymtab", ctxt->getDebugMgr());
}

TaskUpdateFileSymtab::~TaskUpdateFileSymtab() {

}

jrpc::ITask *TaskUpdateFileSymtab::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx = 1;
            /*
            if (!jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true)->done()) {
                break;
            }
             */
        }
        case 1: {
            m_idx = 2;
            m_file = m_ctxt->getSourceFiles()->getFile(m_uri);
            // Build a list of sources that are not this file and do not have errors
            /*
            std::vector<ast::IGlobalScope *> files;
            files.push_back(m_file->getLiveAst());

            for (std::vector<SourceFileDataUP>::const_iterator
                it=m_ctxt->getSourceFiles()->getFiles().begin();
                it!=m_ctxt->getSourceFiles()->getFiles().end(); it++) {
                if (it->get() != m_file) {
                    files.push_back(it->get()->getStaticAst());
                }
            }
             */

            // Run the linker
            m_file->clearMarkers(true); // Maybe be more selective?
            parser::ILinkerUP linker(m_ctxt->getParserFactory()->mkAstLinker());

            ast::IRootSymbolScopeUP root(linker->linkOverlay(
                this, 
                m_ctxt->getSourceFiles()->getRoot(),
                m_file->getLiveAst()));

            m_file->setFileSymtab(root);
        }

        case 2: {
            // Retrieve the 
            setFlags(jrpc::TaskFlags::Complete);
//            m_ctxt->getSourceFiles()->getLock()->unlock_write();
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

void TaskUpdateFileSymtab::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker");
    // Only save messages specific to this file
    // if (m->loc().fileid == m_file->getId()) {
    //     zsp::parser::IMarkerUP mc(m->clone());
    //     m_file->addLinkMarker(mc, true);        
    // } else {
    //     DEBUG("Not from the target file");
    // }
    DEBUG_LEAVE("marker");
}

bool TaskUpdateFileSymtab::hasSeverity(zsp::parser::MarkerSeverityE s) {

}

dmgr::IDebug *TaskUpdateFileSymtab::m_dbg = 0;

}
}
