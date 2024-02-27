/*
 * TaskBuildDocumentSymbols.cpp
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
#include "TaskBuildDocumentSymbols.h"


namespace zsp {
namespace ls {


TaskBuildDocumentSymbols::TaskBuildDocumentSymbols(
    lls::IFactory           *factory) : m_factory(factory) {
    DEBUG_INIT("TaskBuildDocumentSymbols", factory->getDebugMgr());

}

TaskBuildDocumentSymbols::~TaskBuildDocumentSymbols() {

}

lls::IDocumentSymbolResponseUP TaskBuildDocumentSymbols::build(
        zsp::ast::IGlobalScope      *scope) {
    DEBUG_ENTER("build");
    lls::IRangeUP range, selectionRange;
    lls::IDocumentSymbolUP sym;

    sym = m_factory->mkDocumentSymbol(
        "",
        lls::SymbolKind::Null,
        range,
        selectionRange
    );
    m_sym_s.push_back(sym.get());

    scope->accept(m_this);

    lls::IDocumentSymbolResponseUP ret(m_factory->mkDocumentSymbolResponse(
        m_sym_s.back()->getChildren()
    ));

    DEBUG_LEAVE("build");
    return ret;
}

void TaskBuildDocumentSymbols::visitAction(zsp::ast::IAction *i) {
    DEBUG_ENTER("visitAction");
    std::pair<lls::IRangeUP,lls::IRangeUP> r(getScopeRange(i));

    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        i->getName()->getId(),
        lls::SymbolKind::Class,
        r.first,
        r.second
    );

    lls::IDocumentSymbol *sym_p = sym.get();
    m_sym_s.back()->addChild(sym);
    m_sym_s.push_back(sym_p);
    VisitorBase::visitAction(i);
    m_sym_s.pop_back();
    DEBUG_LEAVE("visitAction");
}

void TaskBuildDocumentSymbols::visitComponent(zsp::ast::IComponent *i) {
    DEBUG_ENTER("visitComponent");

    std::pair<lls::IRangeUP,lls::IRangeUP> r(getScopeRange(i));

    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        i->getName()->getId(),
        lls::SymbolKind::Class,
        r.first,
        r.second
    );

    lls::IDocumentSymbol *sym_p = sym.get();
    m_sym_s.back()->addChild(sym);
    m_sym_s.push_back(sym_p);
    VisitorBase::visitComponent(i);
    m_sym_s.pop_back();
    DEBUG_LEAVE("visitComponent");
}

void TaskBuildDocumentSymbols::visitField(zsp::ast::IField *i) {
    DEBUG_ENTER("visitField");
    std::pair<lls::IRangeUP,lls::IRangeUP> r(getItemRange(i));

    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        i->getName()->getId(),
        lls::SymbolKind::Field,
        r.first,
        r.second
    );

    m_sym_s.back()->addChild(sym);
    DEBUG_LEAVE("visitField");
}

void TaskBuildDocumentSymbols::visitFunctionDefinition(zsp::ast::IFunctionDefinition *i) {
    DEBUG_ENTER("visitFunctionDefinition");
    std::pair<lls::IRangeUP,lls::IRangeUP> r(getItemRange(i));

    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        i->getProto()->getName()->getId(),
        lls::SymbolKind::Function,
        r.first,
        r.second
    );

    m_sym_s.back()->addChild(sym);
    DEBUG_LEAVE("visitFunctionDefinition");
}

void TaskBuildDocumentSymbols::visitPackageScope(zsp::ast::IPackageScope *i) {
    DEBUG_ENTER("visitPackageScope");

    std::pair<lls::IRangeUP,lls::IRangeUP> r(getScopeRange(i));

    std::string name;
    for (std::vector<zsp::ast::IExprIdUP>::const_iterator
        it=i->getId().begin();
        it!=i->getId().end(); it++) {
        if (it != i->getId().begin()) {
            name += "::";
        }
        name += (*it)->getId().c_str();
    }
    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        name,
        lls::SymbolKind::Package,
        r.first,
        r.second
    );

    lls::IDocumentSymbol *sym_p = sym.get();
    m_sym_s.back()->addChild(sym);
    m_sym_s.push_back(sym_p);
    VisitorBase::visitPackageScope(i);
    m_sym_s.pop_back();
    DEBUG_LEAVE("visitPackageScope");
}

void TaskBuildDocumentSymbols::visitStruct(zsp::ast::IStruct *i) {
    DEBUG_ENTER("visitStruct");

    std::pair<lls::IRangeUP,lls::IRangeUP> r(getScopeRange(i));

    lls::IDocumentSymbolUP sym = m_factory->mkDocumentSymbol(
        i->getName()->getId(),
        lls::SymbolKind::Struct,
        r.first,
        r.second
    );

    lls::IDocumentSymbol *sym_p = sym.get();
    m_sym_s.back()->addChild(sym);
    m_sym_s.push_back(sym_p);
    VisitorBase::visitStruct(i);
    m_sym_s.pop_back();
    DEBUG_LEAVE("visitStruct");
}

std::pair<lls::IRangeUP, lls::IRangeUP> TaskBuildDocumentSymbols::getScopeRange(
    zsp::ast::INamedScope *s) {
    lls::IRangeUP range, selectionRange;

    lls::IPositionUP start, end;
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        (s->getLocation().linepos>0)?
            s->getLocation().linepos-1:0);
    end = m_factory->mkPosition(
        s->getEndLocation().lineno-1, 
        (s->getEndLocation().linepos>0)?
            s->getEndLocation().linepos-1:0);
    range = m_factory->mkRange(
        start,
        end);
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        (s->getLocation().linepos>0)?
            s->getLocation().linepos-1:0);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getName()->getId().size()-1);
    selectionRange = m_factory->mkRange(
        start,
        end);

    return {std::move(range), std::move(selectionRange)};
}

std::pair<lls::IRangeUP, lls::IRangeUP> TaskBuildDocumentSymbols::getScopeRange(
    zsp::ast::IPackageScope *s) {
    lls::IRangeUP range, selectionRange;

    lls::IPositionUP start, end;
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        (s->getLocation().linepos>0)?
            s->getLocation().linepos-1:0);
    end = m_factory->mkPosition(
        s->getEndLocation().lineno-1, 
        (s->getEndLocation().linepos>0)?
            s->getEndLocation().linepos-1:0);
    range = m_factory->mkRange(
        start,
        end);
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        (s->getLocation().linepos>0)?
            s->getLocation().linepos-1:0);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getId().back()->getId().size()-1);
    selectionRange = m_factory->mkRange(
        start,
        end);

    return {std::move(range), std::move(selectionRange)};
}

std::pair<lls::IRangeUP, lls::IRangeUP> TaskBuildDocumentSymbols::getItemRange(
    zsp::ast::INamedScopeChild *s) {
    lls::IRangeUP range, selectionRange;

    lls::IPositionUP start, end;
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        (s->getLocation().linepos>0)?
            s->getLocation().linepos-1:0);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getName()->getId().size()-1);
    range = m_factory->mkRange(
        start,
        end);
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos-1);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getName()->getId().size()-1);
    selectionRange = m_factory->mkRange(
        start,
        end);

    return {std::move(range), std::move(selectionRange)};
}

std::pair<lls::IRangeUP, lls::IRangeUP> TaskBuildDocumentSymbols::getItemRange(
    zsp::ast::IFunctionDefinition *s) {
    lls::IRangeUP range, selectionRange;

    lls::IPositionUP start, end;
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos-1);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getProto()->getName()->getId().size()-1);
    range = m_factory->mkRange(
        start,
        end);
    start = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos-1);
    end = m_factory->mkPosition(
        s->getLocation().lineno-1, 
        s->getLocation().linepos+s->getProto()->getName()->getId().size()-1);
    selectionRange = m_factory->mkRange(
        start,
        end);

    return {std::move(range), std::move(selectionRange)};
}

dmgr::IDebug *TaskBuildDocumentSymbols::m_dbg = 0;

}
}
