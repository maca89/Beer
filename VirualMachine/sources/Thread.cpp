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

#if defined(BEER_STACK_DEBUGGING)
#define BEER_ASSERT_STACK_START() int __startSize = frame->stackSize();
#define BEER_ASSERT_STACK_END(params) DBG_ASSERT(__startSize - params == frame->stackSize(), BEER_WIDEN("Stack corrupted"));
#define ASSERT_STACK_PARAMS_2(p1, p2) DBG_ASSERT(p1.getIndex() - 1 == p2.getIndex(), BEER_WIDEN("Given parameters have wrong indices"));
#define ASSERT_STACK_PARAMS_3(p1, p2, p3) DBG_ASSERT(p1.getIndex() - 2 == p3.getIndex() && p2.getIndex() - 1 == p3.getIndex(), BEER_WIDEN("Given parameters have wrong indices"));
#define ASSERT_STACK_PARAMS_4(p1, p2, p3, p4) DBG_ASSERT(p1.getIndex() - 3 == p4.getIndex() && p2.getIndex() - 2 == p4.getIndex() && p3.getIndex() - 1 == p4.getIndex(), BEER_WIDEN("Given parameters have wrong indices"));
#else
#define BEER_ASSERT_STACK_START()
#define BEER_ASSERT_STACK_END(params)
#define ASSERT_STACK_PARAMS_2(p1, p2)
#define ASSERT_STACK_PARAMS_3(p1, p2, p3)
#define ASSERT_STACK_PARAMS_4(p1, p2, p3, p4)
#endif // BEER_STACK_DEBUGGING


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
	Frame* frame = getFrame();
	ASSERT_STACK_PARAMS_2(name, ret);
	BEER_ASSERT_STACK_START();
	ret = mVM->getClass(*name);
	frame->stackMoveTop(-1); // pop name
	BEER_ASSERT_STACK_END(1);
}

void Thread::getMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
{
	Frame* frame = getFrame();
	ASSERT_STACK_PARAMS_3(klass, selector, ret);
	BEER_ASSERT_STACK_START();

	Class::findMethod(this, klass, selector, ret);

	frame->stackMoveTop(-2); // pop class & selector
	BEER_ASSERT_STACK_END(2);
}

void Thread::createInteger(StackRef<Integer> ret, Integer::IntegerData value)
{
	Frame* frame = getFrame();
	BEER_ASSERT_STACK_START();
	if(Integer::canBeInlineValue(value))
	{
		ret = Integer::makeInlineValue(value);
	}
	else
	{
		StackRef<Class> integerClass(frame, frame->stackPush(NULL));
		getIntegerClass(integerClass);

		staticCreateObject(integerClass, ret.staticCast<Object>(), sizeof(Integer)); // pops class
		ret->setNonInlineValue(value);
	}
	BEER_ASSERT_STACK_END(0);
}

void Thread::createFloat(StackRef<Float> ret, Float::FloatData value)
{
	Frame* frame = getFrame();
	BEER_ASSERT_STACK_START();

	StackRef<Class> floatClass(frame, frame->stackPush());
	getFloatClass(floatClass);

	floatClass.push(frame);

	staticCreateObject(floatClass, ret.staticCast<Object>(), sizeof(Float)); // pops copied class

	Class::setClass(this, ret, floatClass);
	ret->setData(value);

	frame->stackMoveTop(-1); // pop class

	BEER_ASSERT_STACK_END(0);
}

void Thread::createString(StackRef<String> ret, string value)
{
	Frame* frame = getFrame();
	BEER_ASSERT_STACK_START();
	StackRef<Integer> length(frame, frame->stackPush());
	createInteger(length, value.size());

	createString(length, ret); // pops length
	
	ret->copyData(value.c_str());
	BEER_ASSERT_STACK_END(0);
}

void Thread::createString(StackRef<Integer> length, StackRef<String> ret)
{
	Frame* frame = getFrame();
	// no need for ASSERT_STACK_PARAMS
	BEER_ASSERT_STACK_START();

	StackRef<Class> stringClass(frame, frame->stackPush());
	getStringClass(stringClass);

	ret = getHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (length->getData() + 1)), // +1 for \0
		Object::OBJECT_CHILDREN_COUNT + stringClass->getPropertiesCount()
	);
	ret->size(length->getData());
	Class::setClass(this, ret, stringClass);

	frame->stackMoveTop(-2); // pop length & class
	BEER_ASSERT_STACK_END(1);
}

void Thread::createArray(StackRef<Integer> length, StackRef<Array> ret)
{
	Frame* frame = getFrame();
	ASSERT_STACK_PARAMS_2(length, ret);
	BEER_ASSERT_STACK_START();

	StackRef<Class> arrayClass(frame, frame->stackPush());
	getArrayClass(arrayClass);

	staticCreateObject(arrayClass, ret.staticCast<Object>(), sizeof(Array), static_cast<int32>(length->getData())); // pops class

	ret->setSize(length->getData());
	ret->setItemClass(NULL); // TODO

	frame->stackMoveTop(-1); // pop length
	BEER_ASSERT_STACK_END(1);
}

void Thread::createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret)
{
	Frame* frame = getFrame();
	ASSERT_STACK_PARAMS_3(first, second, ret);
	BEER_ASSERT_STACK_START();

	StackRef<Class> klass(frame, frame->stackPush());
	getPairClass(klass);

	staticCreateObject(klass, ret.staticCast<Object>(), sizeof(Pair)); // pops class

	Pair::setFirst(this, ret, first);
	Pair::setSecond(this, ret, second);

	frame->stackMoveTop(-2);
	BEER_ASSERT_STACK_END(2);
}

void Thread::createInstance(StackRef<Class> klass, StackRef<Object> ret)
{
	Frame* frame = getFrame();
	ASSERT_STACK_PARAMS_2(klass, ret);
	BEER_ASSERT_STACK_START();
	StackRef<Method> method(frame, frame->stackPush(NULL));

	// find method
	{
		static Reference<String> selectorRef = String::gTranslate(this, BEER_WIDEN("$Class::createInstance()")); // TODO;
		StackRef<String> selector(frame, frame->stackPush(selectorRef.get()));
		String* rawSelector = *selectorRef;//

		PolymorphicInlineCache* methodCache = PolymorphicInlineCache::from(createInstanceMethodCacheBytes);

		method = methodCache->find(this, *klass, *selector, CREATE_INSTANCE_CACHE_SIZE);
		//getMethod(frame, klass.copy(), selector, method); // pops selector & copied class
		frame->stackMoveTop(-1); // pop selector
	
		if(method.isNull())
		{
			Class* rawKlass = *klass;
			String* rawSelector = *selectorRef;
			
			throw MethodNotFoundException(*klass, *klass, *selectorRef);
		}
	}
	
	//getVM()->getDebugger()->printFrameStack(this, frame);

	// call method
	{
		Frame* nextFrame = openFrame();
		method->call(this); // pops class & method
		//closeFrame();
	
		//getVM()->getDebugger()->printFrameStack(this, frame);

		DBG_ASSERT(!ret.isNull(), BEER_WIDEN("No instance created"));

		//ret = instance;
		//frame->stackMoveTop(-1); // 
	}

	BEER_ASSERT_STACK_END(1);
}

void Thread::staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount)
{
	ret = getHeap()->alloc(
		staticSize,
		Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount() + additionalChildrenCount
	);

	Class::setClass(this, ret, klass);

	getFrame()->stackMoveTop(-1); // pop klass
}

Class* Thread::getClass(const StackRef<Object>& object)
{
	return mVM->getClassTable()->translate(this, object);
}