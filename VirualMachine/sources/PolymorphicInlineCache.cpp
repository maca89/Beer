#include "stdafx.h"
#include "PolymorphicInlineCache.h"
#include "Class.h"
#include "Method.h"

using namespace Beer;


//#define BEER_INLINE_CACHE_VERBOSE

#ifdef BEER_INLINE_CACHE_VERBOSE
#	define INLINE_CACHE_DEBUG(msg) cout << "// InlineCache: " << msg << std::endl;
#else
#	define INLINE_CACHE_DEBUG(msg)
#endif


Method* PolymorphicInlineCache::find(Class* klass, String* selector, uint16 methodsLength)
{
	DBG_ASSERT(klass != NULL, BEER_WIDEN("Class is NULL"));

	// search the cache
	for(uint8 i = 0; i < methodsLength; i++)
	{
		CachedMethod* cachedMethod = &getMethods()[i];
		Class* cachedClass = cachedMethod->klass;

		if(cachedClass == NULL)
		{
			// end of cached block
			break;
		}
		if(cachedClass == klass)
		{
			// we found cached method
			INLINE_CACHE_DEBUG(BEER_WIDEN("Found ") << selector << " for " << cachedClass->getName() << " at " << static_cast<int32>(i));
			return cachedMethod->method;
		}
	}

	// we must do a lookup
	Method* method = klass->findMethod(selector);
					
	// save method to cache
	if(method && methodsLength > 0)
	{
		// move all cached and pop the last one
		for(uint8 i = methodsLength - 1; i > 0; i--) // *NOT* i >= 0 !!!
		{
			getMethods()[i] = getMethods()[i - 1];
		}

		// save at the top
		getMethods()[0].klass = klass;
		getMethods()[0].method = method;
		INLINE_CACHE_DEBUG(BEER_WIDEN("Saved ") << selector << " for " << klass->getName() << " to 0");
	}

	return method;
}