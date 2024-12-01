/**
 * TaskLinkAst.h
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
#include "Context.h"
#include "jrpc/impl/TaskBase.h"
#include "zsp/parser/IMarkerListener.h"


namespace zsp {
namespace ls {


class TaskLinkAst : 
    public virtual zsp::parser::IMarkerListener,
    public virtual jrpc::TaskBase {
public:
    TaskLinkAst(
        Context                                 *ctxt,
        const std::vector<ast::IGlobalScope *>  &files,
        bool                                    own_files);

    TaskLinkAst(TaskLinkAst *o) : TaskBase(o),
        m_ctxt(o->m_ctxt), m_files(o->m_files.begin(), o->m_files.end()) { }

    virtual ~TaskLinkAst();

    virtual TaskLinkAst *clone() override {
        return new TaskLinkAst(this);
    }

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

	virtual void marker(const zsp::parser::IMarker *m) override;

	virtual bool hasSeverity(zsp::parser::MarkerSeverityE s) override;

    static ast::IRootSymbolScope *getResult(const jrpc::TaskResult &result) {
        return (result.val.p)?reinterpret_cast<ast::IRootSymbolScope *>(result.val.p):0;
    }

private:
    static dmgr::IDebug                 *m_dbg;
    Context                             *m_ctxt;
    std::vector<ast::IGlobalScope *>    m_files;

};

}
}

