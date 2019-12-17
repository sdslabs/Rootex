#include "thread.h"
#include <Windows.h>

DWORD WINAPI mainLoop(LPVOID voidParameters);

void DebugTask::execute()
{
	std::cout << "Task1 is running";
}

void RenderTask::execute()
{
	std::cout << "Task2 is running";
}

void ThreadPool::threadPoolInitialise()
{
	SYSTEM_INFO m_SysInfo;
	GetSystemInfo(&m_SysInfo);
	m_Threads = m_SysInfo.dwNumberOfProcessors;

	isRunning = true;

	InitializeConditionVariable(&m_ConsumerVariable);
	InitializeConditionVariable(&m_ProducerVariable);
	InitializeCriticalSection(&m_CriticalSection);

	{
		m_TaskQueue.m_Read = 0;
		m_TaskQueue.m_Write = 0;
		m_TaskQueue.m_Jobs = 0;
		m_TasksFinished = 0;
		m_TasksComplete.m_Jobs = 0;
	}

	for (__int32 iThread = 0; iThread < m_Threads; iThread++)
	{
		m_WorkerParameters[iThread].iThread = iThread;
		m_WorkerParameters[iThread].m_ThreadPool = this;
		handles[iThread] = CreateThread(NULL, 0, mainLoop, &m_WorkerParameters[iThread], 0, 0);
	}
}

DWORD WINAPI mainLoop(LPVOID voidParameters)
{
	const struct workerParameters* parameters = (struct workerParameters*)voidParameters;

	const __int32 iThread = parameters->iThread;
	ThreadPool& m_ThreadPool = *parameters->m_ThreadPool;

	__int32 taskId = -1;
	__int32 increment = 0;

	while (true)
	{
		EnterCriticalSection(&m_ThreadPool.m_CriticalSection);

		m_ThreadPool.m_TasksComplete.ids[m_ThreadPool.m_TasksComplete.m_Jobs] = taskId;
		m_ThreadPool.m_TasksComplete.m_Jobs += increment;
		m_ThreadPool.m_TasksFinished += increment;
		increment = 1;

		WakeAllConditionVariable(&m_ThreadPool.m_ProducerVariable);

		while ((m_ThreadPool.m_TaskQueue.m_Jobs == 0) && m_ThreadPool.isRunning)
		{
			SleepConditionVariableCS(&m_ThreadPool.m_ConsumerVariable, &m_ThreadPool.m_CriticalSection, INFINITE);
		}

		if (m_ThreadPool.isRunning == false)
		{
			LeaveCriticalSection(&m_ThreadPool.m_CriticalSection);
			return 0;
		}

		taskId = m_ThreadPool.m_TaskQueue.jobs[m_ThreadPool.m_TaskQueue.m_Read]->id;
		__int32 m_ReadTemp = m_ThreadPool.m_TaskQueue.m_Read;
		m_ThreadPool.m_TaskQueue.m_Jobs--;
		m_ThreadPool.m_TaskQueue.m_Read = m_ThreadPool.m_TaskQueue.m_Read + 1;
	
		LeaveCriticalSection(&m_ThreadPool.m_CriticalSection);

		m_ThreadPool.m_TaskQueue.jobs[m_ReadTemp]->execute();
	}
	return 0;
}

void ThreadPool::threadPoolSubmitTasks(Vector<Ref<Task>>& tasks)
{
	MasterThread master_thread;
	master_thread.m_TasksComplete.m_Jobs = 0;
	master_thread.m_TasksReady.m_Jobs = 0;
	master_thread.m_TasksReady.m_Read = 0;
	master_thread.m_TasksReady.m_Write = 0;

	bool isTasksRemaining = true;

	m_TasksFinished = 0;

	{
		__int32 idIndex = 0;
		for (auto i_Job : tasks)
		{
			i_Job->m_Dependencies = 0;
			m_TaskQueue.jobs.push_back(i_Job);
			m_TaskQueue.jobs[idIndex]->id = idIndex;
			idIndex++;
			m_TaskQueue.m_Jobs++;
		}

		for (__int32 i_Job = 0; i_Job < tasks.size(); i_Job++)
		{
			for (__int32 i_Permission = 0; i_Permission < tasks[i_Job]->m_Permisions; i_Permission++)
			{
				tasks[tasks[i_Job]->i_Permissions[i_Permission]]->m_Dependencies++;
			}
		}
	}

	while (isTasksRemaining)
	{
		{
			const __int32 nJobs = master_thread.m_TasksComplete.m_Jobs;
			for (__int32 i_job = 0; i_job < nJobs; i_job++)
			{
				__int32 taskId = master_thread.m_TasksComplete.ids[i_job];

				for (__int32 i_Permission = 0; i_Permission < tasks[taskId]->m_Permisions; i_Permission++)
				{
					tasks[tasks[taskId]->i_Permissions[i_Permission]]->m_Dependencies--;
				}
				master_thread.m_TasksComplete.m_Jobs--;
			}
		}
		{
			__int32 i_Job = 0;
			while (i_Job < tasks.size())
			{
				if (tasks[i_Job]->m_Dependencies == 0)
				{
					master_thread.m_TasksReady.ids[master_thread.m_TasksReady.m_Write] = tasks[i_Job]->id;
					master_thread.m_TasksReady.m_Write = (master_thread.m_TasksReady.m_Write + 1);
					master_thread.m_TasksReady.m_Jobs++;
					tasks[i_Job]->m_Dependencies--;
				}
				i_Job++;
			}
		}

		EnterCriticalSection(&m_CriticalSection);

	/*	{
			__int32 nJobs = master_thread.m_TasksReady.m_Jobs;
			m_TaskQueue.m_Write--;
				for (__int32 i_Job = 0; i_Job < nJobs; i_Job++)
			{
				m_TaskQueue.jobs[m_TaskQueue.m_Write]->id = master_thread.m_TasksReady.ids[master_thread.m_TasksReady.m_Read];
				m_TaskQueue.m_Write = (m_TaskQueue.m_Write + 1);
				m_TaskQueue.m_Jobs++;
				master_thread.m_TasksReady.m_Read = (master_thread.m_TasksReady.m_Read + 1);
				master_thread.m_TasksReady.m_Jobs--;
			} 
		} */

		WakeAllConditionVariable(&m_ConsumerVariable);

		while ((m_TasksComplete.m_Jobs == 0) && isRunning)
		{
			SleepConditionVariableCS(&m_ProducerVariable, &m_CriticalSection, INFINITE);
		}

		{
			const __int32 nJobs = m_TasksComplete.m_Jobs;
			for (__int32 i_Job = 0; i_Job < nJobs; i_Job++)
			{

				master_thread.m_TasksComplete.ids[i_Job] = m_TasksComplete.ids[i_Job];
				master_thread.m_TasksComplete.m_Jobs++;
				m_TasksComplete.m_Jobs--;
			}
		}

		isTasksRemaining = m_TasksFinished < tasks.size();

		LeaveCriticalSection(&m_CriticalSection);
	}
}

void ThreadPool::threadPoolShutdown()
{
	EnterCriticalSection(&this->m_CriticalSection);
	{
		isRunning = false;
	}
	LeaveCriticalSection(&this->m_CriticalSection);
	WakeAllConditionVariable(&this->m_ConsumerVariable);
	WaitForMultipleObjects(m_Threads, handles, TRUE, INFINITE);
}
