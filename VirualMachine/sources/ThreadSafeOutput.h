#pragma once
#include "prereq.h"
#include "CachedOutput.h"


namespace Beer
{
	class ThreadSafeOutput
	{
	protected:
		stringstream mBuffer;
		ostream& mOutput;

	public:
		// no inline because of stringstream
		NOINLINE ThreadSafeOutput(ostream& output)
			: mOutput(output)
		{
		}
		
		// no inline because of stringstream
		NOINLINE ~ThreadSafeOutput()
		{
			flush();
		}

		template <typename T>
		INLINE ThreadSafeOutput& operator<<(const T& t)
		{
			mBuffer << t;
			return *this;
		}

		INLINE ThreadSafeOutput& operator<<(String* str)
		{
			mBuffer << str->c_str(); // TODO
			return *this;
		}

	protected:
		INLINE void flush()
		{
			mOutput << mBuffer.rdbuf() << std::flush;
		}

		INLINE void clear()
		{
			mBuffer.str(string());
		}
	};
};