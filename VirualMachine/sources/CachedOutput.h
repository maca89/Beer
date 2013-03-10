#pragma once
#include "prereq.h"
#include "String.h"


namespace Beer
{
	class CachedOutput
	{
	protected:
		stringstream mStream;
		bool mChanged;

	public:
		// no inline because of stringstream!
		NOINLINE CachedOutput() : mChanged(false)
		{
		}

		// no inline because of stringstream!
		NOINLINE ~CachedOutput()
		{
			flush(cout); // really??
		}

		INLINE void flush(ostream& out)
		{
			if(mChanged)
			{
				doflush(out);
			}
		}

		INLINE CachedOutput& operator<<(String* str)
		{
			mStream << str->c_str();
			mChanged = true;
			return *this;
		}

		template <typename T>
		INLINE CachedOutput& operator<<(const T& t)
		{
			mStream << t;
			mChanged = true;
			return *this;
		}

		// TODO:

		// endl
		//INLINE CachedOutput& operator<<(std::ostream& (__cdecl *fn)(std::ostream&)) { fn(mStream); mChanged = true; return *this; }

		// io manips
		//INLINE CachedOutput& operator<<(std::_Smanip<streamsize> s) { mStream << s; return *this; }

	protected:
		NOINLINE void doflush(ostream& out)
		{
			//mStream.flush(); // TODO for threads

			// TODO
			out << mStream.rdbuf(); // print content

			// TMP
			//string content = mStream.str();

			mStream.str(string()); // clear buffer
			//mStream.clear(); // TODO: clears flags
			mChanged = false; // set flag
		}
	};
};