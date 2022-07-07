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

#include "le_loop.h"
#include "le_epoll.h"


static int TaskNodeCompare(const HashNode *node1, const HashNode *node2)
{
    BaseTask *task1 = HASHMAP_ENTRY(node1, BaseTask, hashNode);
    BaseTask *task2 = HASHMAP_ENTRY(node2, BaseTask, hashNode);
    return (int)task1->taskId.fd - (int)task2->taskId.fd;
}

static int TaskKeyCompare(const HashNode *node, const void *key)
{
    BaseTask *task = HASHMAP_ENTRY(node, BaseTask, hashNode);
    TaskId *taskId = (TaskId *)key;
    return (int)task->taskId.fd - taskId->taskId.fd;
}

static int TaskGetNodeHasCode(const HashNode *node)
{
    BaseTask *task = HASHMAP_ENTRY(node, BaseTask, hashNode);
    return task->taskId.fd;
}

static int TaskGetKeyHasCode(const void *key)
{
    TaskId *taskId = (TaskId *)key;
    return taskId->taskId.fd;
}

static void TaskNodeFree(const HashNode *node)
{
    BaseTask *task = HASHMAP_ENTRY(node, BaseTask, hashNode);
    CloseTask(LE_GetDefaultLoop(), task);
}

static LE_STATUS CreateLoop_(EventLoop **loop, uint32_t maxevents, uint32_t timeout)
{
#ifdef LOOP_EVENT_USE_EPOLL
    LE_STATUS ret = CreateEpollLoop(loop, maxevents, timeout);
    LE_CHECK(ret == LE_SUCCESS, return ret, "Failed to create epoll loop");
#endif
    (*loop)->maxevents = maxevents;
    (*loop)->timeout = timeout;
    (*loop)->stop = 0;
    LoopMutexInit(&(*loop)->mutex);

    HashInfo info = {
        TaskNodeCompare,
        TaskKeyCompare,
        TaskGetNodeHasCode,
        TaskGetKeyHasCode,
        TaskNodeFree,
        128
    };
    return HashMapCreate(&(*loop)->taskMap, &info);
}

LE_STATUS CloseLoop(EventLoop *loop)
{
    if (!loop->stop) {
        return LE_SUCCESS;
    }
    HashMapDestory(loop->taskMap);
    if (loop->close) {
        loop->close(loop);
    }
    return LE_SUCCESS;
}

LE_STATUS ProcessEvent(const EventLoop *loop, int fd, uint32_t oper)
{
    BaseTask *task = GetTaskByFd((EventLoop *)loop, fd);
    if (task != NULL) {
        task->handleEvent((LoopHandle)loop, (TaskHandle)task, oper);
    } else {
        LE_LOGE("ProcessEvent can not find task for %d", fd);
        loop->delEvent(loop, fd, oper);
    }
    return LE_SUCCESS;
}

LE_STATUS AddTask(EventLoop *loop, BaseTask *task)
{
    LoopMutexLock(&loop->mutex);
    HashMapAdd(loop->taskMap, &task->hashNode);
    LoopMutexUnlock(&loop->mutex);
    return LE_SUCCESS;
}

BaseTask *GetTaskByFd(EventLoop *loop, int fd)
{
    BaseTask *task = NULL;
    LoopMutexLock(&loop->mutex);
    TaskId id = {0, {fd}};
    HashNode *node = HashMapGet(loop->taskMap, &id);
    if (node != NULL) {
        task = HASHMAP_ENTRY(node, BaseTask, hashNode);
    }
    LoopMutexUnlock(&loop->mutex);
    return task;
}

void DelTask(EventLoop *loop, BaseTask *task)
{
    loop->delEvent(loop, task->taskId.fd,
        Event_Read | Event_Write | Event_Error | Event_Free | Event_Timeout | Event_Signal);
    LoopMutexLock(&loop->mutex);
    HashMapRemove(loop->taskMap, (TaskId *)task);
    LoopMutexUnlock(&loop->mutex);
    return;
}

static EventLoop *g_defaultLoop = NULL;
LoopHandle LE_GetDefaultLoop(void)
{
    if (g_defaultLoop == NULL) {
        LE_CreateLoop((LoopHandle *)&g_defaultLoop);
    }
    return (LoopHandle)g_defaultLoop;
}

LE_STATUS LE_CreateLoop(LoopHandle *handlle)
{
    EventLoop *loop = NULL;
    LE_STATUS ret = CreateLoop_(&loop, LOOP_MAX_SOCKET, DEFAULT_TIMEOUT);
    *handlle = (LoopHandle)loop;
    return ret;
}

void LE_RunLoop(const LoopHandle handle)
{
    LE_CHECK(handle != NULL, return, "Invalid handle");
    EventLoop *loop = (EventLoop *)handle;
    loop->runLoop(loop);
}

void LE_StopLoop(const LoopHandle handle)
{
    LE_CHECK(handle != NULL, return, "Invalid handle");
    EventLoop *loop = (EventLoop *)handle;
    loop->stop = 1;
}