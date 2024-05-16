/*
 * TaskDidClose.cpp
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
#include "TaskDidClose.h"


namespace zsp {
namespace ls {


TaskDidClose::TaskDidClose(
    Context             *ctxt,
    const std::string   &uri) : TaskBase(ctxt->getQueue()),
    m_ctxt(ctxt), m_uri(uri), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskDidClose", ctxt->getDebugMgr());
}

TaskDidClose::~TaskDidClose() {

}

jrpc::ITask *TaskDidClose::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx = 1;

            if (jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(0, true)->done()) {
                setFlags(jrpc::TaskFlags::Yield);
            }
            break;
        }

        case 1: {
            m_idx = 2;
            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);

                // Clean up live-view data
                file->closeLiveView();
            }
        }

        case 2: {
            bool launch_refresh = m_ctxt->getSourceFiles()->hasFile(m_uri);

            // Identify other open files.
            // Invalidate their file symtabs (condition?) because our content has been committed
            // 

            m_ctxt->getSourceFiles()->getLock()->unlock_write();

            if (launch_refresh) {

            }

            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskDidClose::m_dbg = 0;

}
}
