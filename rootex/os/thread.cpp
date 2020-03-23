#include "thread.h"
#include <Windows.h>

#include "framework/systems/render_system.h"
#include "framework/systems/physics_system.h"

DWORD WINAPI MainLoop(LPVOID voidParameters);
DWORD WINAPI DoubleBuffer(LPVOID voidParameters);

void DebugTask::execute()
{
	int i = 10;
	while (i-- > 0)
	{
		OS::Print("Task1 is running\n");
	}
}

void RenderTask::execute()
{
	int i = 10;
	while (i-- > 0)
	{
		OS::Print("Task2 is running\n");
	}
}

void ThreadPool::initialize()
{
	SYSTEM_INFO m_SysInfo;
	GetSystemInfo(&m_SysInfo);
	m_Threads = m_SysInfo.dwNumberOfProcessors;

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

	for (__int32 iThread = 0; iThread < 1; iThread++)
	{
		m_WorkerParameters[iThread].m_Thread = iThread;
		m_WorkerParameters[iThread].m_ThreadPool = NULL;
		m_Handles[iThread] = CreateThread(NULL, 0, DoubleBuffer, &m_WorkerParameters[iThread], 0, 0);
	}

	for (__int32 iThread = 2; iThread < m_Threads; iThread++)
	{
		m_WorkerParameters[iThread].m_Thread = iThread;
		m_WorkerParameters[iThread].m_ThreadPool = this;
		m_Handles[iThread] = CreateThread(NULL, 0, MainLoop, &m_WorkerParameters[iThread], 0, 0);
	}
}

DWORD WINAPI DoubleBuffer(LPVOID voidParameters)
{
	const struct WorkerParameters* parameters = (struct WorkerParameters*)voidParameters;

	const __int32 iThread = parameters->m_Thread;
	//ThreadPool& m_ThreadPool = *parameters->m_ThreadPool;

	if (iThread == 0)
	{
		while (true)
		{
			PhysicsSystem::GetSingleton()->phsicsTestFunction();
		}
	}
	else if (iThread == 1)
	{
		while (true)
		{
			RenderSystem::GetSingleton()->render();
		}
	}

	return 0;
}

DWORD WINAPI MainLoop(LPVOID voidParameters)
{
	const struct WorkerParameters* parameters = (struct WorkerParameters*)voidParameters;

	const __int32 iThread = parameters->m_Thread;
	ThreadPool& m_ThreadPool = *parameters->m_ThreadPool;

	__int32 taskId = -1;
	__int32 increment = 0;

	while (true)
	{
		EnterCriticalSection(&m_ThreadPool.m_CriticalSection);

		/*if (m_ThreadPool.m_TasksComplete.m_Jobs > 0)
		{
			m_ThreadPool.m_TasksComplete.ids.push_back(taskId);
			m_ThreadPool.m_TasksComplete.m_Jobs += increment;
			m_ThreadPool.m_TasksFinished += increment;
			increment++;
			std::cout << increment;
		}*/

		WakeAllConditionVariable(&m_ThreadPool.m_ProducerVariable);

		while ((m_ThreadPool.m_TaskQueue.m_Jobs == 0) && m_ThreadPool.m_IsRunning)
		{
			SleepConditionVariableCS(&m_ThreadPool.m_ConsumerVariable, &m_ThreadPool.m_CriticalSection, INFINITE);
		}

		if (m_ThreadPool.m_IsRunning == false)
		{
			LeaveCriticalSection(&m_ThreadPool.m_CriticalSection);
			return 0;
		}

		taskId = m_ThreadPool.m_TaskQueue.m_QueueJobs[m_ThreadPool.m_TaskQueue.m_Read]->m_ID;
		__int32 m_ReadTemp = m_ThreadPool.m_TaskQueue.m_Read;
		m_ThreadPool.m_TaskQueue.m_Jobs--;
		m_ThreadPool.m_TaskQueue.m_Read++;

		LeaveCriticalSection(&m_ThreadPool.m_CriticalSection);

		m_ThreadPool.m_TaskQueue.m_QueueJobs[m_ReadTemp]->execute();
		m_ThreadPool.m_TasksComplete.m_Jobs++;
	}
	return 0;
}

ThreadPool::ThreadPool()
{
	initialize();
}

ThreadPool::~ThreadPool()
{
	shutDown();
}

void ThreadPool::submit(Vector<Ref<Task>>& tasks)
{
	MasterThread master_thread;
	master_thread.m_TasksComplete.m_Jobs = 0;
	master_thread.m_TasksReady.m_Jobs = 0;
	master_thread.m_TasksReady.m_Read = 0;
	master_thread.m_TasksReady.m_Write = 0;

	bool isTasksRemaining = true;

	m_TasksFinished = 0;

	{
		static __int32 idIndex = 0;
		for (auto i_Job : tasks)
		{
			i_Job->m_Dependencies = 0;
			m_TaskQueue.m_QueueJobs.push_back(i_Job);
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
			__int32 i_Job = 0;
			while (i_Job < tasks.size())
			{
				if (tasks[i_Job]->m_Dependencies == 0)
				{
					master_thread.m_TasksReady.m_IDs.push_back(tasks[i_Job]->m_ID);
					master_thread.m_TasksReady.m_Write++;
					master_thread.m_TasksReady.m_Jobs++;
					tasks[i_Job]->m_Dependencies--;
				}
				i_Job++;
			}
		}

		EnterCriticalSection(&m_CriticalSection);

		{
			__int32 nJobs = master_thread.m_TasksReady.m_Jobs;
			for (__int32 i_Job = 0; i_Job < nJobs; i_Job++)
			{
				m_TaskQueue.m_QueueJobs[m_TaskQueue.m_Write]->m_ID = master_thread.m_TasksReady.m_IDs[master_thread.m_TasksReady.m_Read];
				m_TaskQueue.m_Write++;
				m_TaskQueue.m_Jobs++;
				m_TasksFinished++;
				master_thread.m_TasksReady.m_Read++;
				master_thread.m_TasksReady.m_Jobs--;
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
