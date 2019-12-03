#include "thread.h"
#include <Windows.h>

DWORD WINAPI mainLoop(LPVOID voidParameters);

void DebugTask::execute()
{
	std::cout << "Task1 is running";
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

		taskId = m_ThreadPool.m_TaskQueue.jobs[m_ThreadPool.m_TaskQueue.m_Write]->id;
		__int32 m_ReadTemp = m_ThreadPool.m_TaskQueue.m_Read;
		m_ThreadPool.m_TaskQueue.m_Jobs--;
		m_ThreadPool.m_TaskQueue.m_Read = (m_ThreadPool.m_TaskQueue.m_Read + 1) & taskQueue::BATCH_MODULO;		

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
		for (auto i_job : tasks)
		{
			i_job->m_Dependencies = 0;
			m_TaskQueue.jobs.push_back(i_job);
			m_TaskQueue.m_Jobs++;
		}

		for (__int32 i_job = 0; i_job < tasks.size(); i_job++)
		{
			for (__int32 i_permission = 0; i_permission < tasks[i_job]->m_Permisions; i_permission++)
			{
				const __int32 i_first = tasks[i_job]->permissions[i_permission][0];
				const __int32 i_last = i_first + tasks[i_job]->permissions[i_permission][1];

				for (__int32 i_job_permit = i_first; i_job_permit < i_last; i_job_permit++)
				{
					tasks[i_job_permit]->m_Dependencies++;
				}
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

				for (__int32 i_permission = 0; i_permission < tasks[taskId]->m_Permisions; i_permission++)
				{
					const __int32 i_first = tasks[taskId]->permissions[i_permission][0];
					const __int32 i_last = i_first + tasks[taskId]->permissions[i_permission][1];

					for (__int32 i_job_permit = i_first; i_job_permit < i_last; i_job_permit++)
					{
						tasks[i_job_permit]->m_Dependencies--;
					}
				}
				master_thread.m_TasksComplete.m_Jobs--;
			}
		}
		{
			__int32 i_job = 0;
			while ((master_thread.m_TasksReady.m_Jobs < taskQueue::NUM_JOBS_BATCH) && (i_job < tasks.size()))
			{

				if (tasks[i_job]->m_Dependencies == 0)
				{
					master_thread.m_TasksReady.ids[master_thread.m_TasksReady.m_Write] = tasks[i_job]->id;
					master_thread.m_TasksReady.m_Write = (master_thread.m_TasksReady.m_Write + 1) & taskQueue::BATCH_MODULO;
					master_thread.m_TasksReady.m_Jobs++;
					tasks[i_job]->m_Dependencies--;
				}
				i_job++;
			}
		}

		EnterCriticalSection(&m_CriticalSection);
		
		{
			const __int32 nJobs = (((master_thread.m_TasksReady.m_Jobs) < (taskQueue::NUM_JOBS_BATCH - m_TaskQueue.m_Jobs)) ? (master_thread.m_TasksReady.m_Jobs) : (taskQueue::NUM_JOBS_BATCH - m_TaskQueue.m_Jobs));

			/*	for (__int32 i_job = 0; i_job < nJobs; i_job++)
			{
				//const __int32 taskId = master_thread.m_TasksReady.ids[master_thread.m_TasksReady.m_Read];
				//m_TaskQueue.jobs[m_TaskQueue.m_Write]->id = taskId;
				//m_TaskQueue.jobs[m_TaskQueue.m_Write]->execute();
				//	m_ThreadPool.m_TaskQueue.jobs[m_ThreadPool.m_TaskQueue.iWrite].function = jobs_in[taskId].function;
				//	m_ThreadPool.m_TaskQueue.jobs[m_ThreadPool.m_TaskQueue.iWrite].parameters = jobs_in[taskId].parameters;
//				m_TaskQueue.m_Write = (m_TaskQueue.m_Write + 1) & taskQueue::BATCH_MODULO;
				m_TaskQueue.m_Jobs++;
				master_thread.m_TasksReady.m_Read = (master_thread.m_TasksReady.m_Read + 1) & taskQueue::BATCH_MODULO;
				master_thread.m_TasksReady.m_Jobs--;
			} */
		} 

		WakeAllConditionVariable(&m_ConsumerVariable);

		while ((m_TasksComplete.m_Jobs == 0) && isRunning)
		{
			SleepConditionVariableCS(&m_ProducerVariable, &m_CriticalSection, INFINITE);
		}

		{
			const __int32 nJobs = m_TasksComplete.m_Jobs;
			for (__int32 i_job = 0; i_job < nJobs; i_job++)
			{

				master_thread.m_TasksComplete.ids[i_job] = m_TasksComplete.ids[i_job];
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

//RenderTask rt;

//taskmanager.submit(db);
//taskmanager.submit(rt);
