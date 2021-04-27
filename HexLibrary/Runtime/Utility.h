#pragma once
#include "RuntimeAlias.h"

namespace RT
{
	template<class NodeT>
	inline void AppendToTwoWayLinkedList(NodeT*& head, NodeT*& previous, NodeT* current)
	{
		if (head == nullptr)
			head = previous = current;
		else
		{
			previous->Next = current;
			current->Prev = previous;
			previous = current;
		}
	}

	template<class NodeT, class Fn>
	inline void AppendToOneWayLinkedListOrdered(NodeT*& head, NodeT* current, Fn&& predicator)
	{
		if (head == nullptr)
			head = previous = current;
		else
		{
			NodeT* previous = nullptr;
			NodeT* iterator = head;
			while (iterator != nullptr)
			{
				if (predicator(current))
				{
					//Found position
					current->Next = iterator->Next;
					iterator->Next = current;
					return;
				}
				previous = iterator;
				iterator = iterator->Next;
			}
			//Not found, then insert to the end.
			previous->Next = current;
		}
	}
}