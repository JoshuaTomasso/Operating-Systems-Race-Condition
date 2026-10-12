///////////////////////////////////////////////////////////////////////////////////
// TODO: #include any files need
///////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

// Include file and line numbers for memory leak detection for visual studio in debug mode
#if defined _MSC_VER && defined _DEBUG
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
	#define ENABLE_LEAK_DETECTION()
#endif

struct ThreadStruct
{
	// ID of the thread
	int id;
	// Length of the shared string
	int sharedStringLength;
	// Number of strings a single thread will generate
	int numberOfStringsToGenerate;
	// Shared string that will be generate in each thread. This memory is shared among all threads.
	char *sharedString;
	
	///////////////////////////////////////////////////////////////////////////////////
	// TODO: Add any extra variables needed by the threads here
	///////////////////////////////////////////////////////////////////////////////////	

	std::mutex* sharedMutex;
	std::condition_variable* condVar;
	int* currentThreadId;
	int runType;
	int threadCount;
};

///////////////////////////////////////////////////////////////////////////////////////////
// Prompts the user to press enter and waits for user input
///////////////////////////////////////////////////////////////////////////////////////////
void Pause()
{
	printf("Press enter to continue\n");
	getchar();
}

///////////////////////////////////////////////////////////////////////////////////
// Entry point for worker threads. 
//
// Arguments:
//   threadData - Pointer to per-thread data for this thread.
///////////////////////////////////////////////////////////////////////////////////
void ThreadEntryPoint(ThreadStruct *threadData)
{
	///////////////////////////////////////////////////////////////////////////////////
	// TODO: Add code to this function to make it run according to the run type.
	//		 However do NOT duplicate the following code.
	///////////////////////////////////////////////////////////////////////////////////	

	std::unique_lock<std::mutex> lock;

	if (threadData->runType == 3) 
	{
		lock = std::unique_lock<std::mutex>(*threadData->sharedMutex);
		threadData->condVar->wait(lock, [threadData]() {
			return *threadData->currentThreadId == threadData->id;
			});
	}
	else if (threadData->runType == 2) 
	{
		lock = std::unique_lock<std::mutex>(*threadData->sharedMutex);
	}

	for (int i = 0; i < threadData->numberOfStringsToGenerate; i++)
	{
		if (threadData->runType == 1)
		{
			lock = std::unique_lock<std::mutex>(*threadData->sharedMutex);
		}

		for (int j = 0; j < threadData->sharedStringLength; j++) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			threadData->sharedString[j] = 'A' + threadData->id;
		}
		printf("Thread %d: %s\n", threadData->id, threadData->sharedString);

		if (threadData->runType == 1) 
		{
			lock.unlock();
		}

		if (threadData->runType != 3)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	if (threadData->runType == 3)
	{
		*threadData->currentThreadId = (*threadData->currentThreadId + 1) % threadData->threadCount;
		threadData->condVar->notify_all();
	}

	///////////////////////////////////////////////////////////////////////////////////
}

int main(int argc, char** argv)
{
	ENABLE_LEAK_DETECTION();

	int threadCount = 0;
	int sharedStringLength = 0;
	int numberOfStringsToGenerate = 0;
	int runType = 0;
	char *sharedString = nullptr;
	ThreadStruct *perThreadData = nullptr;

	///////////////////////////////////////////////////////////////////////////////////
	// TODO:: Handle the runType command line argument.
	//
	//		  The following code already handles the first 4 arguments by ignoring
	//		  the first argument (which is just the name of the program) and reading
	//		  in the next 3 (threadCount, sharedStringLength, and numberOfStringsToGenerate).
	//
	//		  You will need to add code to this section to read in the final command line
	//		  argument (the runType). Once this is done you'll need to adjust the following
	//		  if check to expect one more argument. 
	///////////////////////////////////////////////////////////////////////////////////	


	if (argc != 5)
	{
		fprintf(stderr, "Error: missing or incorrect command line arguments\n\n"); 
		fprintf(stderr, "Usage: RaceCondition threadCount sharedStringLength numberOfStringsToGenerate runType\n\n");
		fprintf(stderr, "Arguments:\n");
		fprintf(stderr, "    threadCount                  Number of threads to create.\n");
		fprintf(stderr, "    sharedStringLength           Length of string to generate.\n");
		fprintf(stderr, "    numberOfStringsToGenerate    Number of strings to generate per thread.\n");
		fprintf(stderr, "    runType                      The run type.\n\n");
		Pause();
		return 1;
	}


	threadCount = atoi(argv[1]);
	sharedStringLength = atoi(argv[2]);
	numberOfStringsToGenerate = atoi(argv[3]);
	runType = atoi(argv[4]);

	if(threadCount < 0 || sharedStringLength < 0 || numberOfStringsToGenerate < 0 || runType < 0)
	{
		fprintf(stderr, "Error: All arguments must be positive integer values.\n");
		Pause();
		return 1;
	}

	printf("%d thread(s), string sharedStringLength %d, %d iterations, %d runType\n",
		threadCount, sharedStringLength, numberOfStringsToGenerate, runType);
	
	sharedString = new char[sharedStringLength + 1];
	memset(sharedString, 0, sharedStringLength + 1);
	perThreadData = new ThreadStruct[threadCount];
	
	///////////////////////////////////////////////////////////////////////////////////
	// TODO:: You will need a container to store the thread class objects. It is up to you
	//   to decide how you want to store the threads.
	///////////////////////////////////////////////////////////////////////////////////	

	std::vector<std::thread> threads;
	threads.reserve(threadCount);

	std::mutex* sharedMutex = nullptr;
	std::condition_variable* condVar = nullptr;
	int* currentThreadId = nullptr;

	if (runType > 0)
	{
		sharedMutex = new std::mutex();

		if (runType == 3)
		{
			condVar = new std::condition_variable();
			currentThreadId = new int(0);
		}
	}
	
	// NOTE: Do NOT change this for loop header
	for (int i = threadCount - 1; i >= 0; i--)
	{
		perThreadData[i].id = i;
		perThreadData[i].sharedStringLength = sharedStringLength;
		perThreadData[i].numberOfStringsToGenerate = numberOfStringsToGenerate;
		perThreadData[i].sharedString = sharedString;

		///////////////////////////////////////////////////////////////////////////////////
		// TODO:: Setup any additional variables in perThreadData and start (create) the threads.
		//			MUST be done in this for loop.
		///////////////////////////////////////////////////////////////////////////////////

		perThreadData[i].runType = runType;
		perThreadData[i].sharedMutex = sharedMutex;
		perThreadData[i].condVar = condVar;
		perThreadData[i].currentThreadId = currentThreadId;
		perThreadData[i].threadCount = threadCount;

		threads.push_back(std::thread(ThreadEntryPoint, &perThreadData[i]));

	}


	///////////////////////////////////////////////////////////////////////////////////
	// TODO:: Wait for all of the threads to finish. Since we are using
	//   Joinable threads we must Join each one. Joining a thread will cause
	//   the calling thread (main in this case) to block until the thread being
	//   joined has completed executing.
	///////////////////////////////////////////////////////////////////////////////////	

	for (auto& thread : threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	Pause();
	
	///////////////////////////////////////////////////////////////////////////////////
	// TODO: Clean up
	///////////////////////////////////////////////////////////////////////////////////
	
	delete[] sharedString;
	delete[] perThreadData;
	delete sharedMutex;
	delete condVar;
	delete currentThreadId;

	return 0;
}
