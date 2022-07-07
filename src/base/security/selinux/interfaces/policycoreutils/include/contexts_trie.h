/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CONTEXTS_TRIE_H
#define CONTEXTS_TRIE_H

#include <string>
#include <unordered_map>

class ParamContextsTrie {
public:
    ParamContextsTrie() {}
    ~ParamContextsTrie() {}

    ParamContextsTrie *FindChild(std::string element);
    bool Insert(const std::string &paramPrefix, const std::string &contexts);
    bool Search(const std::string &paraName, char **context);
    void Clear();

private:
    std::string prefixLabel = "";
    std::string matchLabel = "";
    std::unordered_map<std::string, ParamContextsTrie *> childen;
};

#endif // CONTEXTS_TRIE_H
