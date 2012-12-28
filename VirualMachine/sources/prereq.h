#pragma once


namespace Beer
{
#ifdef _DEBUG
	#define INLINE
	#define NOINLINE
	#define BEER_DEBUG_MODE
#else
	#define INLINE __inline
	#define NOINLINE
#endif

	#define BEER_CALL __cdecl

	#define lengthof(arr) sizeof(arr) / sizeof(arr[0])

	#define SMART_DELETE(_i_) delete (_i_); (_i_) = NULL;
	#define SMART_DELETE_ARR(_i_) delete[] (_i_); (_i_) = NULL;

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

	//void ZeroMemory(void* mem, size_t size);

	struct Exception : std::exception
	{
	protected:
		std::string mName;
		std::string mFilename;
		long mLine;

	public:
		Exception(std::string message, std::string filename = "", long line = 0) : std::exception(message.c_str()), mName("Exception")
		{
			mFilename = filename;
			mLine = line;
		}

		std::string getName() const
		{
			return mName;
		}

		std::string getMessage() const
		{
			return what();
		}

		std::string getFilename() const
		{
			return mFilename;
		}

		long getFileline() const
		{
			return mLine;
		}
	};

	struct InternalException : Exception
	{
		InternalException(std::string message, std::string filename = "", long line = 0) : Exception(message, filename, line)
		{
			mName = "InternalException";
		}
	};

	struct GCException : InternalException
	{
		GCException(std::string message, std::string filename = "", long line = 0) : InternalException(message, filename, line)
		{
			mName = "GCException";
		}
	};

	struct NotEnoughMemoryException : GCException
	{
		NotEnoughMemoryException(std::string message, std::string filename = "", long line = 0) : GCException(message, filename, line)
		{
			mName = "NotEnoughMemoryException";
		}
	};

	struct CriticalAssertException : Exception
	{
		CriticalAssertException(std::string message, std::string filename = "", long line = 0) : Exception(message, filename, line)
		{
			mName = "CriticalAssertException";
		}
	};

	struct RuntimeException : Exception
	{
		RuntimeException(std::string message, std::string filename = "", long line = 0) : Exception(message, filename, line)
		{
			mName = "RuntimeException";
		}
	};

	struct ClassFileException : RuntimeException
	{
		ClassFileException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "ClassFileException";
		}
	};

	struct RuntimeAssertException : RuntimeException
	{
		RuntimeAssertException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "RuntimeAssertException";
		}
	};

	struct UnexpectedClassException : RuntimeException
	{
		UnexpectedClassException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "UnexpectedClassException";
		}
	};

	struct MethodNotFoundException : RuntimeException
	{
		MethodNotFoundException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "MethodNotFoundException";
		}
	};

	struct ClassNotFoundException : RuntimeException
	{
	protected:
		std::string mClassName;

	public:
		ClassNotFoundException(std::string name, std::string filename = "", long line = 0) : RuntimeException(std::string("Class not found: ") + name, filename, line), mClassName(name)
		{
			mName = "ClassNotFoundException";
		}

		std::string getClassName() const { return mClassName; }
	};

	struct CircularParentException : RuntimeException
	{
	protected:
		std::string mClassName;

	public:
		CircularParentException(std::string name, std::string filename = "", long line = 0) : RuntimeException(std::string("Class has circular refrence in parents: ") + name, filename, line), mClassName(name)
		{
			mName = "CircularParentException";
		}

		std::string getClassName() const { return mClassName; }
	};

	struct NullReferenceException : RuntimeException
	{
		NullReferenceException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "NullReferenceException";
		}
	};

	struct BytecodeException : RuntimeException
	{
		BytecodeException(std::string message, std::string filename = "", long line = 0) : RuntimeException(message, filename, line)
		{
			mName = "BytecodeException";
		}
	};

	struct IOException : RuntimeException
	{
		IOException(std::string msg, std::string filename = "", long line = 0) : RuntimeException(msg, filename, line)
		{
			mName = "IOException";
		}
	};

	struct IOFileException : IOException
	{
		IOFileException(std::string msg, std::string filename = "", long line = 0) : IOException(msg, filename, line)
		{
			mName = "IOFileException";
		}
	};

	#define GCException(_msg_) GCException((_msg_), __FILE__, __LINE__)
	#define NotEnoughMemoryException(_msg_) NotEnoughMemoryException((_msg_), __FILE__, __LINE__)
	#define CriticalAssertException(_msg_) CriticalAssertException((_msg_), __FILE__, __LINE__)
	#define RuntimeAssertException(_msg_) RuntimeAssertException((_msg_), __FILE__, __LINE__)
	#define ClassFileException(_msg_) ClassFileException((_msg_), __FILE__, __LINE__)
	#define BadCastException(_from_, _to_) BadCastException((_from_), (_to_), __FILE__, __LINE__)
	#define UnexpectedClassException(_msg_) UnexpectedClassException((_msg_), __FILE__, __LINE__)
	#define MethodNotFoundException(_msg_) MethodNotFoundException((_msg_), __FILE__, __LINE__)
	#define ClassNotFoundException(_msg_) ClassNotFoundException((_msg_), __FILE__, __LINE__)
	#define CircularParentException(_msg_) CircularParentException((_msg_), __FILE__, __LINE__)
	#define NullReferenceException(_msg_) NullReferenceException((_msg_), __FILE__, __LINE__)
	#define BytecodeException(_msg_) BytecodeException((_msg_), __FILE__, __LINE__)
	#define IOException(_msg_) IOException((_msg_), __FILE__, __LINE__)
	#define IOFileException(_msg_) IOFileException((_msg_), __FILE__, __LINE__)


	#ifdef BEER_DEBUG_MODE
		#define BEER_ASSERTS_ON
	#endif // BEER_DEBUG_MODE

	#ifdef BEER_ASSERTS_ON
		#define BEER_DEBUG_ASSERTS_ON
	#endif // BEER_ASSERTS_ON

	
	#define BEER_RUNTIME_ASSERS_ON
	#define BEER_NULL_ASSERTS_ON
	#define BEER_BOUNDS_ASSERT_ON
	#define BEER_CRITICAL_ASSERTS_ON


	// usually bytecode/virutal-runtime related errors
	// without these checks runtime should be faster, but less error-prone
	#ifdef BEER_RUNTIME_ASSERS_ON	
		#define RUNTIME_ASSERT(_cond_, _msg_) if(!(_cond_)) throw RuntimeAssertException((_msg_));
	#else
		#define RUNTIME_ASSERT(cond, msg)
	#endif

	// virtual machine crucial errors
	// without these checks runtime could work, but may behave unexpectedly (threads!)
	#ifdef BEER_CRITICAL_ASSERTS_ON	
		#define CRITICAL_ASSERT(_cond_, _msg_) if(!(_cond_)) throw CriticalAssertException((_msg_));
	#else
		#define CRITICAL_ASSERT(cond, msg)
	#endif

	// bugs => should not happen in release!
	#ifdef BEER_DEBUG_ASSERTS_ON
		#define DBG_ASSERT(_cond_, _msg_) if(!(_cond_)) throw CriticalAssertException((_msg_));
	#else
		#define DBG_ASSERT(cond, msg)
	#endif

	// checks => should be in debug for performance reasons
	#ifdef BEER_NULL_ASSERTS_ON
		#define NULL_ASSERT(var) if(!(var))														\
		{																						\
			throw NullReferenceException("Object is null");										\
		}
	#else
		#define NULL_ASSERT(var)
	#endif

	// debug info
	#ifdef _DEBUG
		#define DEBUG_INFO(_msg_) std::cout << "Debug: " << _msg_ << std::endl;
	#else
		#define DEBUG_INFO(_msg_)
	#endif

	// optimalizations
	#define BEER_INLINE_OPTIMALIZATION

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