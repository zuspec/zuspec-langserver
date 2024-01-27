/**
 * TaskBuildDocumentSymbols.h
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
#include "lls/IFactory.h"
#include "zsp/ast/impl/VisitorBase.h"

namespace zsp {
namespace ls {



class TaskBuildDocumentSymbols : 
    public virtual zsp::ast::VisitorBase {
public:
    TaskBuildDocumentSymbols(
        lls::IFactory           *factory
    );

    virtual ~TaskBuildDocumentSymbols();

    lls::IDocumentSymbolResponseUP build(
        zsp::ast::IGlobalScope      *scope
    );

    virtual void visitAction(zsp::ast::IAction *i) override;

    virtual void visitComponent(zsp::ast::IComponent *i) override;

    virtual void visitField(zsp::ast::IField *i) override;

    virtual void visitFunctionDefinition(zsp::ast::IFunctionDefinition *i) override;

    virtual void visitPackageScope(zsp::ast::IPackageScope *i) override;

    virtual void visitStruct(zsp::ast::IStruct *i) override;

protected:

    std::pair<lls::IRangeUP, lls::IRangeUP> getScopeRange(zsp::ast::INamedScope *s);

    std::pair<lls::IRangeUP, lls::IRangeUP> getScopeRange(zsp::ast::IPackageScope *s);

    std::pair<lls::IRangeUP, lls::IRangeUP> getItemRange(zsp::ast::INamedScopeChild *s);

    std::pair<lls::IRangeUP, lls::IRangeUP> getItemRange(zsp::ast::IFunctionDefinition *s);

private:
    static dmgr::IDebug                     *m_dbg;
    std::vector<lls::IDocumentSymbol *>     m_sym_s;
    lls::IFactory                           *m_factory;

};

}
}


