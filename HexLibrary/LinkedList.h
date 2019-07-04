#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Generic
			{
				template<class T>
				struct LinkedListNode
				{
					T Value;
					LinkedListNode* Next = nullptr;
					LinkedListNode* Prev = nullptr;
					template<typename = std::enable_if_t<std::is_copy_constructible_v<T>,int>>
					LinkedListNode(T const&object) noexcept(std::is_nothrow_copy_constructible_v<T>)
						:Value(object) {}
					template<typename = std::enable_if_t<std::is_move_constructible_v<T>, int>>
					LinkedListNode(T &&object) noexcept(std::is_nothrow_move_constructible_v<T>)
						:Value(std::move(object)) {}
					template<typename = std::enable_if_t<std::is_default_constructible_v<T>, int>>
					LinkedListNode() noexcept(std::is_nothrow_default_constructible_v<T>)
						:Value() {}
				};

				template<class T>
				class LinkedList
				{
				private:
					typedef LinkedListNode<T> NodeT;
					NodeT* m_head = nullptr;
					NodeT* m_tail = nullptr;
				public:
					
					LinkedList(){}
					LinkedList(LinkedList const&) {}
					LinkedList(LinkedList&&)noexcept {}
					LinkedList& operator=(LinkedList const&) {}
					LinkedList& operator=(LinkedList&&) {}
					~LinkedList() {}
					void Remove(T const& key) {

					}
					void Remove() {}
				};
			}
		}
	}
}