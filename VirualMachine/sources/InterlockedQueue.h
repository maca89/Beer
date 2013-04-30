#pragma once
#include "prereq.h"
#include "Interlocked.h"


namespace Beer
{
	template <typename T>
	class InterlockedQueue
	{
	public:
		struct Pointer;
		struct Node;

		struct Node
		{
			T data;
			Pointer next;

			INLINE Node() {}
			INLINE Node(T data) : data(data) {}
			INLINE Node(T data, const Pointer& next) : data(data), next(next) {}
			INLINE ~Node() {}
		};

		// 8bytes
		struct Pointer
		{
			Node* node;
			int32 counter;
		
			INLINE Pointer() : node(NULL), counter(0) {}
			INLINE Pointer(Node* node, int32 counter) : node(node), counter(counter) {}
			INLINE ~Pointer() {}
		};

		class iterator
		{
		protected:
			Pointer mCurrent;

		public:
			INLINE iterator()
			{
			}

			INLINE iterator(const Pointer& start) : mCurrent(start)
			{
			}
		
			INLINE iterator(const iterator& it) : mCurrent(it.mCurrent)
			{
			}

			INLINE ~iterator()
			{
			}
		
			INLINE const iterator& operator=(const iterator& it)
			{
				mCurrent = it.mCurrent;
			}

			INLINE bool operator==(const iterator& it)
			{
				return (mCurrent.node == it.mCurrent.node);
			}

			INLINE bool operator!=(const iterator& it)
			{
				return !operator==(it);
			}

			INLINE const iterator& operator++(int)
			{
				mCurrent = mCurrent.node->next;
				return *this;
			}

			INLINE const T& operator*() const
			{
				return mCurrent.node->next.node->data;
			}
		};

	protected:
		Pointer mHead;
		Pointer mTail;

	public:
		INLINE InterlockedQueue()
		{
			mTail.node = mHead.node = new Node;
		}

		INLINE ~InterlockedQueue()
		{
			clear();
			//TODO: SMART_DELETE(mHead.node);
		}

		NOINLINE void push(T item)
		{
			Node* newNode = new Node;
			newNode->data = item;

			Pointer oldTail;

			while(true)
			{
				oldTail = mTail;
				Pointer newTail = oldTail.node->next;

				if(oldTail.node == mTail.node && oldTail.counter == mTail.counter)
				{
					if(newTail.node == NULL)
					{
						if(CAS(&oldTail.node->next, newTail, Pointer(newNode, newTail.counter + 1)))
						{
							break;
						}
					}
					else
					{
						CAS(&mTail, oldTail, Pointer(newTail.node, oldTail.counter + 1));
					}
				}
			}

			CAS(&mTail, oldTail, Pointer(newNode, oldTail.counter + 1));
		}

		/**
		 * Dequeuing from an empty queue is undefined
		 */
		NOINLINE T pop()
		{
			T result;
			Pointer oldHead;

			while(true)
			{
				oldHead = mHead;
				Pointer oldTail = mTail;
				Pointer next = oldHead.node->next;

				if(oldHead.node == mHead.node && oldHead.counter == mHead.counter) // really???
				{
					if(oldHead.node == oldTail.node)
					{
						if(next.node == NULL)
						{
							return T(); // empty queue, undefined behaviour
						}
						CAS(&mTail, oldTail, Pointer(next.node, oldTail.counter + 1)); // tail is weird, fix it
					}
					else
					{
						result = next.node->data;
						if(CAS(&mHead, oldHead, Pointer(next.node, oldHead.counter + 1)))
						{
							break;
						}
					}
				}
			}

			delete oldHead.node;
			return result;
		}

		INLINE iterator begin() const
		{
			//if(mHead.node == mTail.node && mHead.node->next.node == NULL)
			if(empty()) return iterator();//mHead.node->next);
			return iterator(mHead);
		}
	
		INLINE iterator end() const
		{
			if(empty()) return iterator();//mTail.node->next);
			return iterator(mTail);
		}

		INLINE bool empty() const
		{
			DBG_ASSERT(mTail.node, BEER_WIDEN("Queue is empty"));
			return mHead.node == mTail.node && mHead.node->next.node == NULL;
		}

		NOINLINE void clear()
		{
			while(!empty())
			{
				pop();
			}
		}

	protected:

		INLINE bool CAS(Pointer* dst, Pointer& oldValue, Pointer& newValue)
		{
			if(Interlocked::CompareAndSwap32(reinterpret_cast<int32*>(&dst->node), reinterpret_cast<int32>(oldValue.node), reinterpret_cast<int32>(newValue.node)))
			{
				dst->counter = newValue.counter;
				return true;
			}
			return false;
		}
	};
};