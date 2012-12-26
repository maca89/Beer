#pragma once
#include "prereq.h"
#include "Interlocked.h"


namespace Beer
{
	template <typename T>
	class InterlockedStack
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

	protected:
		Pointer mHead;

	public:
		INLINE InterlockedStack()
		{
			mHead.node = NULL;
		}

		INLINE ~InterlockedStack()
		{
			clear();
			SMART_DELETE(mHead.node);
		}

		NOINLINE void push(T item)
		{
			Pointer oldHead;
			Node* newNode = new Node;
			newNode->data = item;

			while(true)
			{
				oldHead = mHead;
				newNode->next = oldHead;
				if(CAS(&mHead, oldHead, Pointer(newNode, oldHead.counter + 1)))
				{
					break;
				}
			}
		}

		/**
		 * Poping from an empty stack is undefined
		 */
		NOINLINE T pop()
		{
			T result;
			Pointer oldHead;

			while(true)
			{
				oldHead = mHead;

				Pointer next = oldHead.node->next;
				result = oldHead.node->data;
				
				if(CAS(&mHead, oldHead, Pointer(next.node, oldHead.counter + 1)))
				{
					break;
				}
			}

			delete oldHead.node;
			return result;
		}

		INLINE T top()
		{
			return mHead.node->data;
		}

		INLINE bool empty() const
		{
			return mHead.node == NULL;
		}

		void clear()
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