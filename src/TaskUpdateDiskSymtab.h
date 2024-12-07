/**
 * TaskUpdateDiskSymtab.h
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
#include "jrpc/impl/TaskBase.h"
#include "Context.h"

namespace zsp {
namespace ls {



class TaskUpdateDiskSymtab :
    public virtual jrpc::TaskBase {
public:
    TaskUpdateDiskSymtab(Context *ctxt, bool lock);

    TaskUpdateDiskSymtab(TaskUpdateDiskSymtab *o) : TaskBase(o),
        m_ctxt(o->m_ctxt), m_lock(o->m_lock), m_idx(o->m_idx) { }

    virtual ~TaskUpdateDiskSymtab();

    virtual TaskUpdateDiskSymtab *clone() override {
        return new TaskUpdateDiskSymtab(this);
    }

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

private:
    static dmgr::IDebug             *m_dbg;
    Context                         *m_ctxt;
    bool                            m_lock;
    int32_t                         m_idx;
};

}
}


