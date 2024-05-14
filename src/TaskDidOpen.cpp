/*
 * TaskDidOpen.cpp
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
#include "TaskDidOpen.h"
#include "TaskPublishDiagnostics.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskDidOpen::TaskDidOpen(
    Context             *ctxt,
    const std::string   &uri,
    const std::string   &live_txt) : 
        TaskBase(ctxt->getQueue()), m_ctxt(ctxt), m_uri(uri), 
        m_file(0), m_live_txt(live_txt), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskDidOpen", ctxt->getDebugMgr());
}

TaskDidOpen::~TaskDidOpen() {

}

jrpc::ITask *TaskDidOpen::run(jrpc::ITask *parent, bool initial) {
    runEnter(parent, initial);
    DEBUG_ENTER("run");
    switch (m_idx) {
        case 0: {
            // First things first: lock the source collection
            m_idx = 1;
            if (jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true)->done()) {
                setFlags(jrpc::TaskFlags::Yield);
            }
            break;
        }
        case 1: {

            m_idx = 2;
            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                DEBUG("File already found");
                m_file = m_ctxt->getSourceFiles()->getFile(m_uri);
            } else {
                // This file wasn't found during discovery, so we add it to the
                // collection that we're managing now.
                // TODO: Should we mark it as somehow 'unmanaged'?
                DEBUG("File not found already");
                m_file = new SourceFileData(m_uri, -1);
                SourceFileDataUP src_up(m_file);
                m_ctxt->getSourceFiles()->addFile(src_up);
            }

            DEBUG("Set Live Content: %d", m_live_txt.size());
            m_file->setLiveContent(m_live_txt);
            DEBUG("  Set Live Content: %d", m_live_txt.size());
        }

        case 2: {
            // Update the file data
            m_idx = 3;

            if (!TaskUpdateSourceFileData(m_ctxt, m_file).run(this, true)->done()) {
                break;
            }
        }
        
        case 3: {
            m_idx = 4;
            if (!TaskPublishDiagnostics(m_ctxt, m_file).run(this, true)->done()) {
                break;
            }
        }

        case 4: {
            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskDidOpen::m_dbg = 0;

}
}
