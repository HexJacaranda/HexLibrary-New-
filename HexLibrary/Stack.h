#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Generic
			{
				template<class U>
				class StackNode
				{
				public:
					U Object;
					StackNode* Next = nullptr;
					
					StackNode(StackNode const&) = default;
					StackNode(StackNode&&) = default;
					template<class O, typename = std::enable_if_t<
						std::is_constructible_v<U, O>
						, int>>
						StackNode(O&& other)
						noexcept(std::is_nothrow_constructible_v<U, O>)
						: Object(std::forward<O>(other)) {}
					template<typename = std::enable_if_t<
						std::is_copy_constructible_v<U>
						,int>>
						StackNode(U const&target)
						noexcept(std::is_nothrow_copy_constructible_v<U>)
						: Object(target) {}
					template<typename = std::enable_if_t<
						std::is_move_constructible_v<U>
						,int>>
						StackNode(U&& target)
						noexcept(std::is_nothrow_move_constructible_v<U>)
						: Object(std::move(target)) {}
					StackNode& operator=(StackNode const&) = default;
					StackNode& operator=(StackNode &&) = default;
					~StackNode() = default;
				};
				template<class U>
				class StackIterator
				{
					StackNode<U>* m_node;
				public:
					StackIterator(StackNode<U>*Node) :m_node(Node) {}
					StackIterator(StackIterator&&) = default;
					StackIterator(StackIterator const&) = default;
					StackIterator& operator=(StackIterator&&) = default;
					StackIterator& operator=(StackIterator const&) = default;
					StackIterator&operator++() {
						m_node = m_node->Next;
						return *this;
					}
					inline U& operator*()noexcept {
						return m_node->Object;
					}
					inline U const& operator*()const noexcept {
						return m_node->Object;
					}
					inline bool operator==(StackIterator const&other)noexcept {
						return m_node == other.m_node;
					}
					inline bool operator!=(StackIterator const&other)noexcept {
						return m_node != other.m_node;
					}
				};
				template<class U>
				class Stack
				{
					typedef StackNode<U> Node;
					Node* m_node = nullptr;
					void From(Node* target) {
						if (target == nullptr)
							return;
						m_node = new Node(*target);
						target = target->Next;
						Node* previous = m_node;
						while (target != nullptr)
						{
							Node* node = new Node(*target);
							previous->Next = node;
							target = target->Next;
						}
					}
				public:
					Stack()noexcept = default;
					Stack(Stack&&)noexcept = default;
					Stack(Stack const&rhs) {
						From(rhs->m_node);
					}
					Stack& operator=(Stack const& rhs) {
						Clear();
						From(rhs.m_node);
						return *this;
					}
					Stack& operator=(Stack&& lhs) {
						Clear();
						Utility::move_assign(m_node, lhs.m_node);
						return *this;
					}
					~Stack() {
						if (m_node != nullptr)
						{
							while (!IsEmpty())
								Pop();
							m_node = nullptr;
						}
					}
					template<class O>
					void Push(O&&Object) {
						if (m_node == nullptr)
							m_node = new Node(std::forward<O>(Object));
						else
						{
							Node* new_one = new Node(std::forward<O>(Object));
							new_one->Next = m_node;
							m_node = new_one;
						}
					}
					inline U const& Top()const {
						return this->m_node->Object;
					}
					inline U& Top() {
						return this->m_node->Object;
					}
					bool Pop() {
						if (this->m_node != nullptr)
						{
							Node* out = this->m_node;
							this->m_node = this->m_node->Next;
							delete out;
							return true;
						}
						return false;
					}
					inline StackIterator<U> begin()noexcept {
						return m_node;
					}
					inline StackIterator<U> end()noexcept {
						return nullptr;
					}
					inline StackIterator<U> begin()const noexcept {
						return (Node*)m_node;
					}
					inline StackIterator<U> end()const noexcept {
						return nullptr;
					}
					inline void Clear() {
						while (!Pop());
					}
					inline bool IsEmpty()const {
						return m_node == nullptr;
					}
				};
			}
		}
	}
}