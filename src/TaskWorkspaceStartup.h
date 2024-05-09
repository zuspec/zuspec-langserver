/**
 * TaskWorkspaceStartup.h
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
#pragma once
#include <vector>
#include <string>
#include "jrpc/impl/TaskBase.h"
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {



class TaskWorkspaceStartup : public jrpc::TaskBase {
public:
    TaskWorkspaceStartup(
        Context                             *ctxt,
        const std::vector<std::string>      &roots);

    virtual ~TaskWorkspaceStartup();

    TaskWorkspaceStartup(TaskWorkspaceStartup *o) : 
        jrpc::TaskBase(o), m_idx(o->m_idx), m_ctxt(o->m_ctxt),
        m_roots(o->m_roots.begin(), o->m_roots.end()) { }

    virtual ITask *clone() { return new TaskWorkspaceStartup(this); }

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

private:
    static dmgr::IDebug                     *m_dbg;
    Context                                 *m_ctxt;
    int32_t                                 m_idx;
    std::vector<std::string>                m_roots;

};

}
}


