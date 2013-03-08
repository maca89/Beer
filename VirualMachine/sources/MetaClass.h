#pragma once
#include "prereq.h"
#include "Class.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"


namespace Beer
{
	// TODO
	class MetaClass : public Class
	{
	public:
		static void BEER_CALL init(Thread* thread, StackRef<MetaClass> receiver, StackRef<MetaClass> ret);
		//static void BEER_CALL findMethod(Thread* thread, StackRef<MetaClass> receiver, StackRef<String> selector, StackRef<Method> ret);
	};
};