#pragma once
namespace HL
{
	namespace System
	{
		namespace Concurrency
		{
			template<class T>
			class ConcurrentNodeWrapper
			{
			public:
				int ExternalCount = 0;//外部计数
				ConcurrentNode<T> *Ptr = nullptr;//指针
			};

			template<class T>
			class ConcurrentNode
			{
			public:
				template<typename = std::enable_if_t<std::is_copy_constructible_v<T>, int>>
				ConcurrentNode(T const&other) noexcept(std::is_nothrow_copy_constructible_v<T>)
					:Value(other), InternalCount(0) {}
				template<typename = std::enable_if_t<std::is_move_constructible_v<T>, int>>
				ConcurrentNode(T &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
					:Value(std::move(other)), InternalCount(0) {}
				template<typename = std::enable_if_t<std::is_default_constructible_v<T>, int>>
				ConcurrentNode() noexcept(std::is_nothrow_default_constructible_v<T>)
					:Value(nullptr), InternalCount(0) {}

				ConcurrentNodeWrapper<T> Next;
				Nullable::Optional<T> Value;
				std::atomic<int> InternalCount;
			};
			//并发栈
			template<class T>
			class ConcurrentStack
			{
				typedef ConcurrentNode<T> Node;
				typedef ConcurrentNodeWrapper<T> Wrapper;
				std::atomic<Wrapper> m_head;
				void IncHead(Wrapper &old)
				{
					Wrapper new_wrapper;
					do
					{
						new_wrapper = old;
						new_wrapper.ExternalCount++;
					} while (!m_head.compare_exchange_strong(old, new_wrapper, std::memory_order_acquire, std::memory_order_relaxed));
					old.ExternalCount = new_wrapper.ExternalCount;
				}
			public:
				ConcurrentStack() {
					m_head.store(Wrapper());
				}
				ConcurrentStack(ConcurrentStack const&) = delete;
				ConcurrentStack&operator=(ConcurrentStack const&) = delete;
				void Push(T const&Target)
				{
					Wrapper wrapper;
					wrapper.ExternalCount = 1;
					wrapper.Ptr = new Node(Target);
					wrapper.Ptr->Next = m_head.load(std::memory_order_relaxed);
					while (!m_head.compare_exchange_weak(wrapper.Ptr->Next, wrapper, std::memory_order_release, std::memory_order_relaxed));
				}
				Nullable::Optional<T> Pop()
				{
					Nullable::Optional<T> ret;
					Wrapper old_head = m_head.load(std::memory_order_relaxed);
					for (;;)
					{
						IncHead(old_head);
						Node*node = old_head.Ptr;
						if (!node)
							return nullptr;
						if (m_head.compare_exchange_strong(old_head, node->Next, std::memory_order_relaxed))
						{
							ret = std::move(node->Value);
							int count_inc = old_head.ExternalCount - 2;
							if (node->InternalCount.fetch_add(count_inc, std::memory_order_release) == -count_inc)//计数add之后为0时,即之前为负的inc
								delete node;
							return ret;
						}
						else if (node->InternalCount.fetch_sub(1, std::memory_order_relaxed) == 1)
						{
							node->InternalCount.load(std::memory_order_acquire);
							delete node;
						}
					}
				}
				~ConcurrentStack() {
					while (!Pop().IsNull());
				}
			};
		}
	}
}