
.. _program_listing_file_rootex_os_thread.h:

Program Listing for File thread.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_os_thread.h>` (``rootex\os\thread.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include <Windows.h>
   
   class ThreadPool;
   
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
   
       void submit(Vector<Ref<Task>>& tasks);
   
       bool isCompleted() const;
       void join() const;
   };
