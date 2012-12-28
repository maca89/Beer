#pragma once
#include "prereq.h"
#include "ObjectClass.h"


/*namespace Beer
{
	class VirtualMachine;

	class Character : public Object
	{
	public:
		typedef char CharacterData;

	protected:
		CharacterData mData;

	public:
		CharacterData getData() const
		{
			return mData;
		}

		void setData(CharacterData data)
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
			Character* c = gc->alloc<Character>();
			c->setClass(this);
			return c;
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			Character* c = static_cast<Character*>(this->ClassReflection::cloneShallow(object, frame, gc));
			c->setData(object->getInstance<Character>()->getData());
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
};*/