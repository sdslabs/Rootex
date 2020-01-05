#pragma once

#include "common/common.h"

#include <Windows.h>

class ThreadPool;

class Task
{
public:
	__int32 id;
	__int32 m_Dependencies;
	__int32 m_Permisions;
	Vector<__int32> i_Permissions;

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

struct workerParameters
{
	__int32 iThread;
	ThreadPool* m_ThreadPool;
};

struct taskQueue
{
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<Ref<Task>> jobs;
};

struct taskComplete
{
	__int32 m_Jobs;
	Vector<__int32> ids;
};

struct taskReady
{	
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<__int32> ids;
};

struct MasterThread
{	
	taskComplete m_TasksComplete;
	taskReady m_TasksReady;
};

class ThreadPool
{
public:
	enum
	{
		maxWorkerThreads = 8,
	};

	bool isRunning;
	__int32 m_Threads;
	workerParameters m_WorkerParameters[maxWorkerThreads];
	HANDLE handles[maxWorkerThreads];
	CONDITION_VARIABLE m_ConsumerVariable;
	CONDITION_VARIABLE m_ProducerVariable;
	CRITICAL_SECTION m_CriticalSection;

	__int32 m_TasksFinished;
	taskQueue m_TaskQueue;
	taskComplete m_TasksComplete;

	void threadPoolInitialise();

	void threadPoolSubmitTasks(Vector<Ref<Task>>& tasks);

	void threadPoolShutdown();
};
