#pragma once

#include "prereq.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		/*
		 * This class represents a descriptor which is used in code generation phase.
		 * It stores only id and address in class file.
		 */
		class Descriptor
		{
		public:

			typedef uint16 descriptor_id;
			typedef uint16 descriptor_address;

		protected:

			descriptor_id		mDescriptorId;
			descriptor_address	mAddress;			
			
		public:

			INLINE Descriptor()
				:	mDescriptorId(0),
					mAddress(0)
			{ } 


			virtual ~Descriptor()
			{ }
			

			INLINE descriptor_id getDescriptorId() const
			{
				return mDescriptorId;
			}
			
			INLINE descriptor_address getAddress() const
			{
				return mAddress;
			}
			

			INLINE void	setDescriptorId(descriptor_id id)
			{
				mDescriptorId = id;
			}

			INLINE void	setAddress(descriptor_address address)
			{
				mAddress = address;
			}
		};
	}
}