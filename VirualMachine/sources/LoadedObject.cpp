#include "stdafx.h"
#include "LoadedObject.h"

#include "VirtualMachine.h"

#include "ClassFileDescriptor.h"
#include "ClassDescriptor.h"
#include "MethodDescriptor.h"
#include "AttributeDescriptor.h"
#include "ParamDescriptor.h"
//#include "StringDescriptor.h"
#include "BytecodeDescriptor.h"
#include "BytecodeBuilder.h"

#include "Property.h"
#include "Param.h"
#include "Method.h"

#include "Bytecode.h"
#include "Task.h"

using namespace Beer;

void BEER_CALL LoadedObject::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	Class::createInstance(thread, receiver, ret); // call super method

	/////////////////////////////////////////////////////////
	// fix missing value types
	if(true)
	{
		for(uint32 i = 0; i < receiver->getPropertiesCount(); i++)
		{
			Property* prop = receiver->getProperty(i);
		
			if(prop)
			{
				Class* type = prop->getPropertyType();

				if(type->isValueType())
				{
					StackRef<Class> klass(frame, frame->stackPush(type)); // push child
					StackRef<Object> child(frame, frame->stackPush()); // push child
					thread->createInstance(klass, child);

					Object::setChild(thread, ret, Object::OBJECT_CHILDREN_COUNT + i, child);
					frame->stackPop(); // pop child
					frame->stackPop(); // pop klass
				}
			}
		}
	}
	/////////////////////////////////////////////////////////
}


Class* LoadedObjectInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	string className = name->c_str();
	uint32 virtualMethods = 0;
	uint32 interfaceMethods = 0;
	uint32 parentsCount = 0;
	uint32 propertiesLength = mClassDescr->getAttributesLength();

	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		if(!methodDescr->isOverride())
		{
			virtualMethods += 1;
		}
	}

	Class* superClass = NULL;
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);

		Class* parent = thread->getVM()->findClass(parentClassName);

		if(!parent->isInterface())
		{
			superClass = parent;
			parentsCount++;
			frame->stackPop(); // pop parentClassName
			break;
		}

		frame->stackPop(); // pop parentClassName
	}

	if(superClass == NULL)
	{
		superClass = thread->getVM()->getObjectClass();
		parentsCount++;
	}

	parentsCount += superClass->getInterfacesCount();

	virtualMethods += superClass->getVirtualMethodsCount();
	interfaceMethods += superClass->getInterfaceMethodsCount();
	propertiesLength += superClass->getPropertiesCount();

	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);

		Class* parent = thread->getVM()->findClass(parentClassName);

		if(!parent->isInterface() && parent != superClass)
		{
			throw Exception(BEER_WIDEN("Unable to inherit from more than 1 non-interface class")); // TODO: better exception
		}

		if(parent->isSealed())
		{
			throw Exception(BEER_WIDEN("Unable to inherit from sealed class")); // TODO: better exception
		}

		if(parent->isInterface())
		{
			if(!superClass->hasInterface(parent))
			{
				// TODO: virtual multiple inheritance of interfaces!!!!!!!!!!!
				interfaceMethods += parent->getMethodsCount() - parent->getSuperClass()->getVirtualMethodsCount();
				parentsCount += 1 + parent->getInterfacesCount(); // TODO: probably more parents than needed if virtual inheritance from interfaces
			}
		}
		// else is a super class => resolved elsewhere
		

		frame->stackMoveTop(-1); // pop parentClassName
	}

	Class* klass = loader->createClass<Class>(
		thread, 
		name, // classDescr->getName(classFile)->c_str()
		parentsCount,
		propertiesLength, 
		virtualMethods,
		interfaceMethods
	);

	if(mClassDescr->isAbstract())
	{
		// let's say its an interface
		if(propertiesLength == 0 && superClass == thread->getVM()->getObjectClass())
		{
			klass->markInterface();
		}
		else
		{
			klass->markAbstract();
		}
	}

	return klass;
}

void LoadedObjectInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass) // TODO: add parents properties into properties size
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	mClass = klass; // ugly, TODO: get rid of
	
	uint32 instanceStaticSize = sizeof(Object);
	uint16 methodStart = 0;

	Class* superClass = NULL;

	// find super class
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);

		Class* parent = thread->getVM()->findClass(parentClassName);

		if(!parent->isInterface())
		{
			superClass = parent;
			frame->stackPop(); // pop parentClassName
			break;
		}

		frame->stackPop(); // pop parentClassName
	}

	if(superClass == NULL)
	{
		superClass = thread->getVM()->getObjectClass();
	}

	klass->setSuperClass(superClass);

	// find interfaces
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);
		
		Class* parent = thread->getVM()->findClass(parentClassName);

		if(parent->isInterface())
		{
			klass->addInterface(parent);
		}
		
		instanceStaticSize = max(instanceStaticSize, parent->mInstanceStaticSize); // TODO
		
		frame->stackPop(); // pop parentClassName
	}

	klass->mInstanceStaticSize = instanceStaticSize; // TODO
	
	// attributes
	{
		StackRef<Property> prop(frame, frame->stackPush());
		
		for(uint16 i = 0; i < mClassDescr->getAttributesLength(); i++)
		{
			AttributeDescriptor* attrDescr = getAtribute(i);
			makeProperty(thread, prop, loader, attrDescr);
			klass->addProperty(*prop);
		}

		frame->stackPop(); // pop property
	}

	// methods
	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		StackRef<Class> klassOnStack(frame, frame->stackPush(klass)); // TODO: get rid of
		MethodDescriptor* methodDescr = getMethod(i);
		Method* method = makeMethod(thread, klassOnStack, loader, methodDescr);

		string selector = string() + BEER_WIDEN("::") + method->getName()->c_str();

		selector += BEER_WIDEN("(");
		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			selector += String::gTranslate(thread, mClassFile->getClassName(paramDescr->getTypeId())->c_str())->c_str();
			if(parami < methodDescr->getParamsLength() - 1) selector += BEER_WIDEN(",");
		}
		selector += BEER_WIDEN(")");

		if(methodDescr->isOverride())
		{
			string interfName = methodDescr->getInterfaceName(mClassFile)->c_str();
			Class* interf = thread->getVM()->findClass(interfName);
			
			if(interf->isInterface())
			{
				selector = interfName + selector; //ugly
				method->setInterface(interf);
				loader->addInterfaceMethod(thread, klass, interf, method, selector.c_str());
			}
			else // overriding parent's method
			{
				selector = interfName/*klass->getName()->c_str()*/ + selector; //ugly
				method->setInterface(interf); //really??
				loader->addOverrideMethod(thread, klass, method, selector.c_str());
			}
		}
		else
		{
			selector = klass->getName()->c_str() + selector;
			loader->addVirtualMethod(thread, klass, method, selector.c_str());
		}

		frame->stackPop(); // pop klassOnStack
	}

	loader->addOverrideMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &LoadedObject::createInstance, 1, 0);
}

AttributeDescriptor* LoadedObjectInitializer::getAtribute(uint16 i)
{
	return mClassFile->getDescriptor<AttributeDescriptor>(mClassDescr->getAttributeId(i));
}

MethodDescriptor* LoadedObjectInitializer::getMethod(uint16 i)
{
	return mClassFile->getDescriptor<MethodDescriptor>(mClassDescr->getMethodId(i));
}

void LoadedObjectInitializer::makeProperty(Thread* thread, StackRef<Property> ret, ClassLoader* loader, AttributeDescriptor* attrDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	loader->createProperty(thread, ret);
	
	// set name
	{
		StackRef<String> name(frame, frame->stackPush());
		getString(thread, name, attrDescr->getName(mClassFile));

		Property::setPropertyName(thread, ret, name);
		frame->stackPop(); // pop name
	}

	// set class
	{
		StackRef<Class> klass(frame, frame->stackPush());
	
		// fetch klass
		{
			StackRef<String> klassName(frame, frame->stackPush());
			getString(thread, klassName, mClassFile->getClassName(attrDescr->getTypeId()));

			thread->findClass(klassName, klass);
			frame->stackPop(); // pop klassName
		}

		Property::setPropertyType(thread, ret, klass);
		frame->stackPop(); // pop klass
	}
}

void LoadedObjectInitializer::makeParam(Thread* thread, StackRef<Param> ret, ClassLoader* loader, ParamDescriptor* paramDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	loader->createParam(thread, ret);

	// set name
	{
		StackRef<String> name(frame, frame->stackPush());
		getString(thread, name, paramDescr->getName(mClassFile));

		Param::setName(thread, ret, name);
		frame->stackPop(); // pop name
	}
	
	// set class
	{
		StackRef<String> klassName(frame, frame->stackPush());
		getString(thread, klassName, mClassFile->getClassName(paramDescr->getTypeId()));

		StackRef<Class> klass(frame, frame->stackPush());
		thread->findClass(klassName, klass);

		Param::setParamType(thread, ret, klass);
		frame->stackMoveTop(-2); // pop klassName, klass
	}
}

Method* LoadedObjectInitializer::makeMethod(Thread* thread, StackRef<Class> klass, ClassLoader* loader, MethodDescriptor* methodDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	Method* method = loader->createMethod(thread, *String::gTranslate(thread, methodDescr->getName(mClassFile)->c_str()), NULL, methodDescr->getReturnsLength(), methodDescr->getParamsLength());

	// TODO: get rid of this
	StackRef<Method> methodOnStack(frame, frame->stackPush(
		method
	));
	
	method->setMaxStack(20); // TODO: methodDescr->getMaxStack()

	// returns
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
		{
			thread->createInteger(index, reti);

			ParamDescriptor* returnDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getReturnId(reti));
			makeParam(thread, param, loader, returnDescr);

			Method::setReturn(thread, methodOnStack, index, param);
		}

		frame->stackMoveTop(-2); // pop param, index
	}

	// params
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			thread->createInteger(index, parami);

			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			makeParam(thread, param, loader, paramDescr);

			Method::setParam(thread, methodOnStack, index, param);
		}
		
		frame->stackMoveTop(-2); // pop param, index
	}

	if(!methodDescr->isAbstract() && !methodDescr->isNative())
	{
		// bytecode
		BytecodeDescriptor* bcDescr = mClassFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
		thread->getVM()->getBytecodeBuilder()->build(thread, method, mClassFile, bcDescr);
	}

	frame->stackPop(); // pop methodOnStack

	return method;
}

void LoadedObjectInitializer::getParentClassName(Thread* thread, StackRef<String> ret, uint16 i)
{
	ret = *String::gTranslate(thread, mClassFile->getClassName(mClassDescr->getParentId(i))->c_str());
}

const char_t* LoadedObjectInitializer::getParentClassName(Thread* thread, uint16 i)
{
	return String::gTranslate(thread, mClassFile->getClassName(mClassDescr->getParentId(i))->c_str())->c_str();
}

void LoadedObjectInitializer::getType(Thread* thread, StackRef<Class> ret, const StringDescriptor* name)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<String> thisClassName(frame, frame->stackPush());
	
	// fetch class name, veeeeery ugly, TODO!!!
	{
		StackRef<Class> klassOnStack(frame, frame->stackPush(mClass)); // TODO: pass as reference
		Class::getName(thread, klassOnStack, thisClassName);
		frame->stackPop(); // pop class
	}
	
	StackRef<String> thatClassName(frame, frame->stackPush());
	getString(thread, thatClassName, name);
	
	if(thisClassName->compare(*thatClassName) == 0)
	{
		ret = mClass; // prevents circular reference exception
	}
	else
	{
		thread->findClass(thatClassName, ret);
	}

	frame->stackMoveTop(-2); // pop thisClassName, thatClassName
}