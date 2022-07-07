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

#include <deque>
#include <vector>
#include "callbacks.h"
#include "contexts_trie.h"

static std::vector<std::string> StringSplit(std::string paraName, std::string split = ".")
{
    size_t pos;
    std::vector<std::string> result;
    while ((pos = paraName.find(split)) != std::string::npos) {
        std::string element = paraName.substr(0, pos);
        if (!element.empty()) {
            result.push_back(element);
        }
        paraName.erase(0, pos + split.length());
    }
    if (!paraName.empty()) {
        result.push_back(paraName);
    }
    return result;
}

static std::string GetFirstElement(std::string &paraName, std::string split = ".")
{
    size_t pos;
    if ((pos = paraName.find(split)) != std::string::npos) {
        std::string element = paraName.substr(0, pos);
        paraName.erase(0, pos + split.length());
        if (!element.empty()) {
            return element;
        }
    }
    std::string result = paraName;
    paraName = "";
    return result;
}

ParamContextsTrie *ParamContextsTrie::FindChild(std::string element)
{
    ParamContextsTrie *root = this;
    auto iter = root->childen.find(element);
    if (iter != root->childen.end()) {
        return iter->second;
    }
    return nullptr;
}

bool ParamContextsTrie::Insert(const std::string &paramPrefix, const std::string &contexts)
{
    ParamContextsTrie *root = this;
    std::vector<std::string> elements = StringSplit(paramPrefix);
    for (const auto &element : elements) {
        if (root->childen[element] == nullptr) {
            root->childen[element] = new (std::nothrow) ParamContextsTrie();
            if (root->childen[element] == nullptr) {
                return false;
            }
        }
        root = root->childen[element];
    }
    if (paramPrefix.back() == '.') {
        root->prefixLabel = contexts;
    } else {
        root->matchLabel = contexts;
    }
    return true;
}

bool ParamContextsTrie::Search(const std::string &paraName, char **context)
{
    ParamContextsTrie *root = this;
    std::string tmpString = paraName;
    std::string element = GetFirstElement(tmpString);
    const char *updataCurLabel = "";
    while (!element.empty()) {
        auto child = root->FindChild(element);
        if (child == nullptr) {
            if (!root->prefixLabel.empty()) {
                *context = strdup(root->prefixLabel.c_str());
                return true;
            } else if (strcmp(updataCurLabel, "")) {
                *context = strdup(updataCurLabel);
                return true;
            } else {
                return false;
            }
        }
        if (!root->prefixLabel.empty())
            updataCurLabel = root->prefixLabel.c_str();
        root = child;
        element = GetFirstElement(tmpString);
    }

    if (!root->matchLabel.empty()) {
        *context = strdup(root->matchLabel.c_str());
        return true;
    } else if (strcmp(updataCurLabel, "")) {
        *context = strdup(updataCurLabel);
        return true;
    } else {
        return false;
    }
}

void ParamContextsTrie::Clear()
{
    ParamContextsTrie *root = this;
    std::deque<ParamContextsTrie *> nodeDeque;
    for (auto child : root->childen) {
        nodeDeque.emplace_back(child.second);
    }
    while (!nodeDeque.empty()) {
        root = nodeDeque.front();
        nodeDeque.pop_front();
        if (root != nullptr) {
            for (auto child : root->childen) {
                nodeDeque.emplace_back(child.second);
            }
            delete root;
        }
    }
}
