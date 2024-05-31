/*
 * TaskUpdateSourceFileData.cpp
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
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "dmgr/impl/DebugMacros.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskUpdateSourceFileData::TaskUpdateSourceFileData(
    Context                         *ctxt,
    SourceFileData                  *file,
    bool                            update_live) : TaskBase(ctxt->getQueue()),
        m_ctxt(ctxt), m_file(file), m_update_live(update_live) {
    DEBUG_INIT("TaskUpdateSourceFileData", ctxt->getDebugMgr());
    DEBUG("src.getLiveContent.size()=%d", file->getLiveContent().size());
}

TaskUpdateSourceFileData::TaskUpdateSourceFileData(TaskUpdateSourceFileData *o) :
    TaskBase(o), m_ctxt(o->m_ctxt), m_file(o->m_file),
    m_update_live(o->m_update_live) {

}

TaskUpdateSourceFileData::~TaskUpdateSourceFileData() {

}

jrpc::ITask *TaskUpdateSourceFileData::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run fileid: %d", m_file->getId());
    runEnter(parent, initial);
    zsp::ast::IGlobalScopeUP global(
        m_ctxt->getAstFactory()->mkGlobalScope(m_file->getId()));
    zsp::parser::IAstBuilder *builder = m_ctxt->allocAstBuilder();
    builder->setMarkerListener(this);
    builder->setCollectDocStrings(true);

    std::istream *is = 0;
    std::stringstream sstr;
    std::ifstream fstr;

    DEBUG("File: %s ; content: %d",
        m_file->getUri().c_str(),
        m_file->getLiveContent().size());

    m_file->clearMarkers(m_update_live);

    if (m_update_live) {
        // We're working with live content
        sstr = std::stringstream(m_file->getLiveContent());
        is = &sstr;
    } else {
        // We're parsing content off disk
        // TODO:
        std::string path = m_file->getUri().substr(7);
        fstr.open(path.c_str(), std::ios::in);
        is = &fstr;
    }

    if (is) {
        builder->build(
            global.get(),
            is);
    }

    if (is == &fstr) {
        fstr.close();
    }
    
    if (m_update_live) {
        // Handle last-good here?
        if (m_diagnostics.size() == 0) {
            DEBUG("Set Live AST for %s", m_file->getUri().c_str());
            m_file->setLiveAst(global);
        }
    } else {
        DEBUG("Set Static AST for %s (%p)", m_file->getUri().c_str(), global.get());
        m_file->setStaticAst(global);
    }

    // We need to publish diagnostics if we have new ones (from the parse)
    // as well as if a prior parse had markers and the current one does not.
    // This operation clears the previously-set markers
    if (true /*m_diagnostics.size() > 0 || m_file->haveMarkers() */) {

    }

    m_ctxt->freeAstBuilder(builder);

    setFlags(jrpc::TaskFlags::Complete);

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

void TaskUpdateSourceFileData::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker: %s (%d:%d)", 
        m->msg().c_str(),
        m->loc().lineno, m->loc().linepos);

    zsp::parser::IMarkerUP mc(m->clone());
    m_file->addSyntaxMarker(mc, m_update_live);

    DEBUG_LEAVE("marker: %s", m->msg().c_str());
}

bool TaskUpdateSourceFileData::hasSeverity(zsp::parser::MarkerSeverityE s) {
    return m_file->hasSeverity(s, m_update_live);
}

dmgr::IDebug *TaskUpdateSourceFileData::m_dbg = 0;

}
}
