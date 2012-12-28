#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
//#include "ClassTable.h"


namespace Beer
{
	class VirtualMachine;

	class Integer : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();

	public:
		typedef int32 IntegerData;
		//const static int InlineClassId = 1;

	protected:

		IntegerData mData;

	public:
		INLINE IntegerData getData() const
		{
			if(isInlineValue())
			{
				return getInlineValue(this);
			}
			return mData;
		}

		INLINE void setNonInlineValue(IntegerData data)
		{
			mData = data;
		}

		NOINLINE void toString(std::string& out)
		{
			std::stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static bool canBeInlineValue(IntegerData data)
		{
			// TODO: negative numbers
			return (data >> 30) == 0; // int32 only
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << 2) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> 2;
		}
	};

	class IntegerClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			Integer* num = gc->alloc<Integer>();
			num->setClass(this);
			return num;
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			if(object->isInlineValue()) return object;

			Integer* num = static_cast<Integer*>(this->ClassReflection::cloneShallow(object, frame, gc));
			num->setNonInlineValue(object->getInstance<Integer>()->getData());
			return num;
		}

		virtual void dump(Object* object, std::stringstream& out)
		{
			out << object->getInstance<Integer>()->getData();
		};
	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};