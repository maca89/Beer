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
	public:
		typedef bool BooleanData;

	protected:
		//BooleanData mData;

	public:
		INLINE BooleanData getData() const
		{
			return (reinterpret_cast<uint32>(this) >> 4) != 0;
		}

		NOINLINE void toString(std::string& out)
		{
			std::stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static Boolean* makeInlineValue(BooleanData data)
		{
			return reinterpret_cast<Boolean*>((data << 4) | 3);
		}
	};

	class BooleanClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			/*Boolean* b = gc->alloc<Boolean>();
			b->setClass(this);
			return b;*/
			return Boolean::makeInlineValue(false);
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			return object;
			/*Boolean* b = static_cast<Boolean*>(this->ClassReflection::cloneShallow(object, frame, gc));
			b->setData(object->getInstance<Boolean>()->getData());
			return b;*/
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