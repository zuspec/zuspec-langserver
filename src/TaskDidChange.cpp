/*
 * TaskDidChange.cpp
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
#include "TaskDidChange.h"
#include "TaskPublishDiagnostics.h"
#include "TaskUpdateSourceFileData.h"
#include "TaskUpdateFileSymtab.h"


namespace zsp {
namespace ls {


TaskDidChange::TaskDidChange(
    Context             *ctxt,
    const std::string   &uri,
    const std::string   &content) : TaskBase(ctxt->getQueue()),
        m_ctxt(ctxt), m_file(0), m_uri(uri), m_content(content), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskDidChange", ctxt->getDebugMgr());
}

TaskDidChange::~TaskDidChange() {

}

jrpc::ITask *TaskDidChange::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run idx=%d", m_idx);
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx++;

            jrpc::ITask *n = jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true);

            if (n && !n->done()) {
                break;
            }
        }

        case 1: {
            m_idx++;

            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                m_file = m_ctxt->getSourceFiles()->getFile(m_uri);

                m_file->setLiveContent(m_content);
                DEBUG("File already found");

                // Update the file data with 'live' data
                jrpc::ITask *n = TaskUpdateSourceFileData(m_ctxt, m_file, true).run(this, true);

                if (n && !n->done()) {
                    break;
                }
            }
        }

        case 2: {
            m_idx++;
            
            // If the live file doesn't have syntax errors, proceed to build
            // the file view of the symbol table
            DEBUG("UpdateFileSymtab");
            jrpc::ITask *n = TaskUpdateFileSymtab(m_ctxt, m_uri, false).run(this, true);
                
            if (n && !n->done()) {
                break;
            }
        }

        case 3: {
            m_idx++;
            if (m_file) {
                jrpc::ITask *n = TaskPublishDiagnostics(m_ctxt, m_file, true).run(this, true);
                
                if (n && !n->done()) {
                    break;
                }
            }
        }

        case 4: {
            m_ctxt->getSourceFiles()->getLock()->unlock_write();
            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run %d", m_idx);
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskDidChange::m_dbg = 0;

}
}
