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

				};
			}
		}
	}
}