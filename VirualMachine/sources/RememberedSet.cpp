#include "stdafx.h"
#include "RememeberdSet.h"
#include "GCObject.h"

using namespace Beer;

RememberedSet::ReferenceMap* RememberedSet::ReferenceMap::forwardMap() const
{
	ReferenceMap* newRefMap = new ReferenceMap();

	for (const_iterator it = mRefs.begin(); it != mRefs.end(); it++)
	{
		GCObject* newReferent = it->second->getPtr();
		Object** newReference = reinterpret_cast<Object**>(reinterpret_cast<byte*>(newReferent) + (reinterpret_cast<byte*>(it->first) - reinterpret_cast<byte*>(it->second)));

		newRefMap->addReference(newReference, newReferent);
	}

	return newRefMap;
}

RememberedSet* RememberedSet::forwardObjects()
{
	RememberedSet* newRemSet = new RememberedSet();

	for (iterator it = mSet.begin(); it != mSet.end(); it++)
	{
		for (ReferenceMap::iterator rit = it->second->begin(); rit != it->second->end(); rit++)
		{
			(*rit->first) = it->first->getPtr()->getObject();
		}

		newRemSet->mSet[it->first->getPtr()] = it->second;
	}

	return newRemSet;
}

RememberedSet* RememberedSet::forwardSet() const
{
	RememberedSet* newRemSet = new RememberedSet();

	for (const_iterator it = mSet.begin(); it != mSet.end(); it++)
	{
		ReferenceMap* newRefMap = it->second->forwardMap();

		delete it->second;

		newRemSet->mSet[it->first->getPtr()] = newRefMap;
	}

	return newRemSet;
}