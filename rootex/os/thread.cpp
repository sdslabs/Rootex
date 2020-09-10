#include "thread.h"
#include <Windows.h>

/// The main function which runs on every thread.
DWORD WINAPI MainLoop(LPVOID voidParameters);

Task::Task(const Function<void()>& executionTask)
    : m_ExecutionTask(executionTask)
{
}

void Task::execute()
{
	m_ExecutionTask();
}

void ThreadPool::initialize()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_Threads = sysInfo.dwNumberOfProcessors;

	m_IsRunning = true;

	InitializeConditionVariable(&m_ConsumerVariable);
	InitializeConditionVariable(&m_ProducerVariable);
	InitializeCriticalSection(&m_CriticalSection);

	m_DefaultWorkerParameter.m_Thread = 0;
	m_DefaultWorkerParameter.m_ThreadPool = NULL;

	{
		m_TaskQueue.m_Read = 0;
		m_TaskQueue.m_Write = 0;
		m_TaskQueue.m_Jobs = 0;
		m_TasksFinished = 0;
		m_TasksComplete.m_Jobs = 0;
	}

	for (__int32 iThread = 0; iThread < m_Threads; iThread++)
	{
		m_WorkerParameters.push_back(m_DefaultWorkerParameter);
		m_Handles.push_back(m_DefaultHandle);
	}

	for (__int32 iThread = 0; iThread < m_Threads; iThread++)
	{
		m_WorkerParameters[iThread].m_Thread = iThread;
		m_WorkerParameters[iThread].m_ThreadPool = this;
		m_Handles[iThread] = CreateThread(NULL, 0, MainLoop, &m_WorkerParameters[iThread], 0, 0);
	}
}

DWORD WINAPI MainLoop(LPVOID voidParameters)
{
	const struct WorkerParameters* parameters = (struct WorkerParameters*)voidParameters;

	const __int32 iThread = parameters->m_Thread;
	ThreadPool& threadPool = *parameters->m_ThreadPool;

	__int32 taskId = -1;
	__int32 increment = 0;

	while (true)
	{
		EnterCriticalSection(&threadPool.m_CriticalSection);

		/*if (m_ThreadPool.m_TasksComplete.m_Jobs > 0)
		{
			m_ThreadPool.m_TasksComplete.ids.push_back(taskId);
			m_ThreadPool.m_TasksComplete.m_Jobs += increment;
			m_ThreadPool.m_TasksFinished += increment;
			increment++;
			std::cout << increment;
		}*/

		WakeAllConditionVariable(&threadPool.m_ProducerVariable);

		while ((threadPool.m_TaskQueue.m_Jobs == 0) && threadPool.m_IsRunning)
		{
			SleepConditionVariableCS(&threadPool.m_ConsumerVariable, &threadPool.m_CriticalSection, INFINITE);
		}

		if (threadPool.m_IsRunning == false)
		{
			LeaveCriticalSection(&threadPool.m_CriticalSection);
			return 0;
		}

		taskId = threadPool.m_TaskQueue.m_QueueJobs[threadPool.m_TaskQueue.m_Read]->m_ID;
		__int32 readTemp = threadPool.m_TaskQueue.m_Read;
		threadPool.m_TaskQueue.m_Jobs--;
		threadPool.m_TaskQueue.m_Read++;

		LeaveCriticalSection(&threadPool.m_CriticalSection);

		threadPool.m_TaskQueue.m_QueueJobs[readTemp]->execute();
		threadPool.m_TasksComplete.m_Jobs++;
	}
	return 0;
}

void ThreadPool::submit(Vector<Task*>& tasks)
{
	MasterThread masterThread;
	masterThread.m_TasksComplete.m_Jobs = 0;
	masterThread.m_TasksReady.m_Jobs = 0;
	masterThread.m_TasksReady.m_Read = 0;
	masterThread.m_TasksReady.m_Write = 0;

	bool isTasksRemaining = true;

	m_TasksFinished = 0;

	{
		static __int32 idIndex = 0;
		for (auto iJob : tasks)
		{
			iJob->m_Dependencies = 0;
			m_TaskQueue.m_QueueJobs.push_back(iJob);
			m_TaskQueue.m_QueueJobs[idIndex]->m_ID = idIndex;
			idIndex++;
		}

		/*for (__int32 i_Job = 0; i_Job < tasks.size(); i_Job++)
		{
			for (__int32 i_Permission = 0; i_Permission < tasks[i_Job]->m_Permisions; i_Permission++)
			{
				tasks[tasks[i_Job]->i_Permissions[i_Permission]]->m_Dependencies++;
			}
		}*/
	}

	while (isTasksRemaining)
	{

		/*{
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
		}*/
		{
			__int32 iJob = 0;
			while (iJob < tasks.size())
			{
				if (tasks[iJob]->m_Dependencies == 0)
				{
					masterThread.m_TasksReady.m_IDs.push_back(tasks[iJob]->m_ID);
					masterThread.m_TasksReady.m_Write++;
					masterThread.m_TasksReady.m_Jobs++;
					tasks[iJob]->m_Dependencies--;
				}
				iJob++;
			}
		}

		EnterCriticalSection(&m_CriticalSection);

		{
			__int32 nJobs = masterThread.m_TasksReady.m_Jobs;
			for (__int32 i_Job = 0; i_Job < nJobs; i_Job++)
			{
				m_TaskQueue.m_QueueJobs[m_TaskQueue.m_Write]->m_ID = masterThread.m_TasksReady.m_IDs[masterThread.m_TasksReady.m_Read];
				m_TaskQueue.m_Write++;
				m_TaskQueue.m_Jobs++;
				m_TasksFinished++;
				masterThread.m_TasksReady.m_Read++;
				masterThread.m_TasksReady.m_Jobs--;
			}
		}

		WakeAllConditionVariable(&m_ConsumerVariable);

		while ((m_TasksComplete.m_Jobs == 0) && m_IsRunning)
		{
			SleepConditionVariableCS(&m_ProducerVariable, &m_CriticalSection, INFINITE);
		}

		/*{
			const __int32 nJobs = m_TasksComplete.m_Jobs;
			for (__int32 i_Job = 0; i_Job < nJobs; i_Job++)
			{

				master_thread.m_TasksComplete.ids[i_Job] = m_TasksComplete.ids[i_Job];
				master_thread.m_TasksComplete.m_Jobs++;
				m_TasksComplete.m_Jobs--;
			}
		}*/

		isTasksRemaining = m_TasksFinished < tasks.size();

		LeaveCriticalSection(&m_CriticalSection);
	}
}

bool ThreadPool::isCompleted()
{
	return m_TaskQueue.m_Jobs == 0;
}

void ThreadPool::shutDown()
{
	EnterCriticalSection(&this->m_CriticalSection);
	{
		m_IsRunning = false;
	}
	LeaveCriticalSection(&this->m_CriticalSection);
	WakeAllConditionVariable(&this->m_ConsumerVariable);
	WaitForMultipleObjects(m_Threads, m_Handles.data(), TRUE, INFINITE);
}
