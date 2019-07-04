#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Tree
			{
				template<class TKey>
				struct AVLNode
				{
					typedef TKey ElementType;
					AVLNode* Left = nullptr;
					AVLNode* Right = nullptr;
					AVLNode* Parent = nullptr;
					size_t Height = 1;
					TKey Key;

					template<typename = std::enable_if_t<std::is_copy_constructible_v<TKey>,int>>
					AVLNode(TKey const&key) noexcept(std::is_nothrow_copy_constructible_v<TKey>)
						:Key(key) {}
					template<typename = std::enable_if_t<std::is_move_constructible_v<TKey>, int>>
					AVLNode(TKey &&key) noexcept(std::is_nothrow_move_constructible_v<TKey>)
						:Key(std::move(key)) {}
					template<class...Args,typename = std::enable_if_t<std::is_constructible_v<TKey,Args...>,int>>
					AVLNode(Args&&...args) noexcept(std::is_nothrow_constructible_v<TKey, Args...>)
						:Key(std::forward<Args>(args)...) {}
					AVLNode() = default;
					AVLNode(AVLNode const&) = default;
					AVLNode(AVLNode&&) = default;
					AVLNode& operator=(AVLNode const&) = default;
					AVLNode& operator=(AVLNode&&) = default;
					~AVLNode() = default;

					static inline size_t GetHeight(AVLNode* node)noexcept {
						return node == nullptr ? 0U : node->Height;
					}
					static constexpr size_t const& Max(size_t const& left, size_t const& right)noexcept {
						return left > right ? left : right;
					}
					inline void ReevaluateHeight()noexcept {
						Height = Max(GetHeight(Left), GetHeight(Right)) + 1;
					}
					inline intptr_t BalanceFactor() {
						return (intptr_t)GetHeight(Left) - (intptr_t)GetHeight(Right);
					}
				};


				template<class TKey,class ComparatorT = DefaultComparator<TKey>>
				class AVLTree
				{
					typedef AVLNode<TKey> NodeT;
					NodeT* m_root = nullptr;
					ComparatorT m_comparator;
					size_t m_count = 0;
					static inline bool IsLeft(NodeT* node)noexcept {
						return node == node->Parent->Left;
					}
					void RotateLeft(NodeT* Target)
					{
						NodeT* adjacent = Target->Right;
						Target->Right = adjacent->Left;
						if (adjacent->Left != nullptr)
							adjacent->Left->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (IsLeft(Target))
							Target->Parent->Left = adjacent;
						else
							Target->Parent->Right = adjacent;
						adjacent->Left = Target;
						Target->Parent = adjacent;

						adjacent->ReevaluateHeight();
						Target->ReevaluateHeight();
					}
					void RotateRight(NodeT* Target)
					{
						NodeT* adjacent = Target->Left;
						Target->Left = adjacent->Right;
						if (adjacent->Right != nullptr)
							adjacent->Right->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (IsLeft(Target))
							Target->Parent->Left = adjacent;
						else
							Target->Parent->Right = adjacent;
						adjacent->Right = Target;
						Target->Parent = adjacent;

						adjacent->ReevaluateHeight();
						Target->ReevaluateHeight();
					}					
					NodeT* GetTry(TKey const&key, int&last_val)
					{
						NodeT* target = nullptr;
						NodeT* try_node = m_root;
						while (try_node != nullptr)
						{
							target = try_node;
							last_val = m_comparator(key, try_node->Key);
							if (last_val < 0)
								try_node = try_node->Left;
							else if (last_val > 0)
								try_node = try_node->Right;
							else
								return nullptr;
						}
						return target;
					}
					template<class Other>
					NodeT* Get(Other const&key)
					{
						NodeT* iterator = m_root;
						while (iterator != nullptr)
						{
							int val = m_comparator(key, iterator->Key);
							if (val > 0)
								iterator = iterator->Right;
							else if (val < 0)
								iterator = iterator->Left;
							else
								return iterator;
						}
						return nullptr;
					}
					bool PrePut(TKey const&key, int& value,NodeT*& out)
					{
						NodeT* target = GetTry(key, value);
						out = target;
						return target == nullptr && m_root != nullptr;
					}
					void Put(TKey const&key, NodeT* node, NodeT* target, int val)
					{
						node->Parent = target;
						if (m_root == nullptr)
							m_root = node;
						else
						{
							if (val > 0)
								target->Right = node;
							else if (val < 0)
								target->Left = node;
						}
						Rebalance(node->Parent);
					}
					NodeT* Successor(NodeT* node)
					{
						NodeT* iterator = node->Left;
						while (iterator->Right != nullptr)
							iterator = iterator->Right;
						return iterator;
					}
					void Delete(NodeT* node)
					{
						NodeT* to_fix = nullptr;
						if (node->Left == nullptr || node->Left == nullptr)
						{
							NodeT* child = node->Left == nullptr ? node->Right : node->Left;
							if (node->Parent == nullptr)
								m_root = child;
							else
							{
								if (IsLeft(node))
									node->Parent->Left = child;
								else
									node->Parent->Right = child;
							}
							if (child != nullptr)
								child->Parent = node->Parent;
							to_fix = node->Parent;
							delete node;
						}
						else
						{
							NodeT* successor = Successor(node);
							node->Key = std::move(successor->Key);
							if (IsLeft(successor))
								successor->Parent->Left = nullptr;
							else
								successor->Parent->Right = nullptr;
							to_fix = successor->Parent;
							delete successor;
						}
						Rebalance(to_fix);
					}
					void Rebalance(NodeT* target)
					{
						NodeT* iterator = target;
						while (iterator != nullptr)
						{
							iterator->ReevaluateHeight();
							intptr_t balance = iterator->BalanceFactor();
							if (balance > 1)
							{
								if (iterator->Left->BalanceFactor() < 0)
									RotateLeft(iterator->Left);
								RotateRight(iterator);
							}
							else if (balance < -1)
							{
								if (iterator->Right->BalanceFactor() > 0)
									RotateRight(iterator->Right);
								RotateLeft(iterator);
							}
							iterator = iterator->Parent;
						}
					}
					void Erase(NodeT* root)
					{
						if (root == nullptr)
							return;
						for (NodeT* lnode = root; lnode != nullptr; root = lnode)
						{
							Erase(lnode->Right);
							lnode = lnode->Left;
							delete root;
						}
					}
					void ConstructTree(NodeT*& left, NodeT* right)
					{
						if (right == nullptr)
						{
							left = nullptr;
							return;
						}
						left = new NodeT(*right);
						ConstructTree(left->Left, right->Left);
						ConstructTree(left->Right, right->Right);
						if (left->Left != nullptr)
							left->Left->Parent = left;
						if (left->Right != nullptr)
							left->Right->Parent = left;
					}
				public:
					template<class FunT, typename =
						std::enable_if_t<std::is_constructible_v<ComparatorT, FunT>, int>>
						AVLTree(FunT && comparator) noexcept(std::is_nothrow_constructible_v<ComparatorT, FunT>)
						:m_comparator(std::forward<FunT>(comparator)) {}

					template<typename = std::enable_if_t<std::is_default_constructible_v<ComparatorT>, int>>
					AVLTree() noexcept(std::is_nothrow_default_constructible_v<ComparatorT>)
						:m_comparator() {}
					AVLTree(AVLTree&&) = default;
					AVLTree(AVLTree const&rhs)
						:m_count(rhs.m_count), m_comparator(rhs.m_comparator) {
						ConstructTree(m_root, rhs.m_root);
					}
					AVLTree& operator=(AVLTree const&rhs) {
						Erase(m_root);
						ConstructTree(m_root, rhs.m_root);
						m_count = rhs.m_count;
						m_comparator = rhs.m_comparator;
						return *this;
					}
					AVLTree& operator=(AVLTree&&lhs)
					{
						Erase(m_root);
						Utility::move_assign(m_root, lhs.m_root);
						Utility::move_assign(m_count, lhs.m_count);
						Utility::move_assign(m_comparator, lhs.m_comparator);
						return *this;
					}
					~AVLTree() {
						Clear();
					}

					void Add(TKey const&key) {
						int value = 0;
						NodeT* target = nullptr;
						if (PrePut(key, value, target))
							throw DuplicateKeyException();
						else
						{
							Put(key, new NodeT(key), target, value);
							m_count++;
						}
					}
					void Add(TKey &&key)
					{
						int value = 0;
						NodeT* target = nullptr;
						if (PrePut(key, value, target))
							throw DuplicateKeyException();
						else
						{
							Put(key, new NodeT(std::move(key)), target, value);
							m_count++;
						}
					}
					template<class Other>
					TKey & GetValue(Other const&key)
					{
						NodeT* target = Get(key);
						if (target == nullptr)
							throw KeyNotFoundException();
						return target->Key;
					}
					template<class Other>
					TKey const& GetValue(Other const&key)const
					{
						NodeT* target = Get(key);
						if (target == nullptr)
							throw KeyNotFoundException();
						return target->Key;
					}
					void Remove(TKey const&key) {
						NodeT* target = Get(key);
						if (target == nullptr)
							throw KeyNotFoundException();
						Delete(target);
						m_count--;
					}
					template<class Other>
					void Remove(Other const&key) {
						NodeT* target = Get(key);
						if (target == nullptr)
							throw KeyNotFoundException();
						Delete(target);
						m_count--;
					}
					TKey const& Max()const {
						if (m_root == nullptr)
							throw EmptySetException();
						return TreeMax(m_root)->Key;
					}
					TKey & Max() {
						if (m_root == nullptr)
							throw EmptySetException();
						return TreeMax(m_root)->Key;
					}
					TKey const& Min()const {
						if (m_root == nullptr)
							throw EmptySetException();
						return TreeMin(m_root)->Key;
					}
					TKey & Min() {
						if (m_root == nullptr)
							throw EmptySetException();
						return TreeMin(m_root)->Key;
					}
					inline size_t Count()const noexcept {
						return this->m_count;
					}
					void Clear() {
						if (m_root != nullptr)
						{
							Erase(m_root);
							m_root = nullptr;
							m_count = 0;
						}
					}
					public:
					typedef TreeIterator<AVLNode<TKey>> IteratorT;
					typedef TreeConstIterator<AVLNode<TKey>> ConstIteratorT;
					IteratorT begin() {
						return IteratorT(TreeMin(m_root), nullptr);
					}
					IteratorT begin()const {
						return ConstIteratorT(TreeMin(m_root), nullptr);
					}
					IteratorT end() {
						return IteratorT(nullptr, nullptr);
					}
					ConstIteratorT end()const {
						return ConstIteratorT(nullptr, nullptr);
					}
				};
			}
		}
	}
}