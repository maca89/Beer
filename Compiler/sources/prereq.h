#pragma once

#define INLINE __inline

#define VERSION 1

#define FIRST_CLASS_ID 1
#define FIRST_MEMBER_ID 0
#define FIRST_FUNCTION_ID 1
#define FIRST_PARAMETER_ID 0

#ifdef _DEBUG
#define ASSERT(condition, message)										\
    do {																\
        if (! (condition)) {											\
            std::cout << message << " in " << __FILE__					\
                      << " line " << __LINE__ << std::endl;				\
			__asm { int 3 }												\
			system("PAUSE");											\
			std::exit(EXIT_FAILURE);									\
        }																\
    } while (false)
#else
#define ASSERT(condition, message)
#endif

typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;