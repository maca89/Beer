#pragma once
#include "prereq.h"
#include "StringClass.h"


namespace Beer
{
	class CachedOutput
	{
	protected:
		stringstream mStream;
		bool mChanged;

	public:
		INLINE CachedOutput()  : mChanged(false)
		{
		}

		INLINE ~CachedOutput()
		{
		}

		INLINE void flush(ostream& out)
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

		NOINLINE /*INLINE*/ CachedOutput& operator<<(const string& s) {
			mStream << s;
			mChanged = true;
			return *this;
		}

		NOINLINE /*INLINE*/ CachedOutput& operator<<(String* str) {
			mStream << str->c_str();
			mChanged = true;
			return *this;
		}

		NOINLINE CachedOutput& operator<<(const char_t* s){
			mStream << s;
			mChanged = true;
			return *this;
		}

		// endl
		//INLINE CachedOutput& operator<<(std::ostream& (__cdecl *fn)(std::ostream&)) { fn(mStream); mChanged = true; return *this; }

		// manips
		//INLINE CachedOutput& operator<<(std::_Smanip<streamsize> s) { mStream << s; return *this; }

		template <typename T>
		INLINE CachedOutput& operator<<(T t) { mStream << t; mChanged = true; return *this; }

		INLINE string dump() { return mStream.str(); }

	protected:
		NOINLINE void doflush(ostream& out)
		{
			//mStream.flush(); // TODO for threads

			// TODO
			out << mStream.str(); // print content

			// TMP
			//string content = mStream.str();


			mStream.str(string()); // clear buffer
			//mStream.clear(); // TODO: clears flags
			mChanged = false; // set flag
		}
	};
};