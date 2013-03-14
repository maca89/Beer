#include "stdafx.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "Method.h"
#include "Array.h"
//#include "PolymorphicInlineCache.h"
#include "GenerationalGC.h"
#include "Heap.h"
#include "Debugger.h"

using namespace Beer;


Thread::Thread(VirtualMachine* vm, GC * gc)
	: mVM(vm), mGC(gc), mTopFrame(NULL), mRootFrame(NULL)
{
}

void Thread::init()
{
	mHeap = mGC->createHeap();

	mRootFrame = allocFrame(250, 0);

	Frame* frame = allocFrame(50, 2);
	frame->stackMoveTop(2); // simulate method & receiver
	mRootFrame->stackPush(frame);
	fetchTopFrame();

	PolymorphicInlineCache* methodCache = PolymorphicInlineCache::from(createInstanceMethodCacheBytes);
	methodCache->clear(CREATE_INSTANCE_CACHE_SIZE);
}

Frame* Thread::allocFrame(uint32 stackSize, uint32 argsCout)
{
	Frame* frame = NULL;
	frame = getHeap()->alloc<Frame>(Frame::FRAME_CHILDREN_COUNT);// + stackSize);
	//frame = new Frame(frameArgsCount, maxStack); // +2: receiver, method

	new(frame) Frame(argsCout, stackSize);
	//frame->setArgumentsCount(argsCout);

	return frame;
}

Frame* Thread::getPreviousFrame()
{
	if(mRootFrame->stackSize() >= 2)
	{
		return mRootFrame->stackTop<Frame>(mRootFrame->stackTopIndex() - 1);
	}
	
	return NULL;
}

Frame* Thread::openFrame()
{
	Frame* oldFrame = getFrame();
	StackRef<Method> method(oldFrame, oldFrame->stackTopIndex());

	uint16 stackSize = 50;
	uint16 paramsCount = 0;
	uint16 returnsCount = 0;

	if(!method.isNull())
	{
		paramsCount = method->getParamsCount();
		returnsCount = method->getReturnsCount();
		stackSize = method->getMaxStack() + 10;
	}

	Frame* newFrame = allocFrame(stackSize, returnsCount + paramsCount + 2); // +2: receiver, method

	// make space for returns
	newFrame->stackMoveTop(returnsCount);

	// copy params
	for(uint16 i = paramsCount; i >= 1; i--) // *BEWARE* index starts at this position to skip the method on stack!
	{
		newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex() - i - 1)); // -1 for receiver
	}
	
	//mVM->getDebugger()->printFrameStack(this, oldFrame);

	newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex() - 1)); // copy receiver
	newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex())); // copy method
	
	mRootFrame->stackPush(newFrame);
	fetchTopFrame();
	return getFrame();
}

void Thread::closeFrame()
{
	Frame* currentFrame = getFrame();
	Frame* previousFrame = getPreviousFrame();

	StackRef<Method> method(currentFrame, -1);

	uint16 paramsCount = 0;
	uint16 returnsCount = 0;
	
	if(!method.isNull())
	{
		paramsCount = method->getParamsCount();
		returnsCount = method->getReturnsCount();
	}

	if(previousFrame)
	{
		// clean previous frame
		previousFrame->stackMoveTop(-(paramsCount + returnsCount + 2));
	
		// push returns on previous frame
		for(int32 i = returnsCount; i >= 1; i--)
		{
			previousFrame->stackPush(currentFrame->stackTop(-paramsCount - i - 2));
		}
	}

	// clean current frame
	currentFrame->stackMoveTop(-(paramsCount + returnsCount + 2));

	mRootFrame->stackPop();
	mTopFrame = previousFrame;
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

void Thread::getClass(StackRef<String> name, StackRef<Class> ret)
{
	ret = mVM->getClass(name);
}

void Thread::getMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
{
	Class::findMethod(this, klass, selector, ret);
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
		frame->stackMoveTop(-1); // pop integerClass

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
	frame->stackMoveTop(-1); // pop floatClass

	Class::setClass(this, ret, floatClass);
	ret->setData(value);

	frame->stackMoveTop(-1); // pop class
}

void Thread::createString(StackRef<String> ret, string value)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> length(frame, frame->stackPush());
	createInteger(length, value.size());

	createString(length, ret);
	frame->stackMoveTop(-1); // pop length
	
	ret->copyData(value.c_str());
}

void Thread::createString(StackRef<Integer> length, StackRef<String> ret)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> stringClass(frame, frame->stackPush());
	getStringClass(stringClass);

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(this, stringClass, propertiesCount);

	ret = getHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (length->getData() + 1)), // +1 for \0
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData())
	);
	ret->size(length->getData());
	Class::setClass(this, ret, stringClass);

	frame->stackMoveTop(-2); // pop class, propertiesCount
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
		frame->stackMoveTop(-1); // pop arrayClass
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
		frame->stackMoveTop(-1); // pop klass
	}

	// set values
	Pair::setFirst(this, ret, first);
	Pair::setSecond(this, ret, second);
}

void Thread::createInstance(StackRef<Class> klass, StackRef<Object> ret)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Method> method(frame, frame->stackPush(NULL));

	// find method
	{
		static Reference<String> selectorRef = String::gTranslate(this, BEER_WIDEN("$Class::createInstance()")); // TODO;
		StackRef<String> selector(frame, frame->stackPush(selectorRef.get()));
		PolymorphicInlineCache* methodCache = PolymorphicInlineCache::from(createInstanceMethodCacheBytes);

		method = methodCache->find(this, *klass, *selector, CREATE_INSTANCE_CACHE_SIZE);
		frame->stackMoveTop(-1); // pop selector
	
		if(method.isNull())
		{
			throw MethodNotFoundException(*klass, *klass, *selectorRef);
		}
	}

	// call method
	{
		StackRef<Object> copiedRet(frame, frame->stackPush());
		klass.push(frame);
		method.push(frame);

		openFrame();
		method->call(this); // pops copied class, copied method

		ret = copiedRet;
		frame->stackMoveTop(-1); // pop copiedRet

		DBG_ASSERT(!ret.isNull(), BEER_WIDEN("No instance created"));
	}

	frame->stackMoveTop(-1); // pop method
}

void Thread::staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(this, klass, propertiesCount);

	ret = getHeap()->alloc(
		staticSize,
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData() + additionalChildrenCount)
	);

	Class::setClass(this, ret, klass);

	getFrame()->stackMoveTop(-1); // pop propertiesCount
}

Class* Thread::getClass(const StackRef<Object>& object)
{
	return mVM->getClassTable()->translate(this, object);
}

void Thread::getClass(StackRef<Object> object, StackRef<Class> ret)
{
	mVM->getClassTable()->translate(this, object, ret);
}