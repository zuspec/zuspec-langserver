/**
 * TaskFindSourceFiles.h
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
#include <string>
#include <vector>
#include "jrpc/impl/TaskBase.h"
#include "lls/IFactory.h"
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {


class TaskFindSourceFiles : public virtual jrpc::TaskBase {
public:

    TaskFindSourceFiles(
        jrpc::ITaskQueue                *queue,
        lls::IFactory                   *factory,
        SourceFileCollection            *src_files,
        const std::vector<std::string>  &roots);

    TaskFindSourceFiles(TaskFindSourceFiles *o);

    virtual ~TaskFindSourceFiles();

    virtual jrpc::ITask *run(jrpc::ITask *parent, bool initial) override;

    virtual TaskFindSourceFiles *clone() override;

private:
    static dmgr::IDebug                 *m_dbg;
    lls::IFactory                       *m_factory;
    SourceFileCollection                *m_src_files;
    std::vector<std::string>            m_roots;

};

}
}


