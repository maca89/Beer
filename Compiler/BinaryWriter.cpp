#include "stdafx.h"

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(std::stringstream & data)
{
	std::string string = data.str();

	mOut.write(string.c_str(), string.size());

	return string.size();
}

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(std::string data)
{
	mOut.write(data.c_str(), data.size() + 1);

	return data.size() + 1;
}

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(int8 data)
{
	mOut.write((const char *)&data, 1);

	return 1;
}

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(int16 data)
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
BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(int32 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(int64 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(uint8 data)
{
	mOut.write((const char *)&data, 1);

	return 1;
}

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(uint16 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(uint32 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(uint64 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(float32 data)
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

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::BinaryWriter::write(float64 data)
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