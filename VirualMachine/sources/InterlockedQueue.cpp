#include "stdafx.h"
#include "InterlockedQueue.h"
#include "Thread.h"

using namespace Beer;


/*
template <typename T>
void print(const LockFreeQueue<T>& queue, const char_t* txt = "queue")
{
	cout << txt << ": ";
	if(queue.empty()) cout << "[empty]";
	else for(LockFreeQueue<T>::iterator it = queue.begin(); it != queue.end(); it++)
	{
		cout << *it << ", ";
	}
	cout << std::endl;
}

typedef LockFreeQueue<int> IntQueue;
*/