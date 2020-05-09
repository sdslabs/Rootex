Multithreading
==============

Rootex engine is multithreading ready. Every Rootex application (:ref:`Class Application`) has a pool of threads, called simply a threadpool in common CS language.

Rootex uses the concept of Worker threads, a.k.a. Job Based multithreading.

At startup, Rootex' threadpool manager (:ref:`Class ThreadPool`) queries the CPU and returns the number of logical CPU cores in the system. The threadpool allocates the same number of threads and uses one of them to be the master thread that distributes "jobs" to different threads. Jobs are implemented as simple overriden virtual functions of :ref:`Class Task`.

During testing Rootex was run simply as a single threaded engine. As time went on, certain functions of Rootex were run in separate threads in a controlled multithreading environment.
