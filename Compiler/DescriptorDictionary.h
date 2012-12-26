#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class DescriptorInfo;

		typedef std::list<DescriptorInfo *> DescriptorList;
		typedef std::list<DescriptorInfo *>::iterator DescriptorListIterator;

		typedef std::map<std::string, StringConstant *> StringConstantMap;
		typedef std::map<std::string, StringConstant *>::iterator StringConstantMapIterator;

		class DescriptorDictionary
		{
		protected:

			descriptor_id mId;
			DescriptorList mDescriptors;
			StringConstantMap mStringDescriptors;
			
		public:

			DescriptorDictionary() : mId(0) { }

			INLINE descriptor_id getCount() { return mDescriptors.size(); }
			
			INLINE DescriptorList * getDescriptors() { return &mDescriptors; }
			INLINE StringConstantMap * getStringDescriptors() { return &mStringDescriptors; }
			
			StringConstant * getStringDescriptor(std::string value);

			StringConstant * addStringConstant(std::string value);

			INLINE void addDescriptor(DescriptorInfo * descriptor)
			{
				descriptor->setDescriptorId(mId++);
				mDescriptors.push_back(descriptor);
			}
		};
	}
}
