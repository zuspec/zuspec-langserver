/*
 * TaskDeclaration.cpp
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
#include "TaskDeclaration.h"
#include "TaskUpdateFileSymtab.h"


namespace zsp {
namespace ls {


TaskDeclaration::TaskDeclaration(
    Context             *ctxt,
    const std::string   &id,
    const std::string   &uri,
    int32_t             lineno,
    int32_t             linepos) : TaskBase(ctxt->getQueue()),
    m_ctxt(ctxt), m_idx(0), m_id(id), m_uri(uri), 
    m_lineno(lineno), m_linepos(linepos) {

    DEBUG_INIT("zsp::ls::TaskDeclaration", ctxt->getDebugMgr());
}

TaskDeclaration::~TaskDeclaration() {

}

jrpc::ITask *TaskDeclaration::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx++;
            jrpc::ITask *n = jrpc::TaskLockRead(
                m_queue, 
                m_ctxt->getSourceFiles()->getLock()).run(this, true);
            if (n && !n->done()) {
                break;
            }
        }
        case 1: {
            m_idx++;
            // Now that we have the source index locked, 
            // find the indicated location
            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);

                if (!file->getFileSymtab()) {
                    jrpc::ITask *n = TaskUpdateFileSymtab(m_ctxt, m_uri, false).run(this, 1);

                    if (n && !n->done()) {
                        break;
                    }
                }
            }
        }

        case 2: {
            SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);
            parser::ITaskFindElementByLocation::Result res;
            ast::ISymbolScope *symtab = file->getFileSymtab();
            res.isValid = false;

            m_idx++;

            if (symtab) {
                parser::ITaskFindElementByLocationUP finder(
                    m_ctxt->getParserFactory()->mkTaskFindElementByLocation());
                DEBUG("Search for %d:%d", m_lineno+1, m_linepos+1);
                // 
                res = finder->find(symtab, file->getLiveAst(), m_lineno+1, m_linepos+1);

            } else {
                DEBUG("File %s is not present", m_uri.c_str());
            }
           
            DEBUG("isValid=%d", res.isValid);
            if (res.isValid) {
                // Unpack a bit ... 
                res.target->accept(this);
                /*
                res.target->accept(this);
                if (res.isType) {
                    // Already pointing at the target
                    DEBUG("Target location: %d:%d",
                        res.target->getLocation().lineno,
                        res.target->getLocation().linepos);
                } else {
                    res.target->accept(this);
                }
                 */
            } else {
                DEBUG("Location find is not valid");
            }
                // lls::IPositionUP start(m_ctxt->getLspFactory()->mkPosition(
                //     res.sourceRange.start.lineno,
                //     res.sourceRange.start.linepos));
                // lls::IPositionUP end(m_ctxt->getLspFactory()->mkPosition(
                //     res.sourceRange.end.lineno,
                //     res.sourceRange.end.linepos));
                // lls::IRangeUP range(m_ctxt->getLspFactory()->mkRange(
                //     start,
                //     end));
                // DEBUG("Source: %d:%d..%d:%d",
                //     range->getStart()->getLine(),
                //     range->getStart()->getCharacter(),
                //     range->getEnd()->getLine(),
                //     range->getEnd()->getCharacter());
                // lls::ILocationUP target(m_ctxt->getLspFactory()->mkLocation(
                //     m_ctxt->getSourceFiles()->getFileUri(
                //         res.target->getLocation().fileid),
                //     range));

            if (m_response) {
                DEBUG("Send real response");
                m_ctxt->getClient()->sendRspSuccess(m_id, m_response.release());
            } else {
                // Send null response, since no content
                lls::IJsonUP Null(m_ctxt->getLspFactory()->mkNull());

                DEBUG("Send null response");
                m_ctxt->getClient()->sendRspSuccess(
                    m_id,
                    Null.release()
                );
            }

            setFlags(jrpc::TaskFlags::Complete);
            m_ctxt->getSourceFiles()->getLock()->unlock_read();
        }
    }

    m_ctxt->getDebugMgr()->flush();

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

void TaskDeclaration::visitDataTypeUserDefined(ast::IDataTypeUserDefined *i) {
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

void TaskDeclaration::visitField(ast::IField *i) {
    DEBUG_ENTER("visitField");
    i->getType()->accept(m_this);
    DEBUG_LEAVE("visitField");
}

void TaskDeclaration::visitSymbolTypeScope(ast::ISymbolTypeScope *i) {
    DEBUG_ENTER("visitSymbolTypeScope");
    if (i->getTarget()) {
        i->getTarget()->accept(m_this);
    } else {
        DEBUG("No target");
    }
#ifdef UNDEFINED
    ast::IExprId *id = dynamic_cast<ast::ITypeScope *>(i->getTarget())->getName();

    int32_t start_lineno = id->getLocation().lineno;
    int32_t start_linepos = id->getLocation().linepos;

    if (start_lineno) {
        start_lineno--;
    }
    if (start_linepos) {
        start_linepos--;
    }
    
    int end_lineno = start_lineno;
    int end_linepos = start_linepos + id->getId().size();
    DEBUG("Range: %d:%d..%d:%d", start_lineno, start_linepos, end_lineno, end_linepos);

    lls::IPositionUP start(m_ctxt->getLspFactory()->mkPosition(start_lineno, start_linepos));
    lls::IPositionUP end(m_ctxt->getLspFactory()->mkPosition(end_lineno, end_linepos));
    lls::IRangeUP range(m_ctxt->getLspFactory()->mkRange(start, end));
    /*
    DEBUG("Source: %d:%d..%d:%d",
        range->getStart()->getLine(),
        range->getStart()->getCharacter(),
        range->getEnd()->getLine(),
        range->getEnd()->getCharacter());
     */
    DEBUG("Dest fileid: %d", id->getLocation().fileid);
    m_response = m_ctxt->getLspFactory()->mkLocation(
        m_ctxt->getSourceFiles()->getFileUri(id->getLocation().fileid), range);
#endif // UNDEFINED
    DEBUG_LEAVE("visitSymbolTypeScope");
}

void TaskDeclaration::visitTypeIdentifier(ast::ITypeIdentifier *i) {
    DEBUG_ENTER("visitTypeIdentifier");
    DEBUG_LEAVE("visitTypeIdentifier");
}

void TaskDeclaration::visitTypeScope(ast::ITypeScope *i) {
    DEBUG_ENTER("visitTypeScope %s", i->getName()->getId().c_str());
    ast::IExprId *id = i->getName();
    if (m_ctxt->getSourceFiles()->hasFileUri(id->getLocation().fileid)) {

        int32_t start_lineno = id->getLocation().lineno;
        int32_t start_linepos = id->getLocation().linepos;

        if (start_lineno) {
            start_lineno--;
        }
        if (start_linepos) {
            start_linepos--;
        }
    
        int end_lineno = start_lineno;
        int end_linepos = start_linepos + id->getId().size();
        DEBUG("Range: %d:%d..%d:%d", start_lineno, start_linepos, end_lineno, end_linepos);

        lls::IPositionUP start(m_ctxt->getLspFactory()->mkPosition(start_lineno, start_linepos));
        lls::IPositionUP end(m_ctxt->getLspFactory()->mkPosition(end_lineno, end_linepos));
        lls::IRangeUP range(m_ctxt->getLspFactory()->mkRange(start, end));

        DEBUG("Dest fileid: %d", id->getLocation().fileid);
        m_response = m_ctxt->getLspFactory()->mkLocation(
            m_ctxt->getSourceFiles()->getFileUri(id->getLocation().fileid), range);
    } else {
        DEBUG("Fileid %d is not managed by the source collection",
            i->getLocation().fileid);
    }
    DEBUG_LEAVE("visitTypeScope %s", i->getName()->getId().c_str());
}

dmgr::IDebug *TaskDeclaration::m_dbg = 0;

}
}
