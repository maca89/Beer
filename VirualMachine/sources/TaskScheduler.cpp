#include "stdafx.h"
#include "TaskScheduler.h"

using namespace Beer;


TaskScheduler::TaskScheduler()
{
}

TaskScheduler::~TaskScheduler()
{
}

void TaskScheduler::contextSwitch()
{
}

void TaskScheduler::pauseAll()
{
}

void TaskScheduler::resumeAll()
{
}

void TaskScheduler::schedule(Thread* thread, StackRef<Task> task)
{
}

void TaskScheduler::wait(Thread* thread, StackRef<Task> who, StackRef<Task> whatFor)
{
}
