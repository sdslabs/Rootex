#pragma once

#include "common/common.h"

#include <Windows.h>

/// Interface for spawning and maintenance of threads.
class ThreadPool;

/// Defines jobs to be run on threads.
class Task
{
public:
	__int32 m_ID;
	__int32 m_Dependencies;
	Vector<__int32> m_Permissions;
	Function<void()> m_ExecutionTask;

	Task(const Function<void()>& executionTask);
	Task(const Task&) = default;
	~Task() = default;

	void execute();
};

/// Worker thread parameters.
struct WorkerParameters
{
	__int32 m_Thread;
	ThreadPool* m_ThreadPool;
};

/// A queue of jobs.
struct TaskQueue
{
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<Ref<Task>> m_QueueJobs;
};

/// Contains the information of completed jobs.
struct TaskComplete
{
	__int32 m_Jobs;
	Vector<__int32> m_IDs;
};

/// Contains information of jobs ready for execution.
struct TaskReady
{
	__int32 m_Jobs;
	unsigned __int32 m_Write;
	unsigned __int32 m_Read;
	Vector<__int32> m_IDs;
};

/// Master thread to communicate with and maintain worker threads.
struct MasterThread
{
	TaskComplete m_TasksComplete;
	TaskReady m_TasksReady;
};

class ThreadPool
{
	bool m_IsRunning;
	__int32 m_Threads;
	Vector<WorkerParameters> m_WorkerParameters;
	WorkerParameters m_DefaultWorkerParameter;
	Vector<HANDLE> m_Handles;
	HANDLE m_DefaultHandle = 0;
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

	/// To submit a job to the jobs queue.
	void submit(Vector<Ref<Task>>& tasks);

	/// Returns true if all tasks have been completed
	bool isCompleted() const;
	/// Returns when all the tasks have been completed
	void join() const;
};
