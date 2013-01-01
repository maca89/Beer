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
	public:
		typedef int32 IntegerData;
		static const int SignatureBits = 2;

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

		NOINLINE void toString(string& out)
		{
			stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static bool canBeInlineValue(IntegerData data)
		{
			// TODO: negative numbers
			return (data >> (sizeof(IntegerData) * 8 - SignatureBits)) == 0;
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << SignatureBits) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> SignatureBits;
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

		virtual void dump(Object* object, stringstream& out)
		{
			out << object->getInstance<Integer>()->getData();
		};
	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};