/**
 * TaskDidOpen.h
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
#include "dmgr/IDebugMgr.h"
#include "jrpc/impl/TaskBase.h"
#include "Context.h"

namespace zsp {
namespace ls {



class TaskDidOpen : public jrpc::TaskBase {
public:
    TaskDidOpen(
        Context                 *ctxt,
        const std::string       &uri,
        const std::string       &live_txt);

    TaskDidOpen(TaskDidOpen *o) : TaskBase(o),
        m_ctxt(o->m_ctxt), m_uri(o->m_uri), m_file(o->m_file),
        m_live_txt(o->m_live_txt), m_idx(o->m_idx) { }

    virtual ~TaskDidOpen();

    virtual TaskDidOpen *clone() override { 
        return new TaskDidOpen(this); 
    }

    virtual ITask *run(ITask *parent, bool initial) override;

private:
    static dmgr::IDebug     *m_dbg;
    Context                 *m_ctxt;
    std::string             m_uri;
    SourceFileData          *m_file;
    std::string             m_live_txt;
    int32_t                 m_idx;

};

}
}


