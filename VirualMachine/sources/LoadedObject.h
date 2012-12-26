#pragma once
#include "prereq.h"
#include "ObjectClass.h"

//#include "ClassFileDescriptor.h"
//#include "ClassDescriptor.h"


namespace Beer
{
	class ClassDescriptor;
	class ClassFileDescriptor;
	class VirtualMachine;

	class StringDescriptor;
	class AttributeDescriptor;
	class MethodDescriptor;

	class LoadedObject : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();
	};

	class LoadedObjectClass : public ClassReflection
	{
	public:
		// ClassReflection

		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			//std::cout << "[LoadedObject::createInstance]";
			LoadedObject* obj = gc->alloc<LoadedObject>(/*LoadedObject::CHILDREN_COUNT +*/ getPropertiesCount());
			obj->setClass(this);
			return obj;
		}
	};

	class LoadedObjectInitializer : public ClassInitializer
	{
	protected:
		ClassDescriptor* mClassDescr;
		ClassFileDescriptor* mClassFile;
		ClassReflection* mClass;

	public:
		INLINE LoadedObjectInitializer(ClassFileDescriptor* classFile, ClassDescriptor* classDescr)
			: mClassFile(classFile), mClassDescr(classDescr), mClass(NULL)
		{
		}

		// interface ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);

	protected:
		AttributeDescriptor* getAtribute(uint16 i);
		MethodDescriptor* getMethod(uint16 i);

		PropertyReflection* makeProperty(VirtualMachine* vm, AttributeDescriptor* attrDescr);
		MethodReflection* makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr);

		const char* getParentClassName(uint16 i);
		ClassReflection* getType(const StringDescriptor* name, VirtualMachine* vm);
	};
};