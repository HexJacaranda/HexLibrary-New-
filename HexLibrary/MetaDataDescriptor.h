#pragma once
namespace HL::System::Runtime::Meta
{
	class MethodMetaData;
	class ClassMetaData;

	class MethodMetaData
	{
		ClassMetaData* m_closure;
		Text::WString m_name;
		Collection::Generic::Array<int> m_args_type;
		int m_ret_type;
		int m_token;
	public:
		Text::WString const& GetMethodName() {
			return m_name;
		}
	};

	class ClassMetaData
	{
		ClassMetaData* m_closure;
		Text::WString m_name;
		int m_token;
	public:
		Text::WString const& GetClassName() {

		}
	};
}