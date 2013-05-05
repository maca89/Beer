#include "stdafx.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "Method.h"
#include "Array.h"
#include "Heap.h"
#include "Debugger.h"
#include "ClassFileLoader.h"
#include "Pool.h"
#include "PolymorphicCache.h"
#include "Pair.h"

using namespace Beer;


Thread::Thread(uint16 threadId, VirtualMachine* vm, GenerationalGC * gc)
	: mVM(vm), mGC(gc), mContext(NULL), mExecutionPaused(true)
{
	mThreadId = threadId;
}

void Thread::init()
{
	mHeap = mGC->createHeap();

	// just a temporary solution, TODO
	//mTemporaryContext.init(mHeap, getVM()->getFrameClass());
	//setContext(&mTemporaryContext);

}

bool Thread::trampoline()
{
	//mVM->getDebugger()->started();

	bool hf = hasFrame();
	bool iep = isExecutionPaused();
		
	while(hasFrame() && !isExecutionPaused())
	{
		Frame* frame = getFrame();
		StackRef<Method> method(frame, Frame::INDEX_METHOD);
		
		if(method.isNull())
		{
			closeFrame();
			continue;
		}

		try
		{
		#ifdef BEER_DEBUG_MODE
			if(mVM->getDebugger()->isEnabled()) mVM->getDebugger()->step(this, frame);
		#endif // BEER_DEBUG_MODE

		#ifdef BEER_MEASURE_PERFORMANCE
			MiliTimer timer;
			timer.start();
			MethodReflection* oldMethod = method;
		#endif // BEER_MEASURE_PERFORMANCE

			method->invoke(this);

		#ifdef BEER_MEASURE_PERFORMANCE
			oldMethod->addTimeSpent(timer.stop());
		#endif // BEER_MEASURE_PERFORMANCE
		}
		catch(Exception& ex)
		{
			if(!mVM->getDebugger()->catchException(this, frame, ex))
			{
				break;//throw ex;
			}
		}
	}

	//mVM->getDebugger()->ended();
	return !hasFrame();
}

void Thread::loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile)
{
	loader->loadClasses(this, classFile);
}

void Thread::getObjectClass(StackRef<Class> ret)
{
	ret = mVM->getObjectClass();
}

void Thread::getIntegerClass(StackRef<Class> ret)
{
	ret = mVM->getIntegerClass();
}

void Thread::getFloatClass(StackRef<Class> ret)
{
	ret = mVM->getFloatClass();
}

void Thread::getStringClass(StackRef<Class> ret)
{
	ret = mVM->getStringClass();
}

void Thread::getArrayClass(StackRef<Class> ret)
{
	ret = mVM->getArrayClass();
}

void Thread::getPairClass(StackRef<Class> ret)
{
	ret = mVM->getPairClass();
}

void Thread::getPoolClass(StackRef<Class> ret)
{
	ret = mVM->getPoolClass();
}

void Thread::findClass(StackRef<String> name, StackRef<Class> ret)
{
	ret = mVM->findClass(name);
}

void Thread::findMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
{
	// TODO: global polymorphic cache
	ret = klass->findVirtualMethod(*selector);
}

Integer* Thread::createConstantInteger(Integer::IntegerData value)
{
	if(Integer::canBeInlineValue(value))
	{
		return Integer::makeInlineValue(value);
	}

	Integer* ret = static_cast<Integer*>(staticCreateConstant(
		mVM->getIntegerClass(), 
		sizeof(Integer)
	));

	ret->setNonInlineValue(value);

	return ret;
}

Float* Thread::createConstantFloat(Float::FloatData value)
{
	Float* ret = static_cast<Float*>(staticCreateConstant(
		mVM->getFloatClass(), 
		sizeof(Float)
	));

	ret->setData(value);

	return ret;
}

String* Thread::createConstantString(String::LengthData length)
{
	String* ret = static_cast<String*>(staticCreateConstant(
		mVM->getStringClass(), 
		sizeof(String) + sizeof(String::CharacterData) * (static_cast<int32>(length) + 1) // +1 for \0
	));

	ret->size(length);

	return ret;
}

String* Thread::createConstantString(const String::CharacterData* cstring)
{
	String::LengthData length = strlen(cstring);
	String* str = createConstantString(length);
	str->copyData(cstring);
	return str;
}

void Thread::createInteger(StackRef<Integer> ret, Integer::IntegerData value)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	if(Integer::canBeInlineValue(value))
	{
		ret = Integer::makeInlineValue(value);
	}
	else
	{
		StackRef<Class> integerClass(frame, frame->stackPush(NULL));
		getIntegerClass(integerClass);

		staticCreateObject(integerClass, ret, sizeof(Integer));
		frame->stackPop(); // pop integerClass

		ret->setNonInlineValue(value);
	}
}

void Thread::createFloat(StackRef<Float> ret, Float::FloatData value)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> floatClass(frame, frame->stackPush());
	getFloatClass(floatClass);

	floatClass.push(frame);

	staticCreateObject(floatClass, ret, sizeof(Float));
	frame->stackPop(); // pop floatClass

	Object::setType(this, ret, floatClass);
	ret->setData(value);

	frame->stackPop(); // pop class
}

void Thread::createString(StackRef<String> ret, string value)
{
	createString(ret, value.size());
	ret->copyData(value.c_str());
}

void Thread::createString(StackRef<Integer> length, StackRef<String> ret)
{
	createString(ret, length->getData());
}

void Thread::createString(StackRef<String> ret, Integer::IntegerData length)
{
	Class* stringClass = getVM()->getStringClass();

	ret = getHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (length + 1)), // +1 for \0
		Object::OBJECT_CHILDREN_COUNT + stringClass->getPropertiesCount()
	);
	ret->size(length);
	ret->setType(stringClass);
}

void Thread::createArray(StackRef<Integer> length, StackRef<Array> ret)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	// create array
	{
		StackRef<Class> arrayClass(frame, frame->stackPush());
		getArrayClass(arrayClass);

		staticCreateObject(arrayClass, ret.staticCast<Object>(), sizeof(Array), static_cast<int32>(length->getData()));
		frame->stackPop(); // pop arrayClass
	}

	// init
	ret->setSize(length->getData());
	ret->setItemClass(NULL); // TODO
}

void Thread::createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	// create pair
	{
		StackRef<Class> klass(frame, frame->stackPush());
		getPairClass(klass);

		staticCreateObject(klass, ret.staticCast<Object>(), sizeof(Pair));
		frame->stackPop(); // pop klass
	}

	// set values
	Pair::setFirst(this, ret, first);
	Pair::setSecond(this, ret, second);
}

void Thread::createPool(StackRef<Pool> ret, uint16 length)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> poolClass(frame, frame->stackPush());
	getPoolClass(poolClass);

	staticCreateObject(poolClass, ret, sizeof(Pool), length);

	Pool::setLength(this, ret, length);
	Pool::clear(this, ret);

	frame->stackPop(); // pop poolClass
}

void Thread::createInstance(StackRef<Class> klass, StackRef<Object> ret)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	Method* method = klass->getMethod(Class::METHOD_SLOT_CREATE_INSTANCE);
	DBG_ASSERT(method, BEER_WIDEN("Method is NULL"));

	// call method
	{
		StackRef<Object> copiedRet(frame, frame->stackPush());
		frame->stackPush(*klass);
		frame->stackPush(method);

		openFrame();
		method->invoke(this); // pops copied class, copied method

		ret = copiedRet;
		frame->stackPop(); // pop copiedRet

		DBG_ASSERT(!ret.isNull(), BEER_WIDEN("No instance created"));
	}
}

Object* Thread::staticCreateConstant(Class* klass, int32 staticSize, int32 additionalChildrenCount)
{
	Object* ret = getPermanentHeap()->alloc(
		staticSize,
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount() + additionalChildrenCount)
	);

	ret->setType(klass);

	return ret;
}

void Thread::staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount)
{
	ret = getHeap()->alloc(
		staticSize,
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount() + additionalChildrenCount)
	);

	ret->setType(*klass);
}

Class* Thread::getType(Object* object)
{
	return mVM->getClassTable()->translate(this, object);
}

void Thread::getType(StackRef<Object> object, StackRef<Class> ret)
{
	mVM->getClassTable()->translate(this, object, ret);
}

void Thread::createPolycache(StackRef<PolymorphicCache> ret, uint16 length)
{
	ret = getHeap()->alloc<PolymorphicCache>(PolymorphicCache::POLYCACHE_CHILDREN_COUNT + length * 2);

	new(*ret) PolymorphicCache(); // ctor

	ret->setLength(length);
	ret->clear();
}