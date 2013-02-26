#pragma once
#include "prereq.h"
#include "String.h"
#include "Class.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"
#include "CharacterClass.h"
#include "CopyGC.h"


namespace Beer
{
	class VirtualMachine;

	class StringPool
	{
	public:
		typedef std::map<const char16*, Reference<String>> StringMap;
		typedef StringMap::iterator iterator;

	protected:
		StringMap mStrings;

	public:
		//Reference<String> translate(VirtualMachine* vm, const char8* str);
		Reference<String> translate(Thread* thread, const char16* str);
		
		INLINE void clear()
		{
			mStrings.clear();
		}

		INLINE iterator begin()
		{
			return mStrings.begin();
		}

		INLINE iterator end()
		{
			return mStrings.end();
		}
	};

	class StringClass : public Class
	{
	protected:
		StringPool mPool;

	public:
		INLINE StringPool* getPool()
		{
			return &mPool;
		}

		INLINE Reference<String> translate(Thread* thread, const char16* str)
		{
			return mPool.translate(thread, str);
		}

		static void BEER_CALL createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<String> ret);
	};

	class StringClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};