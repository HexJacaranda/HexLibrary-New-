#pragma once
namespace HL
{
	namespace System
	{
		namespace Exception
		{
			class IException
			{
				wchar_t const* ExceptionMessage = nullptr;
			public:
				IException()throw() {}
				IException(wchar_t const*msg)throw()
				{
					ExceptionMessage = msg;
				}
				virtual wchar_t const* Message()throw() {
					return ExceptionMessage;
				}
				virtual ~IException()throw() {

				}
			};

			class InvalidArgumentException:public IException
			{
			public:
				InvalidArgumentException() :IException(L"Argument is invalid") {}
			};

			template<class ExceptionT>
			class ExceptionPtr
			{
				template<class O>friend class ExceptionPtr;
				System::Pointer::ptr<ExceptionT> m_ptr;
			public:
				template<class E,class...Args>
				ExceptionPtr(const E*ptr, Args&&...args)throw() {
					m_ptr = System::Pointer::Reference::newptr<E>(std::forward<Args>(args)...);
				}
				ExceptionPtr(ExceptionPtr const&) = default;
				ExceptionPtr(ExceptionPtr&&) = default;
				ExceptionPtr& operator=(ExceptionPtr const&) = default;
				ExceptionPtr& operator=(ExceptionPtr&&) = default;
				~ExceptionPtr() = default;
				template<class Other>
				ExceptionPtr(ExceptionPtr<Other> const&other) {
					m_ptr = other.m_ptr;
					return *this;
				}
				inline ExceptionT* operator->()noexcept {
					return m_ptr.GetObjectPtr();
				}
				inline ExceptionT const* operator->()const noexcept {
					return m_ptr.GetObjectPtr();
				}
			};

			typedef ExceptionPtr<IException> ExceptionHandle;

			template<class E,class...Args>
			static void constexpr inline Throw(Args&&...args)
			{
				throw E(std::forward<Args>(args)...);
			}
		}
	}
}