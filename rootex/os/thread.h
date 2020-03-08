#pragma once

#include "common/common.h"

#include <Windows.h>

class ThreadPool;

class Task
{
public:
	__int32 m_ID;
	__int32 m_Dependencies;
	__int32 m_Permisions;
	Vector<__int32> m_Permissions;

	virtual void execute() = 0;
	virtual void undo() {}
};

class DebugTask : public Task
{
protected:
	void execute() override;
};

class RenderTask : public Task
{
protected:
	void execute() override;
};

struct WorkerParameters
{
	__int32 m_Thread;
	ThreadPool* m_ThreadPool;
};

struct TaskQueue
{
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<Ref<Task>> m_QueueJobs;
};

struct TaskComplete
{
	__int32 m_Jobs;
	Vector<__int32> m_IDs;
};

struct TaskReady
{	
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<__int32> m_IDs;
};

struct MasterThread
{	
	TaskComplete m_TasksComplete;
	TaskReady m_TasksReady;
};

class ThreadPool
{
	enum
	{
		MaxWorkerThreads = 12,
	};

	bool m_IsRunning;
	__int32 m_Threads;
	WorkerParameters m_WorkerParameters[MaxWorkerThreads];
	HANDLE m_Handles[MaxWorkerThreads];
	CONDITION_VARIABLE m_ConsumerVariable;
	CONDITION_VARIABLE m_ProducerVariable;
	CRITICAL_SECTION m_CriticalSection;

	__int32 m_TasksFinished;
	TaskQueue m_TaskQueue;
	TaskComplete m_TasksComplete;

	friend DWORD WINAPI MainLoop(LPVOID voidParameters);

	void initialize();
	void shutDown();

public:
	ThreadPool();
	ThreadPool(ThreadPool&) = delete;
	~ThreadPool();
	
	void submit(Vector<Ref<Task>>& tasks);
};
