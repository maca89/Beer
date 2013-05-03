#pragma once

#include "prereq.h"
#include "Descriptor.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class StringDescriptor;

		typedef std::list<Descriptor *> DescriptorList;

		typedef std::map<std::string, StringDescriptor *> StringDescriptorMap;

		/*
		 * This class aggregates descriptors which are used in code generation phase. There is a
		 * descriptor for every class, method, member, method parameter and every string that needs
		 * to be generated into class file.
		 *
		 * Class maintains special collection for storing string descriptors. This is used for sharing
		 * string descriptors so there will not be more string descriptors for one string. This reduces
		 * size of final class file.
		 */
		class DescriptorDictionary
		{
		protected:

			Descriptor::descriptor_id	mId; // ID which will be assigned to new descriptor info
			DescriptorList				mDescriptors; // List of desriptor infos
			DescriptorList				mTempDescriptors; // List of descriptors which will not be generated into class file
			StringDescriptorMap			mStringDescriptors; // Map of string constant which u
			
		public:

			DescriptorDictionary()
				:	mId(0)
			{ }

			~DescriptorDictionary();

			INLINE Descriptor::descriptor_id getCount() const
			{
				return mDescriptors.size();
			}

			INLINE DescriptorList * getDescriptors()
			{
				return &mDescriptors;
			}

			INLINE StringDescriptorMap * getStringDescriptors()
			{
				return &mStringDescriptors;
			}

			StringDescriptor * getStringDescriptor(const std::string & value);

			StringDescriptor * addStringDescriptor(const std::string & value);

			void removeStringDescriptor(const std::string & value);

			INLINE void addDescriptor(Descriptor * descriptor, bool temp = true)
			{
				descriptor->setDescriptorId(mId++);

				if (temp) mDescriptors.push_back(descriptor);
				else mTempDescriptors.push_back(descriptor);
			}

			INLINE void removeDescriptor(Descriptor * descriptor)
			{
				mDescriptors.remove(descriptor);
				delete descriptor;
			}
		};
	}
}
