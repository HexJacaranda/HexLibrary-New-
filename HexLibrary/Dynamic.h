#pragma once
namespace HL
{
	namespace System
	{
		namespace Script
		{
			enum class TokenType
			{
				//×ÖÃæÁ¿
				LiteralString,
				//±êÊ¶·û
				Identity,
				//×ó»¨À¨ºÅ
				LCurly,
				//ÓÒ»¨À¨ºÅ
				RCurly,
				//×óÐ¡À¨ºÅ
				LParen,
				//ÓÒÐ¡À¨ºÅ
				RParen,
				//×óÖÐÀ¨ºÅ
				LBracket,
				//ÓÒÖÐÀ¨ºÅ
				RBracket,
				//¼ÓºÅ
				Plus,
				//¼õºÅ
				Minus,
				//³ËºÅ
				Mul,
				//¶ººÅ
				Comma,
				//Ö¸Êý·û
				Pow,
				//ÎÊºÅ
				QMark,
				//Ã°ºÅ
				Colon,
				//ÊúÏß
				Or,
				//×Ö·û´®½áÎ²
				EOS
			};
			struct Token 
			{
				TokenType Type;
				wchar_t const* Begin;
				size_t Length;
			};
			class Tokenizer
			{
				wchar_t const* m_source = nullptr;
				size_t m_index = 0;
				size_t m_length = 0;
				Token* m_current = nullptr;
				TokenType LiteralString() {
					size_t begin = m_index;
					if (m_source[m_index++] != L'"')
						return TokenType::EOS;
					for (;;) {
						if (m_source[m_index] == L'"')
							break;
						if (m_source[m_index] == L'\\')
							m_index++;
						m_index++;
					}
					m_current->Begin = m_source + begin;
					m_current->Length = m_index - begin;
					m_current->Type = TokenType::LiteralString;
					return TokenType::LiteralString;
				}
				TokenType SetToken(TokenType Type) {
					PeekToken(Type);
					m_index++;
					return Type;
				}
				void PeekToken(TokenType Type) {
					m_current->Begin = m_source + m_index;
					m_current->Type = Type;
					m_current->Length = 1;
				}
			public:
				Tokenizer() {}
				Tokenizer(wchar_t const* Source, size_t Length) 
					:m_source(Source), m_length(Length) {}
				inline void SetSource(wchar_t const* Source, size_t Length) {
					m_source = Source;
					m_length = Length;
				}
				TokenType Consume(Token& out) {
					m_current = &out;
					switch (m_source[m_index])
					{
					case L'-':
						return SetToken(TokenType::Minus);
					case L'+':
						return SetToken(TokenType::Plus);
					case L'*':
						return SetToken(TokenType::Mul);
					case L',':
						return SetToken(TokenType::Comma);
					case L'^':
						return SetToken(TokenType::Pow);
					case L'?':
						return SetToken(TokenType::QMark);
					case L':':
						return SetToken(TokenType::Colon);
					case L'|':
						return SetToken(TokenType::Or);
					case L'{':
						return SetToken(TokenType::LCurly);
					case L'}':
						return SetToken(TokenType::RCurly);
					case L'(':
						return SetToken(TokenType::LParen);
					case L')':
						return SetToken(TokenType::RParen);
					case L'[':
						return SetToken(TokenType::LBracket);
					case L']':
						return SetToken(TokenType::RBracket);
					case L'\n':
						return SetToken(TokenType::EOS);
					case L'"':
						return LiteralString();
					default:
						break;
					}
				}
				bool Expect(Token& out, TokenType Type) {
					return Consume(out) == Type;
				}
			};

			enum class DescriptorType
			{
				Function,
				Variable,
				Expression,
				Class,
				Field,
				Property
			};
			enum class OperatorType
			{
				Add,
				AddAssign,
				Sub,
				SubAssign,
				Mul,
				MulAssign,
				Div,
				DivAssign,
				Top,
				TopAssign,
				LShift,
				LShiftAssign,
				RShift,
				RShiftAssign,
				Assign,
				BitOr,
				Or,
				BitAnd,
				And,
				Call,
				PrefixInc,
				SuffixInc,
				PrefixDeinc,
				SuffixDeinc
			};
			enum class Accessibility
			{
				Public,
				Internal,
				Private,
				Protected
			};
			class Descriptor 
			{

			};
			class TypeDescriptor :public Descriptor
			{
				ClassDescriptor* Target;
			};
			class VaribaleDescriptor :public Descriptor
			{
			public:
				TypeDescriptor* Type;
				Text::WString Name;
			};
			class ScopeDescriptor 
			{
				
				Collection::Generic::List<Descriptor*> Descriptors;
			};
			class FunctionDescriptor :public Descriptor
			{
			public:
				Accessibility Access;
				TypeDescriptor* ReturnType;
				Collection::Generic::List<ArgumentDescriptor*> Args;
				Collection::Generic::List<ScopeDescriptor*> Scopes;
				Text::WString Name;
			};
			class ExpressionDescriptor :public Descriptor
			{
			public:
				OperatorType OperatorType;
				Descriptor* First;
				Collection::Generic::List<Descriptor*> Others;
			};
			class FieldDescriptor :public Descriptor
			{
			public:
				Accessibility Access;
				TypeDescriptor* Type;
				Text::WString Name;
			};
			class PropertyDescriptor :public Descriptor
			{
			public:
				bool IsSimplified;
				TypeDescriptor* Type;
				FunctionDescriptor* Getter;
				FunctionDescriptor* Setter;
				Text::WString Name;
			};
			class ClassDescriptor :public Descriptor 
			{
			public:
				Collection::Generic::List<FieldDescriptor*> Fields;
				Collection::Generic::List<FunctionDescriptor*> Methods;
				Collection::Generic::List<ClassDescriptor*> Nested;
				Text::WString Name;
			};
			class ArgumentDescriptor :public Descriptor
			{
				Text::WString Name;
				TypeDescriptor* Type;
			};

			class Parser
			{
				Text::WString m_text;
				Tokenizer m_tokenizer;
			public:
				Parser(Text::WString const& Source) :m_text(Source) {
					m_tokenizer.SetSource(m_text.GetNativePtr(), m_text.Length());
				}
				ExpressionDescriptor* ParseExpression() {

				}
				FunctionDescriptor* ParseFunction() {

				}
				FieldDescriptor* ParseField() {

				}				
				FieldDescriptor* ParseProperty() {

				}
			};
		}
	}
}