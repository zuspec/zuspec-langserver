/*
 * TaskPublishDiagnostics.cpp
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
#include "TaskPublishDiagnostics.h"


namespace zsp {
namespace ls {


TaskPublishDiagnostics::TaskPublishDiagnostics(
    Context             *ctxt,
    SourceFileData      *file,
    bool                use_live) : TaskBase(ctxt->getQueue()), 
        m_ctxt(ctxt), m_file(file), m_use_live(use_live) {
    DEBUG_INIT("zsp::ls::TaskPublishDiagnostics", ctxt->getDebugMgr());
    m_max = 100;
}

TaskPublishDiagnostics::~TaskPublishDiagnostics() {

}

jrpc::ITask *TaskPublishDiagnostics::run(jrpc::ITask *parent, bool initial) {
    runEnter(parent, initial);
    std::vector<lls::IDiagnosticUP> diagnostics;

    for (uint32_t i=0; i<m_max && i<m_file->getSyntaxMarkers(m_use_live).size(); i++) {
        diagnostics.push_back(std::move(markerToDiagnostic(
            m_file->getSyntaxMarkers(m_use_live).at(i).get())));
    }

    lls::IPublishDiagnosticsParamsUP params(m_ctxt->getLspFactory()->mkPublishDiagnosticsParams(
        m_file->getUri(),
        -1,
        diagnostics));

    m_ctxt->getClient()->publishDiagnosticsNotification(params);

    setFlags(jrpc::TaskFlags::Complete);

    return runLeave(parent, initial);
}

lls::IDiagnosticUP TaskPublishDiagnostics::markerToDiagnostic(const zsp::parser::IMarker *m) {
    int start_lineno = (m->loc().lineno)?m->loc().lineno-1:0;
    int end_lineno = start_lineno;
    int start_linepos = m->loc().linepos;
    int end_linepos = m->loc().linepos+m->loc().extent;

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

    lls::IPositionUP start(m_ctxt->getLspFactory()->mkPosition(start_lineno, start_linepos));
    lls::IPositionUP end(m_ctxt->getLspFactory()->mkPosition(end_lineno, end_linepos));
    lls::IRangeUP range(m_ctxt->getLspFactory()->mkRange(start, end));
    return lls::IDiagnosticUP(m_ctxt->getLspFactory()->mkDiagnostic(
        range,
        severity,
        m->msg()
    ));
}

dmgr::IDebug *TaskPublishDiagnostics::m_dbg = 0;

}
}
