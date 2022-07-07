/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INIT_UTILS_H
#define INIT_UTILS_H
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    char *name;
    int value;
} InitArgInfo;

#define BINARY_BASE 2
#define OCTAL_BASE 8
#define DECIMAL_BASE 10
#define WAIT_MAX_SECOND 5
#define MAX_BUFFER_LEN 256
#define CMDLINE_VALUE_LEN_MAX 512
#define PARAM_CMD_LINE "/proc/cmdline"
#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof((array)[0]))

uid_t DecodeUid(const char *name);
char *ReadFileToBuf(const char *configFile);
int GetProcCmdlineValue(const char *name, const char *buffer, char *value, int length);
char *ReadFileData(const char *fileName);

int SplitString(char *srcPtr, const char *del, char **dstPtr, int maxNum);
void WaitForFile(const char *source, unsigned int maxSecond);
size_t WriteAll(int fd, const char *buffer, size_t size);
char *GetRealPath(const char *source);
int StringToInt(const char *str, int defaultValue);
int MakeDirRecursive(const char *dir, mode_t mode);
int MakeDir(const char *dir, mode_t mode);
int ReadFileInDir(const char *dirPath, const char *includeExt,
    int (*processFile)(const char *fileName, void *context), void *context);
char **SplitStringExt(char *buffer, const char *del, int *returnCount, int maxItemCount);
void FreeStringVector(char **vector, int count);
int InUpdaterMode(void);
int InChargerMode(void);
int StringReplaceChr(char *strl, char oldChr, char newChr);

int GetMapValue(const char *name, const InitArgInfo *infos, int argNum, int defValue);
const InitArgInfo *GetServieStatusMap(int *size);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // INIT_UTILS_H
