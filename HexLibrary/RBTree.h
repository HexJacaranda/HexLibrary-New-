#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			class DuplicateKeyException : public Exception::IException
			{
			public:
				DuplicateKeyException() :Exception::IException(L"Duplicate key") {}
			};
			class KeyNotFoundException : public Exception::IException
			{
			public:
				KeyNotFoundException() :Exception::IException(L"Key not found") {}
			};
			class EmptySetException : public Exception::IException
			{
			public:
				EmptySetException() :Exception::IException(L"Empty set") {}
			};
			namespace Tree
			{
				template<class Node>
				Node* TreeMax(Node* root, Node* Nil = nullptr)noexcept
				{
					if (root == nullptr)
						return nullptr;
					while (root->Right != Nil)
						root = root->Right;
					return root;
				}
				template<class Node>
				Node* TreeMin(Node* root, Node* Nil = nullptr)noexcept
				{
					if (root == nullptr)
						return nullptr;
					while (root->Left != Nil)
						root = root->Left;
					return root;
				}

				template<class NodeT>
				class TreeIterator
				{
					NodeT* m_current = nullptr;
					NodeT* m_nil = nullptr;
					typedef typename NodeT::ElementType ElementType;
				public:
					TreeIterator(NodeT* root, NodeT* nil)noexcept
						:m_current(root), m_nil(nil) {}
					TreeIterator(TreeIterator const&)noexcept = default;
					TreeIterator(TreeIterator &&)noexcept = default;
					TreeIterator& operator=(TreeIterator const&)noexcept = default;
					TreeIterator& operator=(TreeIterator &&)noexcept = default;
					~TreeIterator() = default;
					TreeIterator& operator++() {
						if (m_current->Right == m_nil)
						{
							NodeT* ptr = m_current->Parent;
							while (ptr != nullptr && ptr->Right == m_current)
							{
								m_current = ptr;
								ptr = ptr->Parent;
							}
							m_current = ptr;
						}
						else
							m_current = TreeMin(m_current->Right, m_nil);
						return *this;
					}
					TreeIterator operator++(int) {
						TreeIterator temp = ++(*this);
						return temp;
					}
					inline ElementType& operator*() {
						return m_current->Get();
					}
					inline bool operator==(TreeIterator const& other)noexcept {
						return m_current == other.m_current;
					}
					inline bool operator!=(TreeIterator const& other)noexcept {
						return m_current != other.m_current;
					}
				};
				template<class NodeT>
				class TreeConstIterator
				{
					NodeT* m_current = nullptr;
					NodeT* m_nil = nullptr;
					typedef typename NodeT::ElementType ElementType;
				public:
					TreeConstIterator(NodeT const* root, NodeT* nil)noexcept
						:m_current(root), m_nil(nil) {}
					TreeConstIterator(TreeConstIterator const&)noexcept = default;
					TreeConstIterator(TreeConstIterator&&)noexcept = default;
					TreeConstIterator& operator=(TreeConstIterator const&)noexcept = default;
					TreeConstIterator& operator=(TreeConstIterator&&)noexcept = default;
					~TreeConstIterator() = default;
					TreeConstIterator& operator++() {
						if (m_current->Right == m_nil)
						{
							NodeT* ptr = m_current->Parent;
							while (ptr != nullptr && ptr->Right == m_current)
							{
								m_current = ptr;
								ptr = ptr->Parent;
							}
							m_current = ptr;
						}
						else
							m_current = TreeMin(m_current->Right, m_nil);
						return *this;
					}
					TreeConstIterator operator++(int) {
						TreeIterator temp = ++(*this);
						return temp;
					}
					inline ElementType const& operator*() {
						return m_current->Get();
					}
					inline bool operator==(TreeConstIterator const& other)noexcept {
						return m_current == other.m_current;
					}
					inline bool operator!=(TreeConstIterator const& other)noexcept {
						return m_current != other.m_current;
					}
				};

				enum class NodeColor :bool
				{
					Red,
					Black
				};
				template<class TKey>
				struct Node
				{
					typedef TKey ElementType;
					Node* Left = nullptr;
					Node* Right = nullptr;
					Node* Parent = nullptr;
					NodeColor Color = NodeColor::Black;
					Nullable::Nullable<TKey> Key;

					Node() = default;
					Node(NodeColor color) :Color(color) {}
					Node(Node const&) = default;
					Node(Node&&) = default;
					Node& operator=(Node const&) = default;
					Node& operator=(Node&&) = default;
					~Node() = default;

					inline TKey const&Get()const {
						return Key.GetObject();
					}
					inline TKey &Get() {
						return Key.GetObject();
					}
				};

				template<class T>
				struct DefaultComparator
				{
					inline int operator()(T const&left, T const& right)noexcept
					{
						if (left > right)
							return 1;
						else if (left < right)
							return -1;
						else
							return 0;
					}
				};

				template<class TKey, class ComparatorT = DefaultComparator<TKey>>
				class RBTree
				{
					typedef Node<TKey> NodeT;
					NodeT* m_root = nullptr;
					ComparatorT m_comparator;
					size_t m_count = 0;
					static NodeT NilNode;
				private:
					inline static bool IsNil(NodeT* target)noexcept {
						return target == &NilNode;
					}
					inline static NodeT* Nil()noexcept {
						return &NilNode;
					}
					void RotateLeft(NodeT* Target) {
						NodeT* adjacent = Target->Right;
						Target->Right = adjacent->Left;
						if (adjacent->Left != Nil())
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
					}
					void RotateRight(NodeT* Target) {
						NodeT* adjacent = Target->Left;
						Target->Left = adjacent->Right;
						if (adjacent->Right != Nil())
							adjacent->Right->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (!IsLeft(Target))
							Target->Parent->Right = adjacent;
						else
							Target->Parent->Left = adjacent;
						adjacent->Right = Target;
						Target->Parent = adjacent;
					}
					template<class Other>
					NodeT* GetNode(Other const& key)const {
						NodeT* iterator = m_root;
						while (iterator != Nil() && iterator != nullptr)
						{
							int val = const_cast<ComparatorT&>(m_comparator)(key, iterator->Key.GetObject());
							if (val < 0)
								iterator = iterator->Left;
							else if (val > 0)
								iterator = iterator->Right;
							else
								return iterator;
						}
						return nullptr;
					}
					NodeT* GetTry(TKey const& key, int& last_val)
					{
						NodeT* target = nullptr;
						NodeT* try_node = m_root;
						while (try_node != Nil() && try_node != nullptr)
						{
							target = try_node;
							last_val = m_comparator(key, try_node->Key.GetObject());
							if (last_val < 0)
								try_node = try_node->Left;
							else if (last_val > 0)
								try_node = try_node->Right;
							else
								return nullptr;
						}
						return target;
					}
					NodeT* Put(TKey const& key, NodeT* node) {
						int val = 0;
						NodeT* target = GetTry(key, val);
						node->Color = NodeColor::Red;
						node->Left = Nil();
						node->Right = Nil();
						node->Parent = target;
						if (target == nullptr)
						{
							if (m_root == nullptr)
								m_root = node;
							else
								return nullptr;
						}
						else
						{
							if (val < 0)
								target->Left = node;
							else if (val > 0)
								target->Right = node;
							else
								return nullptr;
						}
						FixUpInsert(node);
						return node;
					}
					static inline bool IsLeft(NodeT* target)noexcept {
						return target->Parent->Left == target;
					}
					void FixUpInsert(NodeT* node) {
						while (node->Parent != nullptr&&node->Parent->Color == NodeColor::Red)
						{
							if (IsLeft(node->Parent))
							{
								NodeT* uncle = node->Parent->Parent->Right;
								if (uncle != Nil() && uncle->Color == NodeColor::Red)
								{
									node->Parent->Color = NodeColor::Black;
									uncle->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									node = node->Parent->Parent;
								}
								else
								{
									if (!IsLeft(node))
									{
										node = node->Parent;
										RotateLeft(node);
									}
									node->Parent->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									RotateRight(node->Parent->Parent);
								}
							}
							else
							{
								NodeT* uncle = node->Parent->Parent->Left;
								if (uncle != Nil() && uncle->Color == NodeColor::Red)
								{
									node->Parent->Color = NodeColor::Black;
									uncle->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									node = node->Parent->Parent;
								}
								else
								{
									if (IsLeft(node))
									{
										node = node->Parent;
										RotateRight(node);
									}
									node->Parent->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									RotateLeft(node->Parent->Parent);
								}
							}
						}
						m_root->Color = NodeColor::Black;
					}
					void FixUpDeletion(NodeT* node) {
						while (node != m_root && node->Color == NodeColor::Black)
						{
							NodeT* aux = nullptr;
							if (IsLeft(node))
							{
								aux = node->Parent->Right;
								if (aux->Color == NodeColor::Red)
								{
									aux->Color = NodeColor::Black;
									node->Parent->Color = NodeColor::Red;
									RotateLeft(node->Parent);
									aux = node->Parent->Right;
								}
								if ((aux->Left != nullptr || aux->Left->Color == NodeColor::Black) &&
									(aux->Right != nullptr || aux->Right->Color == NodeColor::Black))
								{
									aux->Color = NodeColor::Red;
									node = node->Parent;
								}
								else 
								{
									if ((aux->Right != nullptr || aux->Right->Color == NodeColor::Black))
									{
										aux->Left->Color = NodeColor::Black;
										aux->Color = NodeColor::Red;
										RotateRight(aux);
										aux = node->Parent->Right;
									}
									aux->Color = node->Parent->Color;
									node->Parent->Color = NodeColor::Black;
									aux->Right->Color = NodeColor::Black;
									RotateLeft(node->Parent);
									node = m_root;
								}
							}
							else
							{
								aux = node->Parent->Left;
								if (aux->Color == NodeColor::Red)
								{
									aux->Color = NodeColor::Black;
									node->Parent->Color = NodeColor::Red;
									RotateRight(node->Parent);
									aux = node->Parent->Left;
								}
								if ((aux->Left != nullptr || aux->Left->Color == NodeColor::Black) &&
									(aux->Right != nullptr || aux->Right->Color == NodeColor::Black))
								{
									aux->Color = NodeColor::Red;
									node = node->Parent;
								}
								else 
								{
									if ((aux->Left != nullptr || aux->Left->Color == NodeColor::Black))
									{
										aux->Right->Color = NodeColor::Black;
										aux->Color = NodeColor::Red;
										RotateLeft(aux);
										aux = node->Parent->Left;
									}
									aux->Color = node->Parent->Color;
									node->Parent->Color = NodeColor::Black;
									aux->Left->Color = NodeColor::Black;
									RotateRight(node->Parent);
									node = m_root;
								}
							}
						}
						node->Color = NodeColor::Black;
					}
					NodeT* GetSuccessor(NodeT* node) {
						if (node == nullptr)
							return node;
						else if (node->Right != Nil())
						{
							node = node->Right;
							while (node->Left != Nil())
								node = node->Left;
							return node;
						}
						else
						{
							NodeT* parent = node->Parent;
							while (parent != nullptr &&node == parent->Right)
							{
								node = parent;
								parent = parent->Parent;
							}
							return parent;
						}
						return nullptr;
					}
					NodeT* Delete(NodeT* node) {
						NodeT* qnode = nullptr;
						NodeT* tnode = nullptr;
						if (node->Left == Nil() || node->Right == Nil())
							qnode = node;
						else
							qnode = GetSuccessor(node);
						if (qnode->Left != Nil())
							tnode = qnode->Left;
						else
							tnode = qnode->Right;
						tnode->Parent = qnode->Parent;
						if (qnode->Parent == nullptr)
							m_root = tnode;
						else if (IsLeft(qnode))
							qnode->Parent->Left = tnode;
						else
							qnode->Parent->Right = tnode;
						if (qnode != node)
							node->Key = std::move(qnode->Key);
						if (qnode->Color == NodeColor::Black)
							FixUpDeletion(tnode);
						return qnode;
					}
					void Erase(NodeT* root)
					{
						if (root == nullptr)
							return;
						for (NodeT* lnode = root; lnode != Nil(); root = lnode)
						{
							Erase(lnode->Right);
							lnode = lnode->Left;
							delete root;
						}
					}
					void ConstructTree(NodeT*& left, NodeT* right)
					{
						if (right == Nil())
						{
							left = Nil();
							return;
						}
						left = new NodeT(*right);
						ConstructTree(left->Left, right->Left);
						ConstructTree(left->Right, right->Right);
						if (left->Left != Nil())
							left->Left->Parent = left;
						if (left->Right != Nil())
							left->Right->Parent = left;
					}
				public:
					template<class FunT, typename =
						std::enable_if_t<std::is_constructible_v<ComparatorT, FunT>, int>>
						RBTree(FunT && comparator) noexcept(std::is_nothrow_constructible_v<ComparatorT, FunT>)
						:m_comparator(std::forward<FunT>(comparator)) {}
					template<typename = std::enable_if_t<std::is_default_constructible_v<ComparatorT>, int>>
					RBTree() noexcept(std::is_nothrow_default_constructible_v<ComparatorT>)
						:m_comparator() {}
					RBTree(RBTree const& rhs)
						:m_count(rhs.m_count), m_comparator(rhs.m_comparator) {
						ConstructTree(m_root, rhs.m_root);
					}
					RBTree(RBTree&&) = default;
					RBTree& operator=(RBTree const& rhs) {
						Erase(m_root);
						ConstructTree(m_root, rhs.m_root);
						m_count = rhs.m_count;
						m_comparator = rhs.m_comparator;
						return *this;
					}
					RBTree& operator=(RBTree&& lhs) {
						Erase(m_root);
						Utility::move_assign(m_root, lhs.m_root);
						Utility::move_assign(m_count, lhs.m_count);
						Utility::move_assign(m_comparator, lhs.m_comparator);
						return *this;
					}
					~RBTree() {
						if (m_root != nullptr)
						{
							Erase(m_root);
							m_root = nullptr;
						}
					}

					void Remove(TKey const&key) {
						NodeT* target = GetNode(key);
						if (target == nullptr)
							return;
						delete Delete(target);
						m_count--;
					}
					template<class Other>
					void Remove(Other const&key) {
						NodeT* target = GetNode(key);
						if (target == nullptr)
							return;
						delete Delete(target);
						m_count--;
					}
					template<class Other>
					TKey const& GetValue(Other const& other)const {
						NodeT* target = GetNode(other);
						if (target == nullptr) {
							//ex
						}
						return target->Key.GetObject();
					}
					template<class Other>
					TKey & GetValue(Other const& other) {
						NodeT* target = GetNode(other);
						if (target == nullptr) {
							//ex
						}
						return target->Key.GetObject();
					}
					void Add(TKey const&key) {
						NodeT* node = new NodeT();
						if (Put(key, node) == nullptr)
							delete node;
						else
						{
							node->Key.Construct(key);
							m_count++;
						}
					}
					void Add(TKey &&key) {
						NodeT* node = new NodeT();
						if (Put(key, node) == nullptr)
							delete node;
						else
						{
							node->Key.Construct(std::move(key));
							m_count++;
						}
					}
					inline bool Contains(TKey const&key)const noexcept {
						return GetNode(key) != nullptr;
					}
					TKey const& Max()const {
						NodeT* iterator = TreeMax(m_root);
						return iterator->Key;
					}
					TKey const& Min()const {
						NodeT* iterator = TreeMin(m_root);
						return iterator->Key;
					}
					TKey &Min() {
						NodeT* iterator = TreeMin(m_root);
						return iterator->Key;
					}
					TKey &Max() {
						NodeT* iterator = TreeMax(m_root);
						return iterator->Key;
					}
					inline size_t Count()const noexcept {
						return m_count;
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
					typedef TreeIterator<Node<TKey>> IteratorT;
					IteratorT begin() {
						return IteratorT(TreeMin(m_root, Nil()), Nil());
					}
					IteratorT begin()const {
						return IteratorT(TreeMin(m_root, Nil()), Nil());
					}
					IteratorT end()const {
						return IteratorT(nullptr, Nil());
					}
				};

				template<class TKey, class ComparatorT>
				Node<TKey> RBTree<TKey, ComparatorT>::NilNode = Node<TKey>(NodeColor::Black);
			}
		}
	}
}