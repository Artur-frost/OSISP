#include <iostream>
#include <time.h>
#include <thread>
using namespace std;

#if defined (WIN32) || (_WIN64)

#include <windows.h>
#define pthread_t DWORD
#define pthread_create(THREAD_ID_PTR, ATTR, ROUTINE, PARAMS) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ROUTINE,(void*)PARAMS,0,THREAD_ID_PTR)
#define sleep(ms) Sleep(ms)

#else // Linux

#include <pthread.h>
#include <unistd.h>

#endif


class QS
{
public:
	int* c_array;
	int c_size;
	int c_thread_id;
	QS** c_threads;

	QS(int* array, int size, QS** threads, int thread_id)
	{
		c_array = array;
		c_size = size;
		c_threads = threads;
		c_thread_id = thread_id;
		threads[thread_id] = this;
	}

	~QS()
	{
		c_threads[c_thread_id] = nullptr;
	}
};

void qsort(int* array, const unsigned int size)
{
	if (size <= 30)
	{
		// insertion sort algorithm
		for (unsigned int i = 1; i < size; i++) {
			int temp = array[i];
			unsigned int j = i;
			while (j > 0 && temp < array[j - 1]) {
				array[j] = array[j - 1];
				j--;
			}
			array[j] = temp;
		}
	}
	else
	{
		// quick sort algorithm
		int pivot = array[size / 2];
		int* left = array;
		int* right = array + size - 1;
		while (true) {
			while ((left <= right) && (*left < pivot)) left++;
			while ((left <= right) && (*right > pivot)) right--;
			if (left > right) break;
			int temp = *left;
			*left++ = *right;
			*right-- = temp;
		}
		qsort(array, right - array + 1);
		qsort(left, array + size - left);
	}
}

void* qsort_thread(void* obj)
{
	qsort(((QS*)obj)->c_array, ((QS*)obj)->c_size);
	delete ((QS*)obj);
	return nullptr;
}


struct RAND
{
	int* s_array;
	int s_size;

	RAND(int* array, int size)
	{
		s_array = array;
		s_size = size;
	}
};

int RANDOM_THREAD_COUNT = 0;

void* randomize_thread(void* obj)
{
	RAND* r = (RAND*)obj;
	for (int i = 0; i < r->s_size; i++)
		r->s_array[i] = rand();
	delete (RAND*)r;
	RANDOM_THREAD_COUNT--;
	return nullptr;
}


// driver
int main(int argc, char** argv)
{
	char* sz;

	int MAX_ARRAY_ELEMENTS = 2000;
	int MAX_THREADS = thread::hardware_concurrency();

	// parse command line arguments
	for (--argc, ++argv; argc > 0; --argc, ++argv)
	{
		sz = *argv;
		if (*sz != '-')
			break;

		switch (sz[1])
		{
		case 'A':  // array max
			MAX_ARRAY_ELEMENTS = atoi(sz + 2);
			break;

		case 'T':  // thread count
			MAX_THREADS = atoi(sz + 2);
			break;
		}
	}

	if (MAX_THREADS < 1)
		MAX_THREADS = 1;

	cout << "\n\nArray[" << MAX_ARRAY_ELEMENTS << "]\nThreads[" << MAX_THREADS << "]";

	// allocate the array itself
	int* g_array = new int[MAX_ARRAY_ELEMENTS];

	int len = MAX_ARRAY_ELEMENTS / MAX_THREADS;

	// generating random values in array
	srand(clock());

	// create threads for randomizing array
	for (int i = 0, ai = 0; i < MAX_THREADS; i++, ai += len)
	{
		RANDOM_THREAD_COUNT++;
		pthread_t t;
		int size = len + (i == (MAX_THREADS - 1) ? (MAX_ARRAY_ELEMENTS % MAX_THREADS) : 0);
		pthread_create(&t, 0, randomize_thread, new RAND(&g_array[ai], size));
	}

	// join all current randomizing threads
	while (RANDOM_THREAD_COUNT)
		sleep(10);

	cout << "\n\nArray Randomized";

	// allocate memeory for threads
	QS** threads = new QS * [MAX_THREADS];

	clock_t time = clock();

	// create threads for qsort
	for (int i = 0, ai = 0; i < MAX_THREADS; i++, ai += len)
	{
		threads[i] = nullptr;
		pthread_t t;
		int size = len + (i == (MAX_THREADS - 1) ? (MAX_ARRAY_ELEMENTS % MAX_THREADS) : 0);
		pthread_create(&t, 0, qsort_thread, new QS(&g_array[ai], size, threads, i));
	}

	// join all current threads
	for (int i = 0; i < MAX_THREADS;)
	{
		sleep(10);
		if (threads[i])
			continue;
		i++;
	}

	// now that threads are finished, do a final sort on the app's main thread
	if (MAX_THREADS > 1)
		qsort(g_array, MAX_ARRAY_ELEMENTS);

	cout << "\n\nSorted in " << ((clock() - time) / 1000.0L) << " Seconds";

	int last = 0;
	for (int i = 0; i < MAX_ARRAY_ELEMENTS; i++)
	{
		if (g_array[i] < last)
		{
			cout << "\n\nArray Not Sorted";
			return 0;
		}
		last = g_array[i];
	}

	cout << "\n\nArray Sorted";
	if (MAX_ARRAY_ELEMENTS < 50)
		for (int i = 0; i < MAX_ARRAY_ELEMENTS; i++)
			cout << " " << g_array[i];
	cout << "\n";

	delete[]g_array;
	delete[]threads;
}