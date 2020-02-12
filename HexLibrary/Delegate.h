#pragma once
namespace HL
{
	namespace System
	{
		namespace Delegation
		{
			template<class R,class...Args>
			class InvokerBase
			{
			public:
				virtual R Invoke(void*, Args&&...) = 0;
				virtual ~InvokerBase() = default;
			};

			template<class Literals,class RealType>
			class Invoker;

			template<class FunctorT,class R,class...Args>
			class Invoker<R(__stdcall*)(Args...), FunctorT> :public InvokerBase<R, Args...>
			{
				FunctorT m_method;
			public:
				template<class Fun>
				Invoker(Fun&&target) :m_method(std::forward<Fun>(target)) {}
				virtual R Invoke(void* Object, Args&& ...args) {
					return m_method(std::forward<Args>(args)...);
				}
				virtual ~Invoker() = default;
			};
			
			template<class FunctorT,class TT,class R,class...Args>
			class Invoker<R(__thiscall TT::*)(Args...), FunctorT> :public InvokerBase<R, Args...>
			{
				FunctorT m_method;
				TT* m_object_ptr;
			public:
				template<class Fun>
				Invoker(Fun&&target, TT* object_ptr):
					m_method(std::forward<Fun>(target)),
					m_object_ptr(object_ptr) {}
				virtual R Invoke(void*Object, Args&&...args) {
					if (Object == nullptr)
						return (m_object_ptr->*m_method)(std::forward<Args>(args)...);
					else
						return (static_cast<TT*>(Object)->*m_method)(std::forward<Args>(args)...);
				}
				virtual ~Invoker() = default;
			};

			template<class R,class...Args>
			class Delegate
			{
				template<class Ret, class...Rest>friend class MulticastDelegate;
				Pointer::ptr<InvokerBase<R, Args...>> m_method_ptr;
			public:
				template<class Fun>
				Delegate(Fun&&target) {
					m_method_ptr = Pointer::Reference::newptr<Invoker<R(__stdcall*)(Args...), Fun>>(std::forward<Fun>(target));
				}
				template<class TT, class Fun>
				Delegate(TT* object, Fun&&target) {
					m_method_ptr = Pointer::Reference::newptr<Invoker<R(__thiscall TT::*)(Args...), Fun>>(std::forward<Fun>(target), object);
				}
				Delegate(Delegate const&) = default;
				Delegate(Delegate&&) = default;
				Delegate& operator=(Delegate const&) = default;
				Delegate& operator=(Delegate&&) = default;
				~Delegate() {
					m_method_ptr.SetToNull();
				}
				inline R operator()(Args...args) {
					return m_method_ptr->Invoke(nullptr,std::forward<Args>(args)...);
				}
				inline R Invoke(void*object, Args...args) {
					return m_method_ptr->Invoke(object, std::forward<Args>(args)...);
				}
			};

			template<class R,class...Args>
			class MulticastDelegate
			{
				Collection::Generic::List<Pointer::ptr<InvokerBase<R, Args...>>> m_list;
			public:
				MulticastDelegate() {}
				MulticastDelegate(MulticastDelegate const&) = default;
				MulticastDelegate(MulticastDelegate&&) = default;
				MulticastDelegate&operator=(MulticastDelegate const&) = default;
				MulticastDelegate&operator=(MulticastDelegate&&) = default;
				template<class FunctorT>
				MulticastDelegate&operator+=(FunctorT&&functor) {
					m_list.Add(Pointer::Reference::newptr<Invoker<R(__stdcall*)(Args...), FunctorT>>(std::forward<FunctorT>(functor)));
					return *this;
				}
				MulticastDelegate&operator+=(Delegate<R, Args...> const&delegate) {
					m_list.Add(delegate.m_method_ptr);
					return *this;
				}
				void operator()(Args...args) {
					for (size_t i = 0; i < m_list.Count(); ++i)
						m_list[i]->Invoke(nullptr, std::forward<Args>(args)...);
				}
				~MulticastDelegate() = default;
			};
		}
	}
}