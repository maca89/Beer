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
#ifdef BEER_VALUE_TYPES_WORKAROUND
			if(this == NULL)
			{
				return false; // just a workaround, TODO
			}
#endif // BEER_VALUE_TYPES_WORKAROUND

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

		static void BEER_CALL init(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret1);
		static void BEER_CALL operatorNegation(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret);
		static void BEER_CALL operatorEqual(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorOr(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorAnd(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret);
	};

	class BooleanClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};