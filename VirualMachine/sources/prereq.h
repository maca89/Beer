#pragma once


namespace Beer
{
#ifdef _DEBUG
	#define INLINE inline
	#define NOINLINE
	#define BEER_DEBUG_MODE
#else
	#define INLINE __inline __forceinline
	#define NOINLINE
#endif

	#define BEER_CALL __cdecl
	typedef void(*Cb)();

	#define lengthof(arr) sizeof(arr) / sizeof(arr[0])

	#define SMART_DELETE(_i_) delete (_i_); (_i_) = NULL;
	#define SMART_DELETE_ARR(_i_) delete[] (_i_); (_i_) = NULL;
	#define BEER_BREAKPOINT() __asm { INT 3 }
	#define BEER_FOLDING_BLOCK

#ifdef BEER_DEBUG_MODE
	#define BEER_DBG_BREAKPOINT() BEER_BREAKPOINT()
#else
	#define BEER_DBG_BREAKPOINT()
#endif

	typedef __int64 int64;
	typedef __int32 int32;
	typedef __int16 int16;
	typedef __int8 int8;

	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;

	typedef float float32;
	typedef double float64;

	typedef char char8;
	typedef wchar_t char16;

	static const int64 INT64_MIN_VALUE = LLONG_MIN;
	static const int64 INT64_MAX_VALUE = LLONG_MAX;
	static const uint64 UINT64_MAX_VALUE = ULLONG_MAX;

	#define BEER_MULTIBYTE_STRINGS

#ifdef BEER_MULTIBYTE_STRINGS
	typedef char16 char_t;
	typedef std::wstring string;
	typedef std::wstringstream stringstream;
	typedef std::wostream ostream;
	typedef std::wifstream ifstream;
	// TODO
	#define cout std::wcout
	#define cin std::wcin

	#define BEER_WIDEN(x) (L##x)
	#define __WFILE__ _T(__FILE__)
#else
	typedef char8 char_t;
	typedef std::string string;
	typedef std::stringstream stringstream;
	typedef std::ostream ostream;
	typedef std::ifstream ifstream;
	// TODO
	#define cout std::cout
	#define cin std::cin

	#define BEER_WIDEN(x) x
	#define __WFILE__ __FILE__
#endif // BEER_MULTIBYTE_STRINGS

//#define BEER_HEAP_CHECK()
#define BEER_HEAP_CHECK() if (!_CrtCheckMemory()) __asm { int 3 }

	static void* memcpy(void* dest, const void* source, uint64 length)
	{
		return ::memcpy(dest, source, static_cast<uint32>(length)); //TODO: custom function
	}

	INLINE static int16 strcmp(const char8* str1, const char8* str2)
	{
		return ::strcmp(str1, str2);
	}

	INLINE static int16 strcmp(const char16* str1, const char16* str2)
	{
		return ::lstrcmpW(str1, str2);
	}

	INLINE static int32 strlen(const char8* str)
	{
		return ::strlen(str);
	}

	INLINE static int32 strlen(const char16* str)
	{
		return ::lstrlenW(str);
	}

	INLINE static void strcpy(char8* dest, size_t maxLength, const char8* source)
	{
		::strcpy_s(dest, maxLength, source);
	}

	INLINE static void strcpy(char16* dest, size_t maxLength, const char16* source)
	{
		::lstrcpynW(dest, source, maxLength);
	}

	INLINE static std::wstring strwiden(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

	//void ZeroMemory(void* mem, size_t size);

	struct Exception : std::exception
	{
	protected:
		string mWhat;
		string mName;
		string mFilename;
		long mLine;

	public:
		NOINLINE Exception(string message, string filename = BEER_WIDEN(""), long line = 0)
			: std::exception(std::string(message.begin(), message.end()).c_str()), mName(BEER_WIDEN("Exception")), mWhat(message)
		{
			mFilename = filename;
			mLine = line;
		}

		INLINE const string& getName() const
		{
			return mName;
		}

		INLINE const string& getMessage() const
		{
			return mWhat;
		}

		INLINE const string& getFilename() const
		{
			return mFilename;
		}

		INLINE long getFileline() const
		{
			return mLine;
		}
	};

	struct InternalException : Exception
	{
		InternalException(string message, string filename = BEER_WIDEN(""), long line = 0) : Exception(message, filename, line)
		{
			mName = BEER_WIDEN("InternalException");
		}
	};

	struct GCException : InternalException
	{
		GCException(string message, string filename = BEER_WIDEN(""), long line = 0) : InternalException(message, filename, line)
		{
			mName = BEER_WIDEN("GCException");
		}
	};

	struct NotEnoughMemoryException : GCException
	{
		NotEnoughMemoryException(string message, string filename = BEER_WIDEN(""), long line = 0) : GCException(message, filename, line)
		{
			mName = BEER_WIDEN("NotEnoughMemoryException");
		}
	};

	struct CriticalAssertException : Exception
	{
		CriticalAssertException(string message, string filename = BEER_WIDEN(""), long line = 0) : Exception(message, filename, line)
		{
			mName = BEER_WIDEN("CriticalAssertException");
		}
	};

	struct RuntimeException : Exception
	{
		RuntimeException(string message, string filename = BEER_WIDEN(""), long line = 0) : Exception(message, filename, line)
		{
			mName = BEER_WIDEN("RuntimeException");
		}
	};

	struct ClassFileException : RuntimeException
	{
		ClassFileException(string message, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(message, filename, line)
		{
			mName = BEER_WIDEN("ClassFileException");
		}
	};

	struct RuntimeAssertException : RuntimeException
	{
		RuntimeAssertException(string message, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(message, filename, line)
		{
			mName = BEER_WIDEN("RuntimeAssertException");
		}
	};

	struct UnexpectedClassException : RuntimeException
	{
		UnexpectedClassException(string message, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(message, filename, line)
		{
			mName = BEER_WIDEN("UnexpectedClassException");
		}
	};

	class Object;
	class String;
	class Class;
	struct MethodNotFoundException : RuntimeException
	{
		MethodNotFoundException(Object* instance, Class* klass, String* selector, string filename = BEER_WIDEN(""), long line = 0);
	};

	class Method;
	struct AbstractMethodException : RuntimeException
	{
		AbstractMethodException(Method* method, string filename = BEER_WIDEN(""), long line = 0);
	};
	
	class Class;
	struct AbstractClassException : RuntimeException
	{
		AbstractClassException(Class* klass, string filename = BEER_WIDEN(""), long line = 0);
	};

	struct ClassNotFoundException : RuntimeException
	{
	protected:
		string mClassName;

	public:
		ClassNotFoundException(string name, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(string(BEER_WIDEN("Class not found: ")) + name, filename, line), mClassName(name)
		{
			mName = BEER_WIDEN("ClassNotFoundException");
		}

		string getClassName() const { return mClassName; }
	};

	struct CircularParentException : RuntimeException
	{
	protected:
		string mClassName;

	public:
		CircularParentException(string name, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(string(BEER_WIDEN("Class has circular refrence in parents: ")) + name, filename, line), mClassName(name)
		{
			mName = BEER_WIDEN("CircularParentException");
		}

		string getClassName() const { return mClassName; }
	};

	struct NullReferenceException : RuntimeException
	{
		NullReferenceException(string message, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(message, filename, line)
		{
			mName = BEER_WIDEN("NullReferenceException");
		}
	};

	struct BytecodeException : RuntimeException
	{
		BytecodeException(string message, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(message, filename, line)
		{
			mName = BEER_WIDEN("BytecodeException");
		}
	};

	struct IOException : RuntimeException
	{
		IOException(string msg, string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(msg, filename, line)
		{
			mName = BEER_WIDEN("IOException");
		}
	};

	struct StackOverflowException : RuntimeException
	{
		StackOverflowException(string msg = BEER_WIDEN(""), string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(msg, filename, line)
		{
			mName = BEER_WIDEN("StackOverflowEception");
		}
	};

	struct ArrayIndexOutOfBoundsException : RuntimeException
	{
		int64 requiredIndex;
		int64 arraySize;

		ArrayIndexOutOfBoundsException(int64 index, int64 size, string filename = BEER_WIDEN(""), long line = 0)
			: RuntimeException(BEER_WIDEN("Array index out of bounds ") /*+ index + BEER_WIDEN(" / ") + size*/, filename, line),
			requiredIndex(index), arraySize(size)
		{
			mName = BEER_WIDEN("ArrayIndexOutOfBoundsException");
		}
	};

	struct SchedulerException : RuntimeException
	{
		SchedulerException(string msg = BEER_WIDEN(""), string filename = BEER_WIDEN(""), long line = 0) : RuntimeException(msg, filename, line)
		{
			mName = BEER_WIDEN("SchedulerException");
		}
	};

	#define GCException(_msg_) GCException((_msg_), __WFILE__, __LINE__)
	#define NotEnoughMemoryException(_msg_) NotEnoughMemoryException((_msg_), __WFILE__, __LINE__)
	#define CriticalAssertException(_msg_) CriticalAssertException((_msg_), __WFILE__, __LINE__)
	#define RuntimeAssertException(_msg_) RuntimeAssertException((_msg_), __WFILE__, __LINE__)
	#define ClassFileException(_msg_) ClassFileException((_msg_), __WFILE__, __LINE__)
	#define BadCastException(_from_, _to_) BadCastException((_from_), (_to_), __WFILE__, __LINE__)
	#define UnexpectedClassException(_msg_) UnexpectedClassException((_msg_), __WFILE__, __LINE__)
	#define MethodNotFoundException(instance, klass, selector) MethodNotFoundException(instance, klass, selector, __WFILE__, __LINE__)
	#define AbstractMethodException(method) AbstractMethodException(method, __WFILE__, __LINE__)
	#define ClassNotFoundException(_msg_) ClassNotFoundException((_msg_), __WFILE__, __LINE__)
	#define CircularParentException(_msg_) CircularParentException((_msg_), __WFILE__, __LINE__)
	#define NullReferenceException(_msg_) NullReferenceException((_msg_), __WFILE__, __LINE__)
	#define BytecodeException(_msg_) BytecodeException((_msg_), __WFILE__, __LINE__)
	#define IOException(_msg_) IOException((_msg_), __WFILE__, __LINE__)
	#define StackOverflowException(_msg_) StackOverflowException((_msg_), __WFILE__, __LINE__)
	#define ArrayIndexOutOfBoundsException(index, size) ArrayIndexOutOfBoundsException(index, size, __WFILE__, __LINE__)
	#define SchedulerException(_msg_) SchedulerException((_msg_), __WFILE__, __LINE__)

	#ifdef BEER_DEBUG_MODE
		#define BEER_DEBUG_ASSERTS_ON
		//#define BEER_MEMORY_DEBUGGING
		//#define BEER_GC_DEBUGGING
		#define BEER_STACK_DEBUGGING
		#define BEER_BC_DEBUGGING
	#endif // BEER_DEBUG_MODE

	// debug asserts
	//#define BEER_DEBUG_ASSERTS_ON
	//#define BEER_MEMORY_DEBUGGING
	//#define BEER_GC_DEBUGGING
	//#define BEER_STACK_DEBUGGING

	// important asserts
	#define BEER_ASSERTS_ON

	#ifdef BEER_ASSERTS_ON
		#define BEER_RUNTIME_ASSERS_ON
		#define BEER_NULL_ASSERTS_ON
		#define BEER_BOUNDS_ASSERT_ON
		#define BEER_CRITICAL_ASSERTS_ON
	#endif // BEER_ASSERTS_ON

	// optimalizations
	#define BEER_INLINE_OPTIMALIZATION
	//#define BEER_INLINE_FRAMES
	#define BEER_MIC_OPTIMIZATION
	#define BEER_PIC_OPTIMIZATION

	// multi-threading
	#define BEER_BARRIER_READ_ON
	#define BEER_BARRIER_WRITE_ON

	// measure performance
	//#define BEER_MEASURE_PERFORMANCE
	//#define BEER_PROFILE_MONOCACHES
	//#define BEER_PROFILE_POLYCACHES

	// debugging
	//#define BEER_DEBUG_SAFE_POINTS


	// usually bytecode/virutal-runtime related errors
	// without these checks runtime should be faster, but less error-prone
	#ifdef BEER_RUNTIME_ASSERS_ON	
#define RUNTIME_ASSERT(_cond_, _msg_) if(!(_cond_)) { BEER_DBG_BREAKPOINT(); throw RuntimeAssertException((_msg_)); }
	#else
		#define RUNTIME_ASSERT(cond, msg)
	#endif

	// virtual machine crucial errors
	// without these checks runtime could work, but may behave unexpectedly (threads!)
	#ifdef BEER_CRITICAL_ASSERTS_ON	
		#define CRITICAL_ASSERT(_cond_, _msg_) if(!(_cond_)) { BEER_DBG_BREAKPOINT(); throw CriticalAssertException((_msg_)); }
	#else
		#define CRITICAL_ASSERT(cond, msg)
	#endif

	// bugs => should not happen in release!
	#ifdef BEER_DEBUG_ASSERTS_ON
		#define DBG_ASSERT(_cond_, _msg_) if(!(_cond_)) { BEER_DBG_BREAKPOINT(); throw CriticalAssertException((_msg_)); }
	#else
		#define DBG_ASSERT(cond, msg)
	#endif

	// null pointer checks => important!
	#ifdef BEER_NULL_ASSERTS_ON
		#define NULL_ASSERT(var) if(!(var)) { BEER_DBG_BREAKPOINT(); throw NullReferenceException(BEER_WIDEN("Object is null")); }
	#else
		#define NULL_ASSERT(var)
	#endif

	// array bounds check => important
	#ifdef BEER_BOUNDS_ASSERT_ON
	#define BOUNDS_ASSERT(index, size) { Integer::IntegerData __index = index; if(__index < 0 || __index >= size) { BEER_DBG_BREAKPOINT(); throw ArrayIndexOutOfBoundsException(__index, size); } }
	#else
		#define BOUNDS_ASSERT(index, size)
	#endif

	// debug info
	#ifdef _DEBUG
		#define DEBUG_INFO(_msg_) cout << "Debug: " << _msg_ << std::endl;
	#else
		#define DEBUG_INFO(_msg_)
	#endif

	// stack debugging
	#if defined(BEER_STACK_DEBUGGING)
		class Frame;
		
		struct DebugStackCheck
		{
			Frame* frame;
			int64 startIndex;

			DebugStackCheck(Frame* frame);
			~DebugStackCheck();
		};
		#define BEER_STACK_CHECK() DebugStackCheck __dbgStackCheck(frame);
	#else
		#define BEER_STACK_CHECK()
	#endif // BEER_STACK_DEBUGGING

	// static initializer
	#define STATIC_INITIALIZER_START(_name_)												\
		struct __si##_name_																	\
		{																					\
		static __si##_name_ __inst;															\
			__si##_name_()																	\
			{																				\

	#define STATIC_INITIALIZER_END(_name_)													\
			}																				\
		};																					\
		__si##_name_ __si##_name_::__inst;													\



	#define STATIC_INITIALIZER(_name_,  _body_) STATIC_INITIALIZER_START(_name_) {_body_} STATIC_INITIALIZER_END(_name_)
};