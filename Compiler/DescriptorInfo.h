#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef uint16 descriptor_id;
		typedef uint16 descriptor_address;

		class DescriptorInfo
		{
		protected:

			descriptor_id mDescriptorId;
			descriptor_address mAddress;			
			
		public:

			INLINE DescriptorInfo() : mDescriptorId(0), mAddress(0) {} 

			INLINE descriptor_id getDescriptorId() const { return mDescriptorId;}
			INLINE descriptor_address getAddress() const { return mAddress;}
			
			INLINE void setDescriptorId(descriptor_id id) { mDescriptorId = id; }
			INLINE void setAddress(descriptor_address address) { mAddress = address; }
		};
	}
}