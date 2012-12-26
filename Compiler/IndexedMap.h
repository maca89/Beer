#include "stdafx.h"

namespace BeerlangCompiler
{
	template <class T, class U>
	class IndexedMap
	{
	protected:

		std::vector<U> mVector;
		std::map<T, U> mMap;

	public:

		size_t getCount() const
		{
			return mVector.size();
		}

		bool containsKey(T & t)
		{
			return mMap.find(t) != mMap.end();
		}

		void add(T & t, U & u)
		{
			mVector.push_back(u);
			mMap[t] = u;
		}

		U & get(size_t index)
		{
			if (index >= mVector.size()) throw std::exception("Index out of bounds.");

			return mVector[index];
		}

		U & get(T & t)
		{
			std::map<T, U>::iterator it = mMap.find(t);

			if (it == mMap.end()) throw std::exception("Key not found.");

			return it.second;
		}
	};
}