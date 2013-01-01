#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"


namespace Beer
{
	class VirtualMachine;

	class Float : public Object
	{
	public:
		typedef float64 FloatData;

	protected:
		FloatData mData;

	public:
		 FloatData getData() const
		{
			return mData;
		}

		INLINE void setData(FloatData data)
		{
			mData = data;
		}
	};

	class FloatClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			Float* num = gc->alloc<Float>();
			num->setClass(this);
			return num;
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			Float* num = static_cast<Float*>(this->ClassReflection::cloneShallow(object, frame, gc));
			num->setData(object->getInstance<Float>()->getData());
			return num;
		}

		virtual void dump(Object* object, stringstream& out)
		{
			out << std::setprecision(8) << std::fixed << object->getInstance<Float>()->getData();
		}
	};
	
	class FloatClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};