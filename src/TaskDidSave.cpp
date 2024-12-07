/*
 * TaskDidSave.cpp
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
#include "TaskDidSave.h"
#include "TaskRefreshRootSymtab.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskDidSave::TaskDidSave(
    Context             *ctxt,
    const std::string   &uri) : TaskBase(ctxt->getQueue()),
    m_ctxt(ctxt), m_uri(uri), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskDidSave", ctxt->getDebugMgr());
}

TaskDidSave::~TaskDidSave() {

}

jrpc::ITask *TaskDidSave::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run %d", m_idx);
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx++;

            jrpc::ITask *n = jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(0, true);

            if (n && !n->done()) {
                DEBUG("Waiting for SourceCollection Write lock");
                break;
            }
        }
        case 1: {
            m_idx++;
            DEBUG("Have SourceCollection Write lock");

            jrpc::ITask *n = TaskUpdateSourceFileData(
                m_ctxt, 
                m_ctxt->getSourceFiles()->getFile(m_uri), 
                false // update 'disk' view
            ).run(this, true);

            if (n && !n->done()) {
                break;
            }
        }
        case 2: {
            // Invalidate root index
            TaskRefreshRootSymtab(m_ctxt).run(this, true);

            m_ctxt->getSourceFiles()->getLock()->unlock_write();

            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskDidSave::m_dbg = 0;

}
}
