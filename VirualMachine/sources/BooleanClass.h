#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"


namespace Beer
{
	class VirtualMachine;

	class Boolean : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();
	
	public:
		typedef bool BooleanData;
		//const static int InlineClassId = 5;

	protected:
		BooleanData mData;

	public:
		BooleanData getData() const
		{
			return mData;
		}

		void setData(BooleanData data)
		{
			mData = data;
		}
	};

	class BooleanClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			Boolean* b = gc->alloc<Boolean>();
			b->setClass(this);
			return b;
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			Boolean* b = static_cast<Boolean*>(this->ClassReflection::cloneShallow(object, frame, gc));
			b->setData(object->getInstance<Boolean>()->getData());
			return b;
		}

		virtual void dump(Object* object, std::stringstream& out)
		{
			out << object->getInstance<Boolean>()->getData();
		};
	};

	class BooleanClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};