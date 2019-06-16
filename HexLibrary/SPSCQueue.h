#pragma once
namespace HL
{
	namespace System
	{
		namespace Concurrency
		{
			template<class U>
			struct QueueNode
			{
				Nullable::Optional<U> Value;
				QueueNode* Next = nullptr;
				template<typename = std::enable_if_t<std::is_copy_constructible_v<U>,int>>
				QueueNode(U const&other) noexcept(std::is_nothrow_copy_constructible_v<U>)
					:Value(other) {}
				template<typename = std::enable_if_t<std::is_move_constructible_v<U>, int>>
				QueueNode(U &&other) noexcept(std::is_nothrow_move_constructible_v<U>)
					:Value(std::move(other)) {}
				template<typename = std::enable_if_t<std::is_default_constructible_v<U>, int>>
				QueueNode() noexcept(std::is_nothrow_default_constructible_v<U>)
					:Value(nullptr) {}
			};

			template<class U>
			class SPSCQueue
			{
				typedef QueueNode<U> Node;
				std::atomic<Node*> m_head;
				std::atomic<Node*> m_tail;
				std::atomic<size_t> m_cnt;
			public:
				SPSCQueue() {
					m_head.store(new Node, std::memory_order_relaxed);
					m_tail.store(m_head.load(std::memory_order_relaxed), std::memory_order_relaxed);
					m_cnt.store(0, std::memory_order_relaxed);
				}
				SPSCQueue(SPSCQueue const&) = delete;
				SPSCQueue(SPSCQueue&&) = default;
				SPSCQueue&operator=(SPSCQueue const&) = delete;
				SPSCQueue&operator=(SPSCQueue&&) = delete;
				~SPSCQueue() {
					Node* head = m_head.load(std::memory_order_acquire);
					while (head != nullptr)
					{
						m_head.store(head->Next, std::memory_order_release);
						delete head;
						head = m_head.load(std::memory_order_acquire);
					}
				}
				void Enqueue(U const&Value) {
					Node* ptr = new Node;
					Node* old = m_tail.load(std::memory_order_acquire);
					old->Value = Value;
					old->Next = ptr;
					m_tail.store(ptr, std::memory_order_release);
					m_cnt.fetch_add(1, std::memory_order_acq_rel);
				}
				Nullable::Optional<U> Dequeue() {
					Node* node = m_head.load(std::memory_order_acquire);
					if (node == m_tail.load(std::memory_order_acquire))
						return nullptr;
					m_head.store(node->Next, std::memory_order_release);
					m_cnt.fetch_sub(1, std::memory_order_acq_rel);
					Nullable::Optional<U> ret(std::move(node->Value));
					delete node;
					return std::move(ret);
				}
				inline bool Empty()const {
					return m_cnt.load(std::memory_order_acquire) == 0;
				}
			};
		}
	}
}