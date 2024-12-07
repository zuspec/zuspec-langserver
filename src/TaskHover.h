/**
 * TaskHover.h
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
#include "zsp/ast/impl/VisitorBase.h"
#include "Context.h"

namespace zsp {
namespace ls {


/**
 * @brief Compute hover results
 * 
 */
class TaskHover : 
    public virtual jrpc::TaskBase,
    public virtual ast::VisitorBase {
public:
    TaskHover(
        const std::string   &id,
        Context             *ctxt,
        const std::string   &uri,
        int32_t             lineno,
        int32_t             linepos
    );

    TaskHover(TaskHover *o) : TaskBase(o),
        m_id(o->m_id), m_ctxt(o->m_ctxt), m_idx(o->m_idx), m_uri(o->m_uri),
        m_symtab(o->m_symtab), m_lineno(o->m_lineno), m_linepos(o->m_linepos) { }

    virtual ~TaskHover();

    virtual TaskHover *clone() override {
        return new TaskHover(this);
    }

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

    virtual void visitAction(ast::IAction *i) override;

    virtual void visitComponent(ast::IComponent *i) override;

    virtual void visitField(ast::IField *i) override;

    virtual void visitStruct(ast::IStruct *i) override;

    virtual void visitDataTypeUserDefined(ast::IDataTypeUserDefined *i) override;

    virtual void visitSymbolTypeScope(ast::ISymbolTypeScope *i) override;

private:
    void addDocComment(ast::IScopeChild *i);

private:
    static dmgr::IDebug             *m_dbg;
    std::string                     m_id;
    Context                         *m_ctxt;
    int32_t                         m_idx;
    std::string                     m_uri;
    ast::ISymbolScope               *m_symtab;
    int32_t                         m_lineno;
    int32_t                         m_linepos;
    std::string                     m_result;
};

}
}


