#include "stdafx.h"

#include "BinaryWriter.h"

using namespace Beer::CodeGeneration;

class_file_address BinaryWriter::write(std::stringstream & data)
{
	std::string string = data.str();

	mOut.write(string.c_str(), string.size());

	return string.size();
}

class_file_address BinaryWriter::write(std::string data)
{
	const char * str = data.c_str();
	size_t length = data.length();

	if (mReverseBytes)
	{
		for (size_t i = 0; i <= length; i++)
		{
			mOut.write(str + length, 1);
			mOut.write(str + i, 1);
		}
	}
	else
	{
		for (size_t i = 0; i <= length; i++)
		{
			mOut.write(str + i, 1);
			mOut.write(str + length, 1);
		}
	}

	return (length + 1) * 2;
}

class_file_address BinaryWriter::write(int8 data)
{
	mOut.write((const char *)&data, 1);

	return 1;
}

class_file_address BinaryWriter::write(int16 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 2);
	}

	return 2;
}
class_file_address BinaryWriter::write(int32 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 4);
	}

	return 4;
}

class_file_address BinaryWriter::write(int64 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 7, 1);
		mOut.write((const char *)&data + 6, 1);
		mOut.write((const char *)&data + 5, 1);
		mOut.write((const char *)&data + 4, 1);
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 8);
	}

	return 8;
}

class_file_address BinaryWriter::write(uint8 data)
{
	mOut.write((const char *)&data, 1);

	return 1;
}

class_file_address BinaryWriter::write(uint16 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 2);
	}

	return 2;
}

class_file_address BinaryWriter::write(uint32 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 4);
	}

	return 4;
}

class_file_address BinaryWriter::write(uint64 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 7, 1);
		mOut.write((const char *)&data + 6, 1);
		mOut.write((const char *)&data + 5, 1);
		mOut.write((const char *)&data + 4, 1);
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 8);
	}

	return 8;
}

class_file_address BinaryWriter::write(float32 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 4);
	}

	return 4;
}

class_file_address BinaryWriter::write(float64 data)
{
	if (mReverseBytes)
	{
		mOut.write((const char *)&data + 7, 1);
		mOut.write((const char *)&data + 6, 1);
		mOut.write((const char *)&data + 5, 1);
		mOut.write((const char *)&data + 4, 1);
		mOut.write((const char *)&data + 3, 1);
		mOut.write((const char *)&data + 2, 1);
		mOut.write((const char *)&data + 1, 1);
		mOut.write((const char *)&data, 1);
	}
	else
	{
		mOut.write((const char *)&data, 8);
	}

	return 8;
}