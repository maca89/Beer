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
		static const int SignatureBits = 4;

	protected:
		//BooleanData mData; // not used

	public:
		INLINE BooleanData getData() const
		{
			return (reinterpret_cast<uint32>(this) >> SignatureBits) != 0;
		}

		NOINLINE void toString(string& out)
		{
			stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static Boolean* makeInlineValue(BooleanData data)
		{
			return reinterpret_cast<Boolean*>((data << SignatureBits) | 3);
		}
	};

	class BooleanClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			return Boolean::makeInlineValue(false);
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			return object;
		}

		virtual void dump(Object* object, stringstream& out)
		{
			out << object->getInstance<Boolean>()->getData();
		};
	};

	class BooleanClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};