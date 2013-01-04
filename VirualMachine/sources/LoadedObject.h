#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "StringClass.h"
#include "VirtualMachine.h"
#include "StringDescriptor.h"
//#include "ClassFileDescriptor.h"
//#include "ClassDescriptor.h"
#include "TrampolineThread.h"


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
	};

	class LoadedObjectClass : public ClassReflection
	{
	public:
		// ClassReflection

		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			LoadedObject* obj = gc->alloc<LoadedObject>(getPropertiesCount());
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
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);

	protected:
		AttributeDescriptor* getAtribute(uint16 i);
		MethodDescriptor* getMethod(uint16 i);

		PropertyReflection* makeProperty(VirtualMachine* vm, AttributeDescriptor* attrDescr);
		MethodReflection* makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr);

		const char_t* getParentClassName(VirtualMachine* vm, uint16 i);
		ClassReflection* getType(const StringDescriptor* name, VirtualMachine* vm);

		INLINE Reference<String> getString(VirtualMachine* vm, const StringDescriptor* strdescr)
		{
			return vm->getStringClass<StringClass>()->translate(vm, strdescr->c_str());
		}
	};
};