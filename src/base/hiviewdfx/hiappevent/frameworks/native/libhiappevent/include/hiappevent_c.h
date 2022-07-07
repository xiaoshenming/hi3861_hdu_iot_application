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

#ifndef HIAPPEVENT_C_H
#define HIAPPEVENT_C_H

#include <stdbool.h>

#include "hiappevent/hiappevent.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event Param types.
 *
 * @since 8
 * @version 1.0
 */
enum ParamType {
    /* param type of the bool */
    BOOL_PARAM = 0,

    /* param type of the bool array */
    BOOL_ARR_PARAM = 1,

    /* param type of the int8_t */
    INT8_PARAM = 2,

    /* param type of the int8_t array */
    INT8_ARR_PARAM = 3,

    /* param type of the int16_t */
    INT16_PARAM = 4,

    /* param type of the int16_t array */
    INT16_ARR_PARAM = 5,

    /* param type of the int32_t */
    INT32_PARAM = 6,

    /* param type of the int32_t array */
    INT32_ARR_PARAM = 7,

    /* param type of the int64_t */
    INT64_PARAM = 8,

    /* param type of the int64_t array */
    INT64_ARR_PARAM = 9,

    /* param type of the float */
    FLOAT_PARAM = 10,

    /* param type of the float array */
    FLOAT_ARR_PARAM = 11,

    /* param type of the double */
    DOUBLE_PARAM = 12,

    /* param type of the double array */
    DOUBLE_ARR_PARAM = 13,

    /* param type of the string */
    STRING_PARAM = 14,

    /* param type of the string array */
    STRING_ARR_PARAM = 15
};

/**
 * @brief Event param value.
 *
 * @since 8
 * @version 1.0
 */
typedef struct ParamValue {
    /* param type */
    enum ParamType type;

    /* param value */
    union Value {
        /* param value of the bool type */
        bool bool_v;

        /* param value of the bool array type */
        const bool* bool_arr_v;

        /* param value of the int8_t type */
        int8_t int8_v;

        /* param value of the int8_t array type */
        const int8_t* int8_arr_v;

        /* param value of the int16_t type */
        int16_t int16_v;

        /* param value of the int16_t array type */
        const int16_t* int16_arr_v;

        /* param value of the int32_t type */
        int32_t int32_v;

        /* param value of the int32_t array type */
        const int32_t* int32_arr_v;

        /* param value of the int64_t type */
        int64_t int64_v;

        /* param value of the int64_t array type */
        const int64_t* int64_arr_v;

        /* param value of the float type */
        float float_v;

        /* param value of the float array type */
        const float* float_arr_v;

        /* param value of the double type */
        double double_v;

        /* param value of the double array type */
        const double* double_arr_v;

        /* param value of the string type */
        const char* str_v;

        /* param value of the string array type */
        const char * const *str_arr_v;
    } value;

    /* If the param type is array, the array size must be passed in. */
    int arrSize;
} ParamValue;

/**
 * @brief Event param entry.
 *
 * @since 8
 * @version 1.0
 */
typedef struct ParamEntry {
    /* the name of param */
    const char* name;

    /* the value of param */
    ParamValue* value;
} ParamEntry;

/**
 * @brief Event param list node.
 *
 * @since 8
 * @version 1.0
 */
typedef struct ParamListNode {
    /* param entry */
    ParamEntry* entry;

    /* next param node */
    struct ParamListNode* next;
} ParamListNode, *ParamList;

bool HiAppEventInnerConfigure(const char* name, const char* value);

int HiAppEventInnerWrite(const char* domain, const char* name, enum EventType type, const ParamList list);

#ifdef __cplusplus
}
#endif
#endif // HIAPPEVENT_C_H
