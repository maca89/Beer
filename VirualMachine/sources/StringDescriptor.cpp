#include "stdafx.h"
#include "StringDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"

using namespace Beer;


void StringDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	if(isMachineEncoded())
	{
		return;
	}

	// no need to convert flags

	// convert size
	format.convert(mSize);
	
	// convert data
	for(uint16 i = 0; i < getSize(); i++)
	{
		format.convert(reinterpret_cast<uint16&>((&mData)[i]));
	}

	/////------------------ <just a test>

	_locale_t locale = 0;
	errno_t state = 0;

	errno_t erange = ERANGE;
	errno_t einval = EINVAL;
	errno_t eilseq = EILSEQ;

	size_t mbs_size = mSize / sizeof(char16);
	//char8* mbs_data = new char8[mbs_size];//reinterpret_cast<char8*>(&mData);
	if(mbs_size >= 256) throw BytecodeException(BEER_WIDEN("String in classfile too long! Either create smaller string or properly implement UTF8 in classfile :-)"));
	char8 mbs_data[256];

	size_t wcs_size = mSize / sizeof(char16);
	char16* wcs_data = &mData;

	// convert double-byte-char mbs to one-byte-char mbs
	for(size_t i = 0; i < wcs_size; i++)
	{
		mbs_data[i] = static_cast<char8>(wcs_data[i]);
	}

	ERROR_INSUFFICIENT_BUFFER;

	// convert back
	size_t retsize = 0;
	//state = mbstowcs_s(&retsize, wcs_data, wcs_size - 1, mbs_data, mbs_size - 1);  // -1 for \0
	retsize = MultiByteToWideChar(CP_UTF8, 0, mbs_data, mbs_size * sizeof(char8), wcs_data, wcs_size);

	//delete[] mbs_data;

	if(retsize == 0)
	{
		RUNTIME_ASSERT(retsize > 0, BEER_WIDEN("Non UTF-16 string"));
	}

	RUNTIME_ASSERT(retsize > 0, BEER_WIDEN("Non UTF-16 string"));
	RUNTIME_ASSERT(state == 0, BEER_WIDEN("Non UTF-16 string"));
	CRITICAL_ASSERT(retsize <= mSize / sizeof(char16), BEER_WIDEN("Non UTF-16 string"));

	mSize = retsize * sizeof(char16);

	/////------------------ </just a test>

	markAsMachineEncoded();
}