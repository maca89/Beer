#pragma once
#include "prereq.h"
#include "Object.h"
#include "String.h"
#include "VirtualMachine.h"
#include "StringDescriptor.h"
//#include "ClassFileDescriptor.h"
//#include "ClassDescriptor.h"
#include "TrampolineThread.h"


namespace Beer
{
	class ClassDescriptor;
	class ClassFileDescriptor;
	class Thread;
	class VirtualMachine;

	class StringDescriptor;
	class AttributeDescriptor;
	class MethodDescriptor;
	class ParamDescriptor;

	class LoadedObject : public Object
	{
	public:
		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret);
	};

	class LoadedObjectInitializer : public ClassInitializer
	{
	protected:
		ClassDescriptor* mClassDescr;
		ClassFileDescriptor* mClassFile;
		Class* mClass; // TODO

	public:
		INLINE LoadedObjectInitializer(ClassFileDescriptor* classFile, ClassDescriptor* classDescr)
			: mClassFile(classFile), mClassDescr(classDescr), mClass(NULL)
		{
		}

		// interface ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* vm, ClassLoader* loader, Class* klass);

	protected:
		AttributeDescriptor* getAtribute(uint16 i);
		MethodDescriptor* getMethod(uint16 i);

		void makeProperty(Thread* thread, StackRef<Property> ret, ClassLoader* loader, AttributeDescriptor* attrDescr);
		Method* makeMethod(Thread* thread, StackRef<Class> klass, ClassLoader* loader, MethodDescriptor* methodDescr);
		void makeParam(Thread* thread, StackRef<Param> ret, ClassLoader* loader, ParamDescriptor* paramDescr);

		const char_t* getParentClassName(Thread* thread, uint16 i);
		void getParentClassName(Thread* thread, StackRef<String> ret, uint16 i);
		void getType(Thread* thread, StackRef<Class> ret, const StringDescriptor* name);

		INLINE void getString(Thread* thread, StackRef<String> ret, const StringDescriptor* strdescr)
		{
			thread->createString(ret, strdescr->c_str());
		}
	};
};