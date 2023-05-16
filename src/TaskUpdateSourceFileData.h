/**
 * TaskUpdateSourceFileData.h
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
#include "jrpc/ITask.h"
#include "lls/IFactory.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/IAstBuilder.h"
#include "zsp/parser/IMarkerListener.h"
#include "SourceFileData.h"

namespace zsp {
namespace ls {



class TaskUpdateSourceFileData :
    public virtual zsp::parser::IMarkerListener,
    public virtual jrpc::ITask {
public:
    TaskUpdateSourceFileData(
        lls::IFactory               *factory,
        zsp::parser::IAstBuilder    *ast_builder,
        SourceFileData              *file);

    virtual ~TaskUpdateSourceFileData();

    virtual bool run(jrpc::ITaskQueue *queue) override;

	virtual void marker(const zsp::parser::IMarker *m) override;

	virtual bool hasSeverity(zsp::parser::MarkerSeverityE s) override;

private:
    static dmgr::IDebug         *m_dbg;
    lls::IFactory               *m_factory;
    zsp::parser::IAstBuilder    *m_ast_builder;
    SourceFileData              *m_file;
    int                         m_has[(int)zsp::parser::MarkerSeverityE::NumLevels];
};

}
}


