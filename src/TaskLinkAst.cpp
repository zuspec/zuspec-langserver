/*
 * TaskLinkAst.cpp
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
#include "TaskLinkAst.h"


namespace zsp {
namespace ls {


TaskLinkAst::TaskLinkAst(
    Context                                 *ctxt,
    const std::vector<ast::IGlobalScope *>  &files,
    bool                                    own_files) : TaskBase(ctxt->getQueue()),
    m_ctxt(ctxt), m_files(files.begin(), files.end()) {
    DEBUG_INIT("zsp::ls::TaskLinkAst", ctxt->getDebugMgr());
}

TaskLinkAst::~TaskLinkAst() {

}

jrpc::ITask *TaskLinkAst::run(jrpc::ITask *parent, bool initial) {
    runEnter(parent, initial);

    zsp::parser::ILinkerUP linker(m_ctxt->getParserFactory()->mkAstLinker());

    zsp::ast::IRootSymbolScopeUP root(linker->link(
        this,
        m_files,
        false));

    setResult(jrpc::TaskResult(root.release(), true));

    setFlags(jrpc::TaskFlags::Complete);

    return runLeave(parent, initial);
}

void TaskLinkAst::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker: %s (%d:%d)", 
        m->msg().c_str(),
        m->loc().lineno, m->loc().linepos);

    if (m_ctxt->getSourceFiles()->hasFileUri(m->loc().fileid)) {
        std::string uri = m_ctxt->getSourceFiles()->getFileUri(m->loc().fileid);
        SourceFileData *file = m_ctxt->getSourceFiles()->getFile(uri);

        zsp::parser::IMarkerUP mc(m->clone());
        file->addLinkMarker(mc, false);
    } else {
        DEBUG("Fileid %s is not recognized", m->loc().fileid);
    }

    DEBUG_LEAVE("marker: %s", m->msg().c_str());
}

bool TaskLinkAst::hasSeverity(zsp::parser::MarkerSeverityE s) {
    return false;
}

dmgr::IDebug *TaskLinkAst::m_dbg = 0;

}
}
