/*
 * TaskHover.cpp
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
#include "jrpc/impl/TaskLockRead.h"
#include "zsp/parser/impl/TaskResolveSymbolPathRef.h"
#include "TaskHover.h"


namespace zsp {
namespace ls {


TaskHover::TaskHover(
    const std::string   &id,
    Context             *ctxt,
    const std::string   &uri,
    int32_t             lineno,
    int32_t             linepos) : TaskBase(ctxt->getQueue()),
    m_id(id), m_ctxt(ctxt), m_idx(0), m_uri(uri),
    m_lineno(lineno), m_linepos(linepos) {

}

TaskHover::~TaskHover() {

}

jrpc::ITask *TaskHover::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);
    
    switch (m_idx) {
        case 0: {
            m_idx++;
            jrpc::ITask *n = jrpc::TaskLockRead(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true);

            if (n && !n->done()) {
                break;
            }
        }
        case 1: {
            m_idx++;
            lls::IFactory *factory = m_ctxt->getLspFactory();
            parser::ITaskFindElementByLocation::Result res;
            res.isValid = false;

            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);
                ast::ISymbolScope *symtab = file->getFileSymtab();

                parser::ITaskFindElementByLocationUP finder(
                    m_ctxt->getParserFactory()->mkTaskFindElementByLocation());
                DEBUG("Search for %d:%d", m_lineno, m_linepos);
                res = finder->find(symtab, file->getLiveAst(), m_lineno+1, m_linepos+1);

            } else {
                DEBUG("File %s is not present", m_uri.c_str());
            }

            if (res.isValid) {
                res.target->accept(this);
            }

            lls::IRangeUP range;
            if (m_result.size()) {
                lls::IJsonUP content(factory->mkMarkupContent(
                    lls::MarkupKind::Markdown, m_result));
                m_ctxt->getClient()->sendRspSuccess(
                    m_id,
                    factory->mkHover(content, range).release());
            } else {
                char tmp[128];
                sprintf(tmp, "Invalid @ %d:%d", m_lineno, m_linepos);
                lls::IJsonUP content(factory->mkContentMarkedString("", tmp));

                m_ctxt->getClient()->sendRspSuccess(
                    m_id,
                    factory->mkHover(content, range).release());
            }
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

void TaskHover::visitAction(ast::IAction *i) {
    m_result.append("```pss\n");
    m_result.append("action " + i->getName()->getId() + "\n");
    m_result.append("```\n");
    addDocComment(i);
}

void TaskHover::visitComponent(ast::IComponent *i) {
    m_result.append("```pss\n");
    m_result.append("component " + i->getName()->getId() + "\n");
    m_result.append("```\n");
    addDocComment(i);
}

void TaskHover::visitField(ast::IField *i) {

}

void TaskHover::visitStruct(ast::IStruct *i) {
    m_result.append("```pss\n");
    m_result.append("struct " + i->getName()->getId() + "\n");
    m_result.append("```\n");
    addDocComment(i);
}

void TaskHover::visitDataTypeUserDefined(ast::IDataTypeUserDefined *i) {
    DEBUG_ENTER("visitDataTypeUserDefined");

    if (i->getType_id()->getTarget()) {
        SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);

        ast::IScopeChild *target = parser::TaskResolveSymbolPathRef(
            m_ctxt->getDebugMgr(),
            file->getFileSymtab()).resolve(i->getType_id()->getTarget());
        
        target->accept(m_this);
    }

    DEBUG_LEAVE("visitDataTypeUserDefined");
}

void TaskHover::visitSymbolTypeScope(ast::ISymbolTypeScope *i) {
    i->getTarget()->accept(m_this);
}

void TaskHover::addDocComment(ast::IScopeChild *i) {
    if (i->getDocstring() != "") {
        m_result.append("\n");
        m_result.append(i->getDocstring());
    }
}

dmgr::IDebug *TaskHover::m_dbg = 0;
}
}
