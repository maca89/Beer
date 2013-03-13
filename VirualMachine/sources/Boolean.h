#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"


namespace Beer
{
	class VirtualMachine;
	class Class;

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

		static void BEER_CALL createInstance(Thread* thread/*, Frame* frame*/, StackRef<Class> receiver, StackRef<Boolean> ret);

		static void BEER_CALL init(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> ret1);

		static void BEER_CALL operatorNegation(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> ret);
		static void BEER_CALL operatorEqual(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorOr(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorAnd(Thread* thread/*, Frame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
	};

	class BooleanClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};