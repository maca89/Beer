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
		INLINE FloatData getData() const
		{
			return mData;
		}

		INLINE void setData(FloatData data)
		{
			mData = data;
		}
	};

	class FloatClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Float* num = gc->alloc<Float>(Object::OBJECT_CHILDREN_COUNT);
			num->setClass(this);
			return num;
		}
	};
	
	class FloatClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};