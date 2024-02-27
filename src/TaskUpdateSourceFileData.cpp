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
    jrpc::ITaskGroup                *group,
    Context                         *ctxt,
    SourceFileCollection            *files,
    SourceFileData                  *file) : TaskBase(group),
        m_ctxt(ctxt), m_files(files), m_file(file) {
    DEBUG_INIT("TaskUpdateSourceFileData", ctxt->getDebugMgr());
    memset(m_has, 0, sizeof(m_has));
    DEBUG("src.getLiveContent.size()=%d", file->getLiveContent().size());
}

TaskUpdateSourceFileData::TaskUpdateSourceFileData(TaskUpdateSourceFileData *o) :
    TaskBase(this), m_ctxt(o->m_ctxt), m_files(o->m_files),
    m_file(o->m_file) {

};

TaskUpdateSourceFileData::~TaskUpdateSourceFileData() {

}

jrpc::TaskStatus TaskUpdateSourceFileData::run() {
    DEBUG_ENTER("run");
    zsp::ast::IGlobalScopeUP global(
        m_ctxt->getAstFactory()->mkGlobalScope(m_file->getId()));
    zsp::parser::IAstBuilder *builder = m_ctxt->allocAstBuilder();
    builder->setMarkerListener(this);

    std::istream *is = 0;
    std::stringstream sstr;
    std::ifstream fstr;

    DEBUG("File: %s ; content: %d",
        m_file->getUri().c_str(),
        m_file->getLiveContent().size());

    if (m_file->getLiveContent().size()) {
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
    
    if (m_file->getLiveContent().size()) {
        if (m_diagnostics.size() == 0) {
            m_file->setLiveAst(global);
        }
    } else {
        m_file->setStaticAst(global);
    }

    // We need to publish diagnostics if we have new ones (from the parse)
    // as well as if a prior parse had markers and the current one does not.
    // This operation clears the previously-set markers
    if (true /*m_diagnostics.size() > 0 || m_file->haveMarkers() */) {
        m_file->setHaveMarkers(m_diagnostics.size() > 0);
        lls::IPublishDiagnosticsParamsUP params(m_ctxt->getLspFactory()->mkPublishDiagnosticsParams(
            m_file->getUri(),
            -1,
            m_diagnostics));

        m_ctxt->getClient()->publishDiagnosticsNotification(params);
    }

    m_ctxt->freeAstBuilder(builder);

    DEBUG_LEAVE("run");
    return jrpc::TaskStatus::Done;
}

void TaskUpdateSourceFileData::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker: %s", m->msg().c_str());
    m_has[(int)m->severity()]++;

    lls::DiagnosticSeverity severity = lls::DiagnosticSeverity::Information;
    switch (m->severity()) {
        case zsp::parser::MarkerSeverityE::Error:
            severity = lls::DiagnosticSeverity::Error;
            break;
        case zsp::parser::MarkerSeverityE::Warn:
            severity = lls::DiagnosticSeverity::Warning;
            break;
        case zsp::parser::MarkerSeverityE::Info:
            severity = lls::DiagnosticSeverity::Information;
            break;
        case zsp::parser::MarkerSeverityE::Hint:
            severity = lls::DiagnosticSeverity::Hint;
            break;
    }

    lls::IPositionUP start(m_ctxt->getLspFactory()->mkPosition(m->loc().lineno, m->loc().linepos));
    lls::IPositionUP end(m_ctxt->getLspFactory()->mkPosition(m->loc().lineno, m->loc().linepos+1));
    lls::IRangeUP range(m_ctxt->getLspFactory()->mkRange(start, end));
    lls::IDiagnosticUP diagnostic(m_ctxt->getLspFactory()->mkDiagnostic(
        range,
        severity,
        m->msg()
    ));
    m_diagnostics.push_back(std::move(diagnostic));

    DEBUG_LEAVE("marker: %s", m->msg().c_str());
}

bool TaskUpdateSourceFileData::hasSeverity(zsp::parser::MarkerSeverityE s) {
    return m_has[(int)s];
}

dmgr::IDebug *TaskUpdateSourceFileData::m_dbg = 0;

}
}
