/**
 * TaskUpdateSourceFileData.h
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
class TaskUpdateSourceFileData :
    public virtual zsp::parser::IMarkerListener,
    public virtual jrpc::TaskBase {
public:
    TaskUpdateSourceFileData(
        Context                     *ctxt,
        SourceFileData              *file,
        bool                        update_live);

    TaskUpdateSourceFileData(TaskUpdateSourceFileData *o);

    virtual ~TaskUpdateSourceFileData();

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

    virtual TaskUpdateSourceFileData *clone() override {
        return new TaskUpdateSourceFileData(this);
    }

	virtual void marker(const zsp::parser::IMarker *m) override;

	virtual bool hasSeverity(zsp::parser::MarkerSeverityE s) override;

private:
    static dmgr::IDebug                 *m_dbg;
    Context                             *m_ctxt;
    SourceFileData                      *m_file;
    bool                                m_update_live;
    std::vector<lls::IDiagnosticUP>     m_diagnostics;
};

}
}


