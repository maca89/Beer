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

		struct ReferenceMap
		{
		public:

			typedef std::map<Object**, GCObject*>::iterator iterator;
			typedef std::map<Object**, GCObject*>::const_iterator const_iterator;

		protected:

			std::map<Object**, GCObject*> mRefs;

		public:

			ReferenceMap()
			{
			}

			ReferenceMap(Object** reference, GCObject* referent)
			{
				mRefs[reference] = referent;
			}

			size_t count() const
			{
				return mRefs.size();
			}

			void addReference(Object** reference, GCObject* referent)
			{
				mRefs[reference] = referent;
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

			ReferenceMap* forwardMap() const;
		};

		typedef std::map<GCObject*, ReferenceMap*>::iterator iterator;
		typedef std::map<GCObject*, ReferenceMap*>::const_iterator const_iterator;

	protected:

		std::map<GCObject*, ReferenceMap*> mSet;

	public:
		
		INLINE RememberedSet()
		{
		}

		INLINE ~RememberedSet()
		{
		}

		void add(GCObject* obj, Object** reference, GCObject* referent)
		{
			iterator it = mSet.find(obj);

			if (it == mSet.end())
			{
				it->second->addReference(reference, referent);
			}
			else
			{
				mSet[obj] = new ReferenceMap(reference, referent);
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

		RememberedSet* forwardObjects();
		RememberedSet* forwardSet() const;		
	};
};