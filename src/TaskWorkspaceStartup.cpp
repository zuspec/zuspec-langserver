/*
 * TaskWorkspaceStartup.cpp
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
#include "jrpc/impl/TaskLambda.h"
#include "jrpc/impl/TaskRunTasklist.h"
#include "Context.h"
#include "TaskFindSourceFiles.h"
#include "TaskWorkspaceStartup.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskWorkspaceStartup::TaskWorkspaceStartup(
    Context                             *ctxt,
    const std::vector<std::string>      &roots) : 
        TaskBase(ctxt->getQueue()), m_idx(0), m_ctxt(ctxt),
        m_roots(roots.begin(), roots.end()) {
    DEBUG_INIT("zsp::ls::TaskWorkspaceStartup", ctxt->getDebugMgr());
}

TaskWorkspaceStartup::~TaskWorkspaceStartup() {

}

jrpc::ITask *TaskWorkspaceStartup::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            DEBUG("Locking the source collection");
            // First, lock the source collection so we can update the filelist
            m_idx = 1;
            if (!jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true)->done()) {
                break;
            }
        }
        case 1: { // We now hold a lock to the source collection. 
            DEBUG("Finding source files");
            m_idx = 2;
            if (!TaskFindSourceFiles(m_ctxt->getDebugMgr(), m_queue, m_roots).run(this, 1)->done()) {
                break;
            }
        }
        case 2: {  // Have source files. Now, need to update source files
            m_idx = 3;
            DEBUG("Parse source files");
            std::vector<std::string> *files = TaskFindSourceFiles::getResult(getResult());

            // Start and queue individual jobs to process files
            std::vector<jrpc::ITask *> tasks;
            for (std::vector<std::string>::const_iterator
                f_it=files->begin();
                f_it!=files->end(); f_it++) {
                if (!m_ctxt->getSourceFiles()->hasFile(*f_it)) {
                    SourceFileDataUP file(new SourceFileData(*f_it, 0));
                    m_ctxt->getSourceFiles()->addFile(file);
                }

                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(*f_it);
                tasks.push_back(new jrpc::TaskLambda(m_ctxt->getQueue(),
                    [this,file](jrpc::ITask *p, bool i) -> jrpc::ITask * {
                        return TaskUpdateSourceFileData(m_ctxt, file).run(0, true);
                    }));
            }

            // Wait for all jobs to complete
            DEBUG("-- Starting %d tasks", tasks.size());
            if (!jrpc::TaskRunTasklist(m_ctxt->getQueue(), tasks).run(this, true)->done()) {
                DEBUG("-- Jobs incomplete");
                break;
            }
        }

        case 3: { // If no parse errors, create a linked representation

        }

        case 4: { // Report any errors

        }

        case 5: {
            // All done. Mark the collection valid and release the write lock
            m_ctxt->getSourceFiles()->getLock()->unlock_write_valid(true);
            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskWorkspaceStartup::m_dbg = 0;

}
}
