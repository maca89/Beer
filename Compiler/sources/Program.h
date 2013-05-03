#pragma once

#include "prereq.h"
#include "Class.h"
#include "DescriptorDictionary.h"

namespace Beer
{
	class Output;

	namespace SemanticAnalysis
	{
		typedef std::list<Class *> ClassList;
		typedef std::map<std::string, Class *> ClassMap;
		typedef int16 length;

		class Program
		{
		protected:

			DescriptorDictionary mDictionary;
			ClassList mExternalClasses;
			ClassList mClasses;
			ClassMap mClassMap;

			bool mContainsEP;

		public:

											Program();
											~Program();

			INLINE	bool					containsEntryPoint() const
			{
				return mContainsEP;
			}

			INLINE	void					containsEntryPoint(bool contains)
			{
				mContainsEP = contains;
			}

			INLINE	DescriptorDictionary *	getDescriptorDictionary() { return &mDictionary; }

			INLINE	ClassList *				getExternalClasses() { return &mExternalClasses; }
			INLINE	ClassList *				getClasses() { return &mClasses; }
			INLINE	ClassMap *				getClassMap() { return &mClassMap; }

					Class *					getClass(std::string name);
						
					Class *					addClass(std::string name, Class::flags flags, Output & out);
		};
	}
}