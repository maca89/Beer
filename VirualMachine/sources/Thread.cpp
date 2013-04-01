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

using namespace Beer;


Thread::Thread(VirtualMachine* vm, GenerationalGC * gc)
	: mVM(vm), mGC(gc), mPolycache(NULL), mContext(NULL)//, mTopFrame(NULL), mRootFrame(NULL)
{
}

void Thread::init()
{
	mHeap = mGC->createHeap();

	// just a temporary solution, TODO
	mTemporaryContext.init(mHeap);
	setContext(&mTemporaryContext);

	// init cache, TODO
	Frame* frame = getFrame();
	StackRef<PolymorphicCache> pcache(frame, frame->stackPush());
	createPolycache(pcache, CREATE_INSTANCE_CACHE_SIZE);
	mPolycache = *pcache;
	frame->stackPop(); // pop pcache
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
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> stringClass(frame, frame->stackPush());
	getStringClass(stringClass);

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(this, stringClass, propertiesCount);

	ret = getHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (length + 1)), // +1 for \0
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData())
	);
	ret->size(length);
	Object::setType(this, ret, stringClass);

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

	StackRef<Method> method(frame, frame->stackPush(NULL));

	// find method
	{
		StackRef<PolymorphicCache> cache(frame, frame->stackPush(
			mPolycache
		));

		static Reference<String> selectorRef = String::gTranslate(this, BEER_WIDEN("$Class::createInstance()")); // TODO;
		StackRef<String> selector(frame, frame->stackPush(
			*selectorRef
		));

		PolymorphicCache::find(this, cache, klass, selector, method);
		frame->stackMoveTop(-2); // pop cache, selector
	
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
		method->invoke(this); // pops copied class, copied method

		ret = copiedRet;
		frame->stackPop(); // pop copiedRet

		DBG_ASSERT(!ret.isNull(), BEER_WIDEN("No instance created"));
	}

	frame->stackPop(); // pop method
}

void Thread::staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();	

	ret = getHeap()->alloc(
		staticSize,
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount() + additionalChildrenCount)
	);

	Object::setType(this, ret, klass);
}

Class* Thread::getType(Object* object)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Object> objectOnStack(frame, frame->stackPush(object));
	Class* type = getType(objectOnStack);
	frame->stackPop(); // pop objectOnStack

	return type;
}

Class* Thread::getType(StackRef<Object> object)
{
	return mVM->getClassTable()->translate(this, object);
}

void Thread::getType(StackRef<Object> object, StackRef<Class> ret)
{
	mVM->getClassTable()->translate(this, object, ret);
}

void Thread::createPolycache(StackRef<PolymorphicCache> ret, uint16 length)
{
	ret = getHeap()->alloc<PolymorphicCache>(PolymorphicCache::POLYCACHE_CHILDREN_COUNT + length);

	PolymorphicCache::setLength(this, ret.staticCast<Pool>(), length);
	PolymorphicCache::clear(this, ret);
}