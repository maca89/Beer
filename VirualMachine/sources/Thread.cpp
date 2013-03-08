#include "stdafx.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "Method.h"
#include "Array.h"
//#include "PolymorphicInlineCache.h"
#include "GenerationalGC.h"
#include "Heap.h"

using namespace Beer;

#if defined(BEER_STACK_DEBUGGING)
#define BEER_ASSERT_STACK_START() int __startSize = frame->stack->size();
#define BEER_ASSERT_STACK_END(params) DBG_ASSERT(__startSize - params == frame->stack->size(), BEER_WIDEN("Stack corrupted"));
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
	: mVM(vm), mStack(1024), mFrames(50), mGC(gc), mTopFrame(NULL)
{
	StackFrame frame(&mStack);
	mFrames.push(frame);
	fetchTopStackFrame();

	mHeap = mGC->createHeap();

	PolymorphicInlineCache* methodCache = PolymorphicInlineCache::from(createInstanceMethodCacheBytes);
	methodCache->clear(CREATE_INSTANCE_CACHE_SIZE);
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
	StackFrame* frame = getStackFrame();
	ASSERT_STACK_PARAMS_2(name, ret);
	BEER_ASSERT_STACK_START();
	ret = mVM->getClass(*name);
	frame->stackMoveTop(-1); // pop name
	BEER_ASSERT_STACK_END(1);
}

void Thread::getMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
{
	StackFrame* frame = getStackFrame();
	ASSERT_STACK_PARAMS_3(klass, selector, ret);
	BEER_ASSERT_STACK_START();

	Class::findMethod(this, klass, selector, ret);

	frame->stackMoveTop(-2); // pop class & selector
	BEER_ASSERT_STACK_END(2);
}

void Thread::createInteger(StackRef<Integer> ret, Integer::IntegerData value)
{
	StackFrame* frame = getStackFrame();
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
	StackFrame* frame = getStackFrame();
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
	StackFrame* frame = getStackFrame();
	BEER_ASSERT_STACK_START();
	StackRef<Integer> length(frame, frame->stackPush());
	createInteger(length, value.size());

	createString(length, ret); // pops length
	
	ret->copyData(value.c_str());
	BEER_ASSERT_STACK_END(0);
}

void Thread::createString(StackRef<Integer> length, StackRef<String> ret)
{
	StackFrame* frame = getStackFrame();
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
	StackFrame* frame = getStackFrame();
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
	StackFrame* frame = getStackFrame();
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
	StackFrame* frame = getStackFrame();
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

	Method* rawmethod = *method; // ugly, TODO: on stack
	frame->stackPop(); // pop method

	// call method
	{
		//StackRef<Object> instance(frame, frame->stackPush(NULL)); // ret
		//klass.copy(); // receiver

		StackFrame* nextFrame = openStackFrame();
		rawmethod->call(this); // pops class

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

	getStackFrame()->stackMoveTop(-1); // pop klass
}

Class* Thread::getClass(const StackRef<Object>& object)
{
	return mVM->getClassTable()->translate(this, object);
}