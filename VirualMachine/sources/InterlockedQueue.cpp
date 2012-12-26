#include "stdafx.h"
#include "InterlockedQueue.h"
#include "Thread.h"

using namespace Beer;


/*
template <typename T>
void print(const LockFreeQueue<T>& queue, const char* txt = "queue")
{
	std::cout << txt << ": ";
	if(queue.empty()) std::cout << "[empty]";
	else for(LockFreeQueue<T>::iterator it = queue.begin(); it != queue.end(); it++)
	{
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
}

typedef LockFreeQueue<int> IntQueue;
*/