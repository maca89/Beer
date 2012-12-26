#pragma once
#include "prereq.h"
//#include "CompareAndSwap.h"


namespace Beer
{
	class QuickSort
	{
	public:
		//template <typename T>
		struct Instance
		{
			void/*T*/* arr;
			size_t length;

			INLINE Instance() {}
			INLINE Instance(const Instance& inst) : arr(inst.arr), length(inst.length) {}
			INLINE Instance(void/*T*/* arr, size_t length) : arr(arr), length(length) {}
			INLINE Instance& operator=(const Instance& inst) { arr = inst.arr; length = inst.length; return *this; }
		};
		
		template <typename T>
		INLINE QuickSort(T* items, size_t length)
		{
			(*this)(items, length);
		}
		
		template <typename T>
		INLINE void operator()(T* items, size_t length)
		{
			iterative(items, length);
		}

		template <typename T>
		static INLINE void swap(T& t1, T& t2)
		{
			// ordinary
			std::swap(t1, t2);
			
			// CAS
			/*T bckp1 = t1;
			T bckp2 = t2;
			CompareAndSwap32(&t1, bckp1, bckp2);
			CompareAndSwap32(&t2, bckp2, bckp1);*/
		}

		template <typename T>
		static size_t partition(T* items, size_t length)
		{
			size_t pivot = length / 2;
			T pivotValue = items[pivot];

			swap(items[pivot], items[length - 1]); // move pivot to end

			size_t j = 0;
			for(size_t i = 0; i < length - 1; i++)
			{
				if(items[i] < pivotValue)
				{
					swap(items[i], items[j]);
					j++;
				}
			}

			swap(items[j], items[length - 1]); // move pivot to final place

			return j;
		}

		template <typename T>
		static NOINLINE void resursive(T* items, size_t length)
		{
			if(length <= 1) return;
		
			size_t pivotIndex = partition(items, length);
			resursive(items, pivotIndex);
			resursive(&items[pivotIndex + 1], length - (pivotIndex + 1));
		}

		template <typename T>
		static NOINLINE bool iterative_step(Instance& in, Instance& out1, Instance& out2)
		{
			if(in.length <= 1) return false;
			size_t pivotIndex = partition(reinterpret_cast<T*>(in.arr), in.length);
			out1 = Instance(in.arr, pivotIndex);
			out2 = Instance(&reinterpret_cast<T*>(in.arr)[pivotIndex + 1], in.length - (pivotIndex + 1));
			return true;
		}

		template <typename T>
		static NOINLINE void iterative(T* arr, size_t arrLength)
		{
			std::stack<Instance> stack;
			stack.push(Instance(arr, arrLength));

			while(!stack.empty())
			{
				Instance inst = stack.top();
				stack.pop();
		
				Instance lesser, bigger;
				if(!iterative_step<T>(inst, lesser, bigger))
				{
					continue;
				}

				stack.push(lesser);
				stack.push(bigger);
			}
		}

		/*template <typename T>
		static NOINLINE void parallel(T* arr, size_t arrLength)
		{
			std::stack<Instance> stack;
			stack.push(Instance(0, arrLength));

			while(!stack.empty())
			{
				Instance inst = stack.top();
				stack.pop();

				if(inst.length <= 1) continue;
		
				size_t pivotIndex = partition(&arr[inst.start], inst.length);

				stack.push(Instance(inst.start, pivotIndex));
				stack.push(Instance(inst.start + pivotIndex + 1, inst.length - (pivotIndex + 1)));
			}
		}*/


	};
};