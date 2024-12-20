/**
 * TaskRefreshRootSymtab.h
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
#include "jrpc/ITask.h"
#include "jrpc/impl/TaskBase.h"
#include "jrpc/impl/LockRwValid.h"
#include "lls/IClient.h"
#include "lls/IFactory.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/IAstBuilder.h"
#include "zsp/parser/IMarkerListener.h"
#include "Context.h"
#include "SourceFileCollection.h"
#include "SourceFileData.h"

namespace zsp {
namespace ls {


/**
 * @brief Parses and updates a single file in the source collection
 */
class TaskRefreshRootSymtab :
    public virtual jrpc::TaskBase {
public:
    TaskRefreshRootSymtab(Context *ctxt);

    TaskRefreshRootSymtab(TaskRefreshRootSymtab *o);

    virtual ~TaskRefreshRootSymtab();

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

    virtual TaskRefreshRootSymtab *clone() override {
        return new TaskRefreshRootSymtab(this);
    }

private:
    static dmgr::IDebug                 *m_dbg;
    int32_t                             m_idx;
    Context                             *m_ctxt;
};

}
}


