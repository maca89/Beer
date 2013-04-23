#include "stdafx.h"
#include "Class.h"
#include "Object.h"
#include "Method.h"
#include "Property.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Thread.h"
#include "Heap.h"

using namespace Beer;


void BEER_CALL Class::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret)
{
	DBG_ASSERT(receiver->mInstanceStaticSize > 0, BEER_WIDEN("Instance size is zero"));

	ret = thread->getHeap()->alloc<Object>(
		receiver->mInstanceStaticSize,
		Object::OBJECT_CHILDREN_COUNT + receiver->getPropertiesCount()
	);

	if(ret.isNull())
	{
		throw NotEnoughMemoryException(BEER_WIDEN("Unable to create new instance - Not enough memory"));
	}
	
	ret->setType(*receiver);
}

bool Class::findVirtualMethodIndex(String* selector, uint32& out_index)
{
	for(uint32 methodi = 0; methodi < getMethodsCount(); methodi++)
	{
		Method* method = getMethod(methodi); // TODO: getVirtualMethod

		if(method)
		{
			if(method->satisfyVirtual(this, selector))
			{
				out_index = methodi;
				return true;
			}
		}
	}

	if(getSuperClass() != this)
	{
		return getSuperClass()->findVirtualMethodIndex(selector, out_index);
	}

	return false;
}

bool Class::findInterfaceMethodIndex(Class* interf, String* selector, uint32& out_index)
{
	uint32 startIndex = 0;
	if(findInterfaceMethodsStart(interf, startIndex))
	{
		for(uint32 methodi = 0; methodi < interf->getVirtualMethodsCount() - interf->getSuperClass()->getVirtualMethodsCount(); methodi++)
		{
			Method* method = getMethod(startIndex + methodi); // TODO: getInterfaceMethod

			if(method)
			{
				if(method->satisfyInterface(this, interf, selector))
				{
					out_index = startIndex + methodi;
					return true;
				}
			}
		}
	}

	return false;
}

bool Class::substituable(Class* otherClass)
{
	// is otherClass the same class?
	if(this == otherClass)
	{
		return true;
	}

	// check parents
	for(uint32 i = 0; i < getInterfacesCount(); i++)
	{
		Class* interf = getInterface(i);

		if(interf == otherClass)
		{
			return true;
		}
	}

	// parent of myself
	if(this == getSuperClass())
	{
		// is not substituable
		return false;
	}

	// check parent
	return getSuperClass()->substituable(otherClass);
}

void BEER_CALL Class::substituable(Thread* thread, StackRef<Class> receiver, StackRef<Class> otherClass, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->substituable(*otherClass));
}

void BEER_CALL Class::getName(Thread* thread, StackRef<Class> receiver, StackRef<String> ret)
{
	ret = receiver->getName();
}

bool Class::findInterfaceIndex(Class* interf, uint32& out_index)
{
	for(uint32 interfi = 0; interfi < getInterfacesCount(); interfi++)
	{
		Class* myInterf = getInterface(interfi);
		
		if(myInterf == interf)
		{
			out_index = interfi;
			return true;
		}

		if(myInterf == NULL)
		{
			break;
		}
	}

	return false;
}

bool Class::findInterfaceMethodsStart(Class* interf, uint32& out_index)
{
	uint32 index = getVirtualMethodsCount();
	for(uint32 interfi = 0; interfi < getInterfacesCount(); interfi++)
	{
		Class* myInterf = getInterface(interfi);
		
		if(myInterf == interf)
		{
			out_index = index;
			return true;
		}

		if(myInterf == NULL)
		{
			break;
		}

		index += myInterf->getVirtualMethodsCount() - myInterf->getSuperClass()->getVirtualMethodsCount();
	}

	return false;
}

uint32 Class::addInterface(Class* interf)
{
	DBG_ASSERT(interf->isInterface(), BEER_WIDEN("Not an interface"));
	DBG_ASSERT(mInterfaceNext < mInterfacesCount, BEER_WIDEN("Unable to add more parents"));

	uint32 interfIndex = 0;
	if(!findInterfaceIndex(interf, interfIndex))
	{
		for(uint16 interfacei = 0; interfacei < interf->getInterfacesCount(); interfacei++)
		{
			Class* subInterf = interf->getInterface(interfacei);
			addInterface(subInterf);
		}

		interfIndex = mInterfaceNext++;
		setChild(OBJECT_CHILDREN_COUNT + interfIndex, interf);

		uint16 interfMethodsStart = interf->getSuperClass()->getVirtualMethodsCount(); // skip superclass (probably Object) methods
		for(uint16 i = interfMethodsStart; i < interf->getVirtualMethodsCount(); i++)
		{
			Method* method = interf->getMethod(i);
			addInterfaceMethod(method);
		}
	}

	return interfIndex;
}

void Class::setSuperClass(Class* klass)
{
	DBG_ASSERT(!klass->isInterface(), BEER_WIDEN("Superclass may not be an interface"));


	mSuperClass = klass;
	mInterfaceNext = 0;
	setTraverser(klass->getTraverser());
	
	if(this != klass)
	{
		mVirtualMethodNext = 0;
		mInterfaceMethodNext = 0;
		mPropertyNext = 0;

		// copy methods
		for(uint16 i = 0; i < klass->getVirtualMethodsCount(); i++)
		{
			Method* method = klass->getMethod(i);
			addVirtualMethod(method);
		}

		// copy interfaces
		for(uint16 interfacei = 0; interfacei < klass->getInterfacesCount(); interfacei++)
		{
			Class* interf = klass->getInterface(interfacei);
			addInterface(interf);
		}

		for(uint16 i = 0; i < klass->getPropertiesCount(); i++)
		{
			Property* prop = klass->getProperty(i);
			addProperty(prop);
		}
	}
}

void Class::addProperty(Property* value)
{
	DBG_ASSERT(hasPropertyFreeSlot(), BEER_WIDEN("Unable to add more properties"));
	setProperty(mPropertyNext++, value);
}

void Class::DefaultInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance)
{
	if(!Object::isInlineValue(instance))
	{
		receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mType));
	
		uint32 childrenCount = Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount();

		for(uint32 i = 0; i < childrenCount; i++)
		{
			receiver->traverseObjectPtr(&instance->getChildren()[i]);
		}
	}
}

String::LengthData Class::getFQNFromSelector(String* selector)
{
	// TODO: namespaces, TODO: check if valid
	//bool first = false;
	String::LengthData i = 0;
	for(; i < selector->size(); i++)
	{
		if(selector->c_str()[i] == ':')
		{
			break;
			//if(first) first = true;
			//else { break; }
		}
	}

	return i;
}