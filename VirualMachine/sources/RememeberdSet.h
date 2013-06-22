#pragma once

#include "prereq.h"
#include <map>

namespace Beer
{
	class GCObject;
	class Object;
	
	class RememberedSet
	{
	public:

		struct References
		{
		public:

			typedef std::set<Object**>::iterator iterator;

		protected:

			std::set<Object**> mRefs;

		public:

			References(Object** ref)
			{
				mRefs.insert(ref);
			}

			size_t count() const
			{
				return mRefs.size();
			}

			void addReference(Object** ref)
			{
				mRefs.insert(ref);
			}

			void removeReference(Object** ref)
			{
				mRefs.erase(ref);
			}

			INLINE iterator begin()
			{
				return mRefs.begin();
			}

			INLINE iterator end()
			{
				return mRefs.end();
			}
		};

		typedef std::map<GCObject*, References*>::iterator iterator;

	protected:

		std::map<GCObject*, References*> mSet;

	public:
		
		INLINE RememberedSet()
		{
		}

		INLINE ~RememberedSet()
		{
		}

		void add(GCObject* obj, Object** ref)
		{
			iterator it = mSet.find(obj);

			if (it == mSet.end())
			{
				it->second->addReference(ref);
			}
			else
			{
				mSet[obj] = new References(ref);
			}
		}

		void remove(GCObject* obj)
		{
			iterator it = mSet.find(obj);

			if (it == mSet.end())
			{
				delete it->second;
			}

			mSet.erase(it);
		}

		void remove(GCObject* obj, Object** ref)
		{
			iterator it = mSet.find(obj);

			if (it == mSet.end())
			{
				it->second->removeReference(ref);

				if (it->second->count() == 0)
				{
					delete it->second;

					mSet.erase(it);
				}
			}			
		}

		INLINE iterator begin()
		{
			return mSet.begin();
		}

		INLINE iterator end()
		{
			return mSet.end();
		}

		INLINE void erase(const iterator& it)
		{
			mSet.erase(it);
		}
	};
};