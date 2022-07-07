/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hiappevent_c.h"

#define NEW(type) (type*)malloc(sizeof(type))
#define NEW_PARAM(uType, uName, uValue, len) ({ \
    ParamValue* pValue = NEW(ParamValue);  \
    if (pValue != NULL) {   \
        pValue->type = uType;    \
        pValue->value.uName = uValue; \
        pValue->arrSize = len;  \
    }   \
    pValue; \
})

#define ADD_BOOL_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(BOOL_PARAM, bool_v, uValue, 0))
#define ADD_BOOL_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(BOOL_ARR_PARAM, bool_arr_v, uValue, len))
#define ADD_INT8_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(INT8_PARAM, int8_v, uValue, 0))
#define ADD_INT8_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(INT8_ARR_PARAM, int8_arr_v, uValue, len))
#define ADD_INT16_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(INT16_PARAM, int16_v, uValue, 0))
#define ADD_INT16_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(INT16_ARR_PARAM, int16_arr_v, uValue, len))
#define ADD_INT32_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(INT32_PARAM, int32_v, uValue, 0))
#define ADD_INT32_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(INT32_ARR_PARAM, int32_arr_v, uValue, len))
#define ADD_INT64_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(INT64_PARAM, int64_v, uValue, 0))
#define ADD_INT64_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(INT64_ARR_PARAM, int64_arr_v, uValue, len))
#define ADD_FLOAT_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(FLOAT_PARAM, float_v, uValue, 0))
#define ADD_FLOAT_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(FLOAT_ARR_PARAM, float_arr_v, uValue, len))
#define ADD_DOUBLE_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(DOUBLE_PARAM, double_v, uValue, 0))
#define ADD_DOUBLE_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(DOUBLE_ARR_PARAM, double_arr_v, uValue, len))
#define ADD_STRING_PARAM(list, name, uValue)  \
    AddParamValue(list, name, NEW_PARAM(STRING_PARAM, str_v, uValue, 0))
#define ADD_STRING_ARR_PARAM(list, name, uValue, len) \
    AddParamValue(list, name, NEW_PARAM(STRING_ARR_PARAM, str_arr_v, uValue, len))

ParamEntry* CreateParamEntry(const char* name, ParamValue* value)
{
    ParamEntry* entry = NEW(ParamEntry);
    if (entry != NULL) {
        entry->name = name;
        entry->value = value;
    }
    return entry;
}

ParamList OH_HiAppEvent_CreateParamList()
{
    ParamList list = NEW(ParamListNode);
    if (list != NULL) {
        list->entry = NULL;
        list->next = NULL;
    }
    return list;
}

void DestroyParamValue(ParamValue* value)
{
    if (value != NULL) {
        free(value);
    }
}

void DestroyParamEntry(ParamEntry* entry)
{
    if (entry != NULL) {
        DestroyParamValue(entry->value);
        free(entry);
    }
}

void OH_HiAppEvent_DestroyParamList(ParamList list)
{
    ParamList curNode = list;
    while (curNode != NULL) {
        ParamList nextNode = curNode->next;
        DestroyParamEntry(curNode->entry);
        free(curNode);
        curNode = nextNode;
    }
}

ParamList AddParamValue(ParamList list, const char* name, ParamValue* value)
{
    // return the list if the ParamValue is null(list and name have been checked)
    if (value == NULL) {
        return list;
    }

    // if the param is the first one added, create the head node of the list
    if (list->entry == NULL) {
        ParamEntry* entry = CreateParamEntry(name, value);
        if (entry == NULL) {
            DestroyParamValue(value);
            return list;
        }
        list->entry = entry;
        return list;
    }

    ParamList curNode = list;
    while (curNode != NULL) {
        if (curNode->entry != NULL && (strcmp(name, curNode->entry->name) == 0)) {
            // if the names of the params are same, overwrite it
            ParamValue* oldValue = curNode->entry->value;
            curNode->entry->value = value;
            DestroyParamValue(oldValue);
            return list;
        } else if (curNode->next == NULL) {
            // if the curNode is the last node, create a tail node
            ParamListNode* node = OH_HiAppEvent_CreateParamList();
            ParamEntry* entry = CreateParamEntry(name, value);
            if (node == NULL || entry == NULL) {
                DestroyParamValue(value);
                DestroyParamEntry(entry);
                OH_HiAppEvent_DestroyParamList(node);
                return list;
            }
            node->entry = entry;
            node->next = NULL;
            curNode->next = node;
            return list;
        } else {
            // otherwise, find the next Node
            curNode = curNode->next;
        }
    }
    return list;
}

ParamList OH_HiAppEvent_AddBoolParam(ParamList list, const char* name, bool boolean)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_BOOL_PARAM(list, name, boolean);
}

ParamList OH_HiAppEvent_AddBoolArrayParam(ParamList list, const char* name, const bool* booleans, int arrSize)
{
    if (list == NULL || name == NULL || booleans == NULL) {
        return list;
    }
    return ADD_BOOL_ARR_PARAM(list, name, booleans, arrSize);
}

ParamList OH_HiAppEvent_AddInt8Param(ParamList list, const char* name, int8_t num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_INT8_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddInt8ArrayParam(ParamList list, const char* name, const int8_t* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_INT8_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddInt16Param(ParamList list, const char* name, int16_t num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_INT16_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddInt16ArrayParam(ParamList list, const char* name, const int16_t* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_INT16_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddInt32Param(ParamList list, const char* name, int32_t num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_INT32_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddInt32ArrayParam(ParamList list, const char* name, const int32_t* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_INT32_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddInt64Param(ParamList list, const char* name, int64_t num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_INT64_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddInt64ArrayParam(ParamList list, const char* name, const int64_t* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_INT64_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddFloatParam(ParamList list, const char* name, float num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_FLOAT_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddFloatArrayParam(ParamList list, const char* name, const float* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_FLOAT_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddDoubleParam(ParamList list, const char* name, double num)
{
    if (list == NULL || name == NULL) {
        return list;
    }
    return ADD_DOUBLE_PARAM(list, name, num);
}

ParamList OH_HiAppEvent_AddDoubleArrayParam(ParamList list, const char* name, const double* nums, int arrSize)
{
    if (list == NULL || name == NULL || nums == NULL) {
        return list;
    }
    return ADD_DOUBLE_ARR_PARAM(list, name, nums, arrSize);
}

ParamList OH_HiAppEvent_AddStringParam(ParamList list, const char* name, const char* str)
{
    if (list == NULL || name == NULL || str == NULL) {
        return list;
    }
    return ADD_STRING_PARAM(list, name, str);
}

ParamList OH_HiAppEvent_AddStringArrayParam(ParamList list, const char* name, const char* const *strs, int arrSize)
{
    if (list == NULL || name == NULL || strs == NULL) {
        return list;
    }
    return ADD_STRING_ARR_PARAM(list, name, strs, arrSize);
}

bool OH_HiAppEvent_Configure(const char* name, const char* value)
{
    return HiAppEventInnerConfigure(name, value);
}

int OH_HiAppEvent_Write(const char* domain, const char* name, enum EventType type, const ParamList list)
{
    return HiAppEventInnerWrite(domain, name, type, list);
}