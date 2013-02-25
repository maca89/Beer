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
	class ParamDescriptor;

	class LoadedObject : public Object
	{
	};

	class LoadedObjectClass : public Class
	{
	public:
		// ClassReflection

		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			LoadedObject* obj = gc->alloc<LoadedObject>(Object::OBJECT_CHILDREN_COUNT + getPropertiesCount());
			obj->setClass(this);

			return obj;
		}
	};

	class LoadedObjectInitializer : public ClassInitializer
	{
	protected:
		ClassDescriptor* mClassDescr;
		ClassFileDescriptor* mClassFile;
		Class* mClass;

	public:
		INLINE LoadedObjectInitializer(ClassFileDescriptor* classFile, ClassDescriptor* classDescr)
			: mClassFile(classFile), mClassDescr(classDescr), mClass(NULL)
		{
		}

		// interface ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);

	protected:
		AttributeDescriptor* getAtribute(uint16 i);
		MethodDescriptor* getMethod(uint16 i);

		Property* makeProperty(VirtualMachine* vm, ClassLoader* loader, AttributeDescriptor* attrDescr);
		Method* makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr);
		Param* makeParam(VirtualMachine* vm, ClassLoader* loader, ParamDescriptor* paramDescr);

		const char_t* getParentClassName(VirtualMachine* vm, uint16 i);
		Class* getType(const StringDescriptor* name, VirtualMachine* vm);

		INLINE String* getString(VirtualMachine* vm, const StringDescriptor* strdescr)
		{
			//return vm->getStringClass<StringClass>()->translate(vm, strdescr->c_str());
			return vm->createString(strdescr->c_str());
		}
	};
};