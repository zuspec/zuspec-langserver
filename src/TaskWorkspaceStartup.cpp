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
#include "TaskLinkAst.h"
#include "TaskPublishDiagnostics.h"
#include "TaskWorkspaceStartup.h"
#include "TaskUpdateSourceFileData.h"
#include "TaskUpdateDiskSymtab.h"


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
    DEBUG_ENTER("run (%d)", m_idx);
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            DEBUG("Locking the source collection");
            // First, lock the source collection so we can update the filelist
            m_idx++;

            // Yield this task after startup
            jrpc::ITask *n = jrpc::TaskLockWrite(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true);

            if (n && !n->done()) {
                DEBUG("Waiting for Write lock");
                break;
            }
        }
        case 1: { // We now hold a lock to the source collection. 
            DEBUG("Finding source files");
            m_idx = 2;
            jrpc::ITask *n = TaskFindSourceFiles(m_ctxt->getDebugMgr(), m_queue, m_roots).run(this, 1);
            
            if (n && !n->done()) {
                DEBUG("Blocked on finding source files");
                break;
            }
        }
        case 2: {  // Have source files. Now, need to update source files
            m_idx = 3;
            DEBUG("Parse source files");
            m_files = std::unique_ptr<std::vector<std::string>>(TaskFindSourceFiles::getResult(getResult()));

            // Start and queue individual jobs to process files
#ifdef MULTITHREADED
            std::vector<jrpc::ITask *> tasks;
            for (std::vector<std::string>::const_iterator
                f_it=m_files->begin();
                f_it!=m_files->end(); f_it++) {
                if (!m_ctxt->getSourceFiles()->hasFile(*f_it)) {
                    SourceFileDataUP file(new SourceFileData(*f_it, 0));
                    m_ctxt->getSourceFiles()->addFile(file);
                }

                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(*f_it);
                tasks.push_back(new jrpc::TaskLambda(m_ctxt->getQueue(),
                    [this,file](jrpc::ITask *p, bool i) -> jrpc::ITask * {
                        fprintf(stdout, "Lambda starting\n"); fflush(stdout);
                        return TaskUpdateSourceFileData(m_ctxt, file, false).run(p, i);
                    }));
            }

            // Wait for all jobs to complete
            DEBUG("-- Starting %d tasks", tasks.size());
            if (!jrpc::TaskRunTasklist(m_ctxt->getQueue(), tasks).run(this, true)->done()) {
                DEBUG("-- Jobs incomplete");
                break;
            }
#else // !MULTITHREADED
            for (std::vector<std::string>::const_iterator
                f_it=m_files->begin();
                f_it!=m_files->end(); f_it++) {
                if (!m_ctxt->getSourceFiles()->hasFile(*f_it)) {
                    SourceFileDataUP file(new SourceFileData(*f_it, 0));
                    m_ctxt->getSourceFiles()->addFile(file);
                }

                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(*f_it);
                TaskUpdateSourceFileData(
                    m_ctxt, 
                    file,
                    false).run(this, true);
                m_file_asts.push_back(file->getDiskAst());
            }
#endif
        }

        case 3: { // Trust that any provided AST content is okay
            m_idx++;

            // Drop the write lock on the source collection, since we only
            // need to read files
            m_ctxt->getSourceFiles()->getLock()->unlock_write();

            jrpc::ITask *n = TaskUpdateDiskSymtab(m_ctxt, true).run(this, true);

            if (n && !n->done()) {
                break;
            }
        }

        case 4: { // Report any errors
            m_idx++;

            setFlags(jrpc::TaskFlags::Complete);
        }

        case 5: {
        }
    }

    DEBUG_LEAVE("run (%d)", m_idx);
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskWorkspaceStartup::m_dbg = 0;

}
}
