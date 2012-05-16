#include<iostream>
#include<Windows.h>
using namespace std;
#include<queue>

HANDLE sema;
HANDLE mutex;
HANDLE handle[3];

typedef void (*PTRFUN)(int,int);

struct TaskQueue
{
	PTRFUN taskFun;
	int argv1;
	int argv2;
};

std::queue<TaskQueue *> int_queue;
void add(int a, int b)
{
	cout << a <<" + " << b << " = " << a+b <<endl;
}

void sub(int a, int b)
{
	cout << a <<" - " << b << " = " << a-b <<endl;
}
DWORD WINAPI runThreadProc(LPVOID lpParameter)
{
	TaskQueue *task = NULL;
	while(1)
	{
		if(WaitForSingleObject(mutex,INFINITE) == WAIT_OBJECT_0)
		{
			cout << "wait to be done ..." <<endl;
		}
		ReleaseMutex(mutex);
		if(WaitForSingleObject(sema,INFINITE) == WAIT_OBJECT_0)
		{
			if(WaitForSingleObject(mutex,INFINITE) == WAIT_OBJECT_0)
			{
				task = int_queue.front();
				int_queue.pop();
			}
			ReleaseMutex(mutex);
		}
		if(WaitForSingleObject(mutex,INFINITE) == WAIT_OBJECT_0)
		{
			task->taskFun(task->argv1,task->argv2);
		}
		ReleaseMutex(mutex);
		delete task;
	//	ReleaseSemaphore(sema,1,NULL);
	}
	return 0;
}
void InitThreadPool(int count)
{
	handle[0] = CreateThread(NULL, 0, runThreadProc, NULL, NULL, NULL);
	handle[1] = CreateThread(NULL, 0, runThreadProc, NULL, NULL, NULL);
	handle[2] = CreateThread(NULL, 0, runThreadProc, NULL, NULL, NULL);
}

void addFun(int i)
{
	TaskQueue *node = new TaskQueue;
	if( i == 0)
	{
		node->taskFun = add;
		node->argv1 = 2;
		node->argv2 = 3;
	}
	if( i == 1)
	{
		node->taskFun = sub;
		node->argv1 = 5;
		node->argv2 = 3;
	}
	if(WaitForSingleObject(mutex,INFINITE) == WAIT_OBJECT_0)
	{
		int_queue.push(node);
	}
	ReleaseMutex(mutex);
	ReleaseSemaphore(sema,1,NULL);
}
void main()
{
	sema = CreateSemaphore(NULL,0,100,NULL);
	mutex = CreateMutex(NULL,false,NULL);
	

	InitThreadPool(3);
	for(int i = 0; i < 100; ++i)
	{
		int a = i % 2;
		addFun(a);
	}
	WaitForMultipleObjects(3,handle,true,INFINITE);
}
