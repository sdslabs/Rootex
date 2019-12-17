#pragma once

#include "common/common.h"

#include <Windows.h>

class ThreadPool;

class Task
{
public:
	enum
	{
		maxPermissions = 8,
	};

	__int32 id;
	__int32 m_Dependencies;
	__int32 m_Permisions;
	Vector<__int32> i_Permissions;
	//__int32 permissions[maxPermissions][2];

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
	enum
	{
		NUM_JOBS_BATCH = 16,
		BATCH_MODULO = NUM_JOBS_BATCH - 1,
	};

	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<Ref<Task>> jobs;
};

struct taskComplete
{
	enum
	{
		MAX_JOBS = taskQueue::NUM_JOBS_BATCH * 2,
	};

	__int32 m_Jobs;
	//Vector<__int32> ids;
	__int32 ids[MAX_JOBS];
};

struct taskReady
{	
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	//Vector<__int32> ids;
	__int32 ids[taskQueue::NUM_JOBS_BATCH];
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
