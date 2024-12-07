/*
 * TaskRefreshRootSymtab.cpp
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
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "dmgr/impl/DebugMacros.h"
#include "jrpc/impl/TaskLockWrite.h"
#include "TaskRefreshRootSymtab.h"
#include "TaskUpdateDiskSymtab.h"


namespace zsp {
namespace ls {


TaskRefreshRootSymtab::TaskRefreshRootSymtab(Context *ctxt) : 
    TaskBase(ctxt->getQueue()), m_ctxt(ctxt), m_idx(0) {
    DEBUG_INIT("TaskRefreshRootSymtab", ctxt->getDebugMgr());
}

TaskRefreshRootSymtab::TaskRefreshRootSymtab(TaskRefreshRootSymtab *o) :
    TaskBase(o), m_ctxt(o->m_ctxt), m_idx(o->m_idx) {

}

TaskRefreshRootSymtab::~TaskRefreshRootSymtab() {

}

jrpc::ITask *TaskRefreshRootSymtab::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run %d", m_idx);
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx++;

            // Acquire a lock to the root symbol table
            jrpc::ITask *n = jrpc::TaskLockWrite(
                m_ctxt->getQueue(),
                &m_ctxt->getSourceFiles()->getRoot()).run(this, true);

            if (n && !n->done()) {
                DEBUG("Waiting for write lock on root symbol table");
                break;
            }
        }

        case 1: {
            m_idx++;

            // Dispose the current symbol table
            zsp::ast::IRootSymbolScope *root = 
                m_ctxt->getSourceFiles()->getRoot().getDataT<ast::IRootSymbolScope>();
            
            if (root) {
                delete root;
            }

            m_ctxt->getSourceFiles()->getRoot().unlock_write_data(0, [this]() {
                // Immediately suspend and wait?
                jrpc::ITask *refresh = TaskUpdateDiskSymtab(m_ctxt, true).run(0, true);
                refresh->queue();
                return refresh;
            });
        }

        case 2: {
            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskRefreshRootSymtab::m_dbg = 0;

}
}
