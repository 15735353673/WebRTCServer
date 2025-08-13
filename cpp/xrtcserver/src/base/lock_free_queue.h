/*************************************************************************
	> File Name: src/base/lock_free_queue.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 07 Jul 2025 06:24:20 PM CST
 ************************************************************************/

#ifndef _BASE_LOCK_FREE_QUEUE_H_
#define _BASE_LOCK_FREE_QUEUE_H_

#include<iostream>
#include<atomic>
namespace xrtc
{
	//一个生产者一个消费者
	//指针的操作是原子的
	template <typename T>
	class LockFreeQueue
	{
	public:
		LockFreeQueue()
		{
			first=last=divider=new Node(T());
			_size=0;
		}

		~LockFreeQueue()
		{
			while(first)
			{
				Node* temp=first;
				first=first->next;
				delete temp;
			}

			_size=0;
		}

		void produce(const T& t)
		{
			last->next = new Node(t);
			last=last->next;
			_size++;

			//清理
			while(first != divider)
			{
				Node* temp=first;
				first=first->next;
				delete temp;
			}
		}

		bool consume(T* result)
		{
			if(divider != last)
			{
				*result=divider->next->value;
				divider=divider->next;
				--_size;
				return true;
			}
			return false;
		}

		bool empty()
		{
			return _size==0;
		}

		bool size()
		{
			return _size;
		}

	private:
		struct Node
		{
			T value;
			Node* next;

			Node(const T& value):value(value),next(nullptr){}
		};

		Node* first;
		Node* divider;
		Node* last;
		std::atomic<int> _size;
	};

}//namespace xrtc
#endif //_BASE_LOCK_FREE_QUEUE_H_
