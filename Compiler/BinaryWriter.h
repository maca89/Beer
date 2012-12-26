#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace CodeGeneration
	{
		typedef uint16 class_file_address;
		typedef uint16 count;

		class BinaryWriter
		{
		protected:

			std::ostream & mOut;
			bool mReverseBytes;

		public:

			BinaryWriter(std::ostream & out, bool reverseBytes = false) : mOut(out), mReverseBytes(reverseBytes) { }
			//BinaryWriter(bool reverseBytes = false) : mOut(std::stringstream()), mReverseBytes(reverseBytes) { }

			INLINE bool getReverseBytes() const { return mReverseBytes; }

			class_file_address write(std::stringstream & data);

			class_file_address write(std::string data);

			class_file_address write(int8 data);
			class_file_address write(int16 data);
			class_file_address write(int32 data);
			class_file_address write(int64 data);

			class_file_address write(uint8 data);
			class_file_address write(uint16 data);
			class_file_address write(uint32 data);
			class_file_address write(uint64 data);

			class_file_address write(float32 data);
			class_file_address write(float64 data);
		};
	}
}