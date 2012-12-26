#pragma once
#include "prereq.h"


namespace Beer
{
	class CachedOutput
	{
	protected:
		std::stringstream mStream;
		bool mChanged;

	public:
		INLINE CachedOutput()  : mChanged(false)
		{
		}

		INLINE ~CachedOutput()
		{
		}

		INLINE void flush(std::ostream& out)
		{
			if(mChanged)
			{
				doflush(out);
			}
		}

		INLINE CachedOutput& operator<<(uint16 n) { mStream << n; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(uint32 n) { mStream << n; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(uint64 n) { mStream << n; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(float32 n) { mStream << n; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(float64 n) { mStream << n; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(const std::string& s) { mStream << s; mChanged = true; return *this; }
		INLINE CachedOutput& operator<<(const char* s) { mStream << s; mChanged = true; return *this; }

		// endl
		INLINE CachedOutput& operator<<(std::ostream& (__cdecl *fn)(std::ostream&)) { fn(mStream); mChanged = true; return *this; }

		// manips
		//INLINE CachedOutput& operator<<(std::_Smanip<streamsize> s) { mStream << s; return *this; }

		template <typename T>
		INLINE CachedOutput& operator<<(T t) { mStream << t; mChanged = true; return *this; }

		INLINE std::string dump() { return mStream.str(); }

	protected:
		NOINLINE void doflush(std::ostream& out)
		{
			mStream.flush();
			out << mStream.str();
			mStream = std::stringstream();
			mChanged = false;
		}
	};
};