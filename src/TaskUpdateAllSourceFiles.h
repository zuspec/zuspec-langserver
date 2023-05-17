/**
 * TaskUpdateAllSourceFiles.h
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
#include "zsp/parser/IAstBuilder.h"
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {



class TaskUpdateAllSourceFiles : public virtual jrpc::ITask {
public:
    TaskUpdateAllSourceFiles(
        lls::IFactory               *factory,
        lls::IClient                *client,
        zsp::parser::IAstBuilder    *ast_builder,
        SourceFileCollection        *src_files
    );

    virtual ~TaskUpdateAllSourceFiles();

    virtual bool run(jrpc::ITaskQueue *queue) override;

private:
    lls::IFactory                   *m_factory;
    lls::IClient                    *m_client;
    zsp::parser::IAstBuilder        *m_ast_builder;
    SourceFileCollection            *m_src_files;

};

}
}


