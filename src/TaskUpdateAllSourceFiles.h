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
#include "jrpc/impl/TaskBase.h"
#include "lls/IFactory.h"
#include "zsp/parser/IAstBuilder.h"
#include "Context.h"
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {



class TaskUpdateAllSourceFiles : public virtual jrpc::TaskBase {
public:
    TaskUpdateAllSourceFiles(
        jrpc::ITaskQueue            *queue,
        Context                     *ctxt,
        SourceFileCollection        *src_files
    );

    TaskUpdateAllSourceFiles(TaskUpdateAllSourceFiles *o);

    virtual ~TaskUpdateAllSourceFiles();

    virtual TaskUpdateAllSourceFiles *clone() override {
        return new TaskUpdateAllSourceFiles(this);
    }

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

private:
    Context                         *m_ctxt;
    SourceFileCollection            *m_src_files;

};

}
}


