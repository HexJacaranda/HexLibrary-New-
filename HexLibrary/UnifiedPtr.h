#pragma once
namespace HL
{
	namespace System
	{
		namespace Pointer
		{

			template<bool IsVoid,class Replace,class Target>
			struct GetReference
			{
				typedef Target& Type;
				static inline Target& Get(Target* ptr)noexcept {
					return *ptr;
				}
			};
			template<class Replace, class Target>
			struct GetReference<true, Replace, Target>
			{
				typedef Replace Type;
				static inline Replace Get(Target* ptr)noexcept {
					return Replace();
				}
			};

			template<class T>
			struct IteratorInterface
			{
				typedef void_t* IteratorT;
				typedef const void_t* ConstIteratorT;
			};

			struct ResourceActions
			{
				typedef void*(*ReleaseCallType)(void*);
				typedef void*(*UpdateCallType)(void*);
				typedef bool(*QueryCallType)(void*);
				typedef void*(*CloneCallType)(void*);
				ReleaseCallType Release = nullptr;
				UpdateCallType Update = nullptr;
				QueryCallType IsValid = nullptr;
				CloneCallType Clone = nullptr;
			};
			struct Resource
			{
				size_t WeakReference = 0;
				void* ResourcePtr = nullptr;
			};

			template<class T>
			class me
			{
				friend struct me_setter;
			protected:
				weakptr<T> This;
			};

			struct me_setter
			{
				template<class T,class U>
				static inline void set(T*object_ptr, U&& managed, typename std::enable_if_t<std::is_base_of_v<me<T>, T>, int>* = 0)noexcept {
					object_ptr->This = std::forward<U>(managed);
				}
				template<class T, class U>
				static inline void set(T*object_ptr, U&& managed, typename std::enable_if_t<!std::is_base_of_v<me<T>, T>, int>* = 0)noexcept {}
			};

			template<class T>
			class ptr
			{
				template<class U>friend class ptr;
				template<class U>friend class weakptr;

				ResourceActions* m_actions = nullptr;
				Resource* m_resource = nullptr;
				T* m_object_ptr = nullptr;
			public:
				ptr() {}
				ptr(nullptr_t) {}
				ptr(void*ResourcePtr, T*ObjectPtr, ResourceActions* Actions)
					:m_actions(Actions), m_object_ptr(ObjectPtr) {
					m_resource = new Resource();
					m_resource->ResourcePtr = ResourcePtr;
					me_setter::set(m_object_ptr, *this);
				}
				ptr(ptr const&rhs) {
					this->m_object_ptr = rhs.m_object_ptr;
					this->m_resource = rhs.m_resource;
					this->m_actions = rhs.m_actions;
					if (m_resource == nullptr)
						return;
					m_resource->ResourcePtr = this->m_actions->Update(m_resource->ResourcePtr);
				}
				template<class U>
				ptr(ptr<U> const&other) {
					this->m_object_ptr = static_cast<T*>(other.m_object_ptr);
					this->m_resource = other.m_resource;
					this->m_actions = other.m_actions;
					if (m_resource == nullptr)
						return;
					m_resource->ResourcePtr = this->m_actions->Update(m_resource->ResourcePtr);
				}
				ptr(ptr&&lhs)noexcept {
					this->m_object_ptr = lhs.m_object_ptr;
					this->m_resource = lhs.m_resource;
					this->m_actions = lhs.m_actions;
					lhs.m_object_ptr = nullptr;
					lhs.m_resource = nullptr;
				}
				ptr& operator=(ptr const&rhs) {
					SetToNull();
					this->m_object_ptr = rhs.m_object_ptr;
					this->m_resource = rhs.m_resource;
					this->m_actions = rhs.m_actions;
					if (m_resource == nullptr)
						return *this;
					m_resource->ResourcePtr = this->m_actions->Update(m_resource->ResourcePtr);
					return *this;
				}
				template<class U>
				ptr& operator=(ptr<U> const&other) {
					SetToNull();
					this->m_object_ptr = static_cast<T*>(other.m_object_ptr);
					this->m_resource = other.m_resource;
					this->m_actions = other.m_actions;
					if (m_resource == nullptr)
						return *this;
					m_resource->ResourcePtr = this->m_actions->Update(m_resource->ResourcePtr);
					return *this;
				}
				ptr& operator=(ptr&&lhs)noexcept {
					SetToNull();
					this->m_object_ptr = lhs.m_object_ptr;
					this->m_resource = lhs.m_resource;
					this->m_actions = lhs.m_actions;
					lhs.m_object_ptr = nullptr;
					lhs.m_resource = nullptr;
					return *this;
				}
				ptr& operator=(nullptr_t) {
					SetToNull();
					return *this;
				}
				~ptr() {
					SetToNull();
				}

				void SetToNull() {
					if (this->m_resource != nullptr)
					{
						this->m_resource->ResourcePtr = this->m_actions->Release(this->m_resource->ResourcePtr);
						if (this->m_resource->WeakReference == 0 && this->m_resource->ResourcePtr == nullptr)
							delete m_resource;
						m_resource = nullptr;
					}
					m_object_ptr = nullptr;
				}
				inline T* operator->()const noexcept {
					return this->m_object_ptr;
				}
				inline T* GetObjectPtr()noexcept {
					return this->m_object_ptr;
				}
				inline const T* GetObjectPtr()const noexcept {
					return this->m_object_ptr;
				}
				inline typename GetReference<Utility::IsSame<T, void>::Value, void_t, T>::Type GetObject()noexcept {
					return GetReference<Utility::IsSame<T, void>::Value, void_t, T>::Get(this->m_object_ptr);
				}
				inline typename GetReference<Utility::IsSame<T, void>::Value, void_t, const T>::Type GetObject()const noexcept {
					return GetReference<Utility::IsSame<T, void>::Value, void_t, const T>::Get(this->m_object_ptr);
				}
				inline bool IsNull()const noexcept {
					return this->m_resource == nullptr;
				}
			public:		
				template<class IndexT>
				inline constexpr auto operator[](IndexT&& index)noexcept ->decltype((*m_object_ptr)[std::forward<IndexT>(index)]) {
					return (*m_object_ptr)[std::forward<IndexT>(index)];
				}
				template<class IndexT>
				inline constexpr auto operator[](IndexT&& index)const noexcept ->decltype((*m_object_ptr)[std::forward<IndexT>(index)]) {
					return (*m_object_ptr)[std::forward<IndexT>(index)];
				}
				template<class...Args>
				inline constexpr auto operator()(Args&&...args)noexcept ->decltype((*m_object_ptr)(std::forward<Args>(args)...)) {
					return (*this->m_object_ptr)(std::forward<Args>(args)...);
				}
				template<class...Args>
				inline constexpr auto operator()(Args&&...args)const noexcept ->decltype((*m_object_ptr)(std::forward<Args>(args)...)) {
					return (*this->m_object_ptr)(std::forward<Args>(args)...);
				}

				inline ptr& operator++() {
					++(*this->m_object_ptr);
					return *this;
				}
				inline ptr& operator--() {
					--(*this->m_object_ptr);
					return *this;
				}
				inline ptr& operator--(int) {
					(*this->m_object_ptr)--;
					return *this;
				}
				inline ptr& operator++(int) {
					(*this->m_object_ptr)++;
					return *this;
				}

				typedef typename IteratorInterface<T>::IteratorT IteratorT;
				typedef typename IteratorInterface<T>::ConstIteratorT ConstIteratorT;
				inline IteratorT begin() {
					return this->m_object_ptr->begin();
				}
				inline IteratorT end() {
					return this->m_object_ptr->end();
				}
				inline ConstIteratorT begin()const {
					return this->m_object_ptr->begin();
				}
				inline ConstIteratorT end()const {
					return this->m_object_ptr->end();
				}

				inline bool operator==(ptr const&other)const noexcept {
					return *other.m_object_ptr == *this->m_object_ptr;
				}
				inline bool operator!=(ptr const&other)const noexcept {
					return *other.m_object_ptr != *this->m_object_ptr;
				}
				inline bool ReferenceEquals(ptr const&other)const noexcept {
					return this->m_resource == other.m_resource;
				}
			};

			typedef ptr<void> object;

			template<class T>
			class weakptr
			{
				ResourceActions* m_actions = nullptr;
				Resource* m_resource = nullptr;
				T* m_object_ptr = nullptr;
			public:
				weakptr() = default;
				weakptr(nullptr_t) {}
				weakptr(weakptr const&rhs) 
					:m_actions(rhs.m_actions), m_resource(rhs.m_resource), m_object_ptr(rhs.m_object_ptr) {
					if (m_resource != nullptr)
						this->m_resource->WeakReference++;
				}
				weakptr(weakptr&&) = default;
				weakptr(ptr<T> const&pointer)
					:m_actions(pointer.m_actions),m_resource(pointer.m_resource),m_object_ptr(pointer.m_object_ptr) {
					if (m_resource != nullptr)
						this->m_resource->WeakReference++;
				}
				weakptr& operator=(weakptr&&) = default;
				weakptr& operator=(weakptr const&rhs) {
					this->SetToNull();
					if (rhs.IsInvaild())
						return *this;
					this->m_actions = rhs.m_actions;
					this->m_resource = rhs.m_resource;
					this->m_object_ptr = rhs.m_object_ptr;
					this->m_resource->WeakReference++;
					return *this;
				}
				weakptr& operator=(ptr<T> const&pointer) {
					this->SetToNull();
					this->m_actions = pointer.m_actions;
					this->m_object_ptr = pointer.m_object_ptr;
					this->m_resource = pointer.m_resource;
					if (this->m_resource != nullptr)
						this->m_resource->WeakReference++;
					return *this;
				}
				~weakptr() {
					SetToNull();
				}
				bool IsInvaild()const {
					if (m_resource != nullptr)
						return m_resource->ResourcePtr == nullptr;
					return true;
				}
				void SetToNull() {
					if (!IsInvaild())
					{
						m_resource->WeakReference--;
						if (m_resource->WeakReference == 0)
							delete m_resource;
					}
					m_resource = nullptr;
					m_object_ptr = nullptr;
				}
				ptr<T> Lock() {
					if (IsInvaild())
						return nullptr;
					ptr<T> ret;
					ret.m_actions = m_actions;
					ret.m_object_ptr = m_object_ptr;
					ret.m_resource = m_resource;
					ret.m_resource->ResourcePtr = m_actions->Update(ret.m_resource->ResourcePtr);
					return ret;
				}
			};
		}
	}
}