#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			namespace RegularExpression
			{
				class RegexParsingException :public Exception::IException
				{
				public:
					RegexParsingException(const wchar_t* Msg) :Exception::IException(Msg) {}
				};
				//��ǵ�����
				enum class TokenType
				{
					//�ַ�(������
					Char,
					//������
					LCurly,
					//�һ�����
					RCurly,
					//��С����
					LParen,
					//��С����
					RParen,
					//��������
					LBracket,
					//��������
					RBracket,
					//�Ӻ�
					Plus,
					//����
					Minus,
					//�˺�
					Mul,
					//����
					Comma,
					//ָ����
					Pow,
					//�ʺ�
					QMark,
					//ð��
					Colon,
					//����
					Or,
					//ת��
					Escape,
					//�ַ�����β
					EOS
				};
				//���
				class Token
				{
				public:
					wchar_t const* Begin;
					size_t Size;
					TokenType Type;
				};
				//�ʷ�������
				class Tokenizer
				{
					wchar_t const* m_source = nullptr;
					size_t m_index = 0;
					size_t m_size = 0;
					Token* m_current = nullptr;
				private:
					void SetToken(TokenType Type) {
						PeekToken(Type);
						m_index++;
					}
					void PeekToken(TokenType Type) {
						m_current->Begin = m_source + m_index;
						m_current->Type = Type;
						m_current->Size = 1;
					}
				public:
					Tokenizer(wchar_t const* Source, size_t Size) :m_source(Source), m_size(Size) {}
					Tokenizer() {}
					Tokenizer(Tokenizer&&) = default;
					Tokenizer(Tokenizer const&) = default;
					Tokenizer& operator=(Tokenizer&&) = default;
					Tokenizer& operator=(Tokenizer const&) = default;
					void Consume(Token &Out)
					{
						m_current = &Out;
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
						case L'\\':
							return SetToken(TokenType::Escape);
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
						default:
							return SetToken(TokenType::Char);
						}
					}
					void Abandon() {
						m_index++;
					}
					void Peek(Token &Out)
					{
						m_current = &Out;
						switch (m_source[m_index])
						{
						case L'-':
							return PeekToken(TokenType::Minus);
						case L'+':
							return PeekToken(TokenType::Plus);
						case L'*':
							return PeekToken(TokenType::Mul);
						case L',':
							return PeekToken(TokenType::Comma);
						case L'^':
							return PeekToken(TokenType::Pow);
						case L'?':
							return PeekToken(TokenType::QMark);
						case L':':
							return PeekToken(TokenType::Colon);
						case L'|':
							return PeekToken(TokenType::Or);
						case L'\\':
							return PeekToken(TokenType::Escape);
						case L'{':
							return PeekToken(TokenType::LCurly);
						case L'}':
							return PeekToken(TokenType::RCurly);
						case L'(':
							return PeekToken(TokenType::LParen);
						case L')':
							return PeekToken(TokenType::RParen);
						case L'[':
							return PeekToken(TokenType::LBracket);
						case L']':
							return PeekToken(TokenType::RBracket);
						case L'\n':
							return PeekToken(TokenType::EOS);
						default:
							return PeekToken(TokenType::Char);
						}
					}
					void Move(offset_t Offset) {
						m_index += Offset;
					}
					void SetSource(wchar_t const*Source, size_t Size) {
						m_source = Source;
						m_size = Size;
					}
					inline void Position(size_t Target) {
						m_index = Target;
					}
					inline size_t Position() {
						return m_index;
					}
					inline size_t Count() {
						return m_size;
					}
					inline bool Done() {
						return m_index >= m_size;
					}
				};
				//����
				class Content
				{
					wchar_t m_left;
					wchar_t m_right;
					bool m_any_match = false;
				protected:
					bool m_complementary = false;
				public:
					Content(Content const&) = default;
					Content(Content &&) = default;
					Content(wchar_t Single, bool Not = false)
						:m_left(Single), m_right(Single), m_complementary(Not) {}
					Content(wchar_t Left, wchar_t Right, bool Not = false)
						:m_left(Left), m_right(Right), m_complementary(Not) {}
					Content(bool)
						:m_any_match(true),m_left(0),m_right(0) {}
					virtual bool Accept(wchar_t Char)const {
						bool flag = Char >= m_left && Char <= m_right;
						if (m_any_match)
							flag = true;
						return m_complementary ? !flag : flag;
					}
					virtual ~Content() {}
				};
				//����
				class ContentSet :public Content
				{
					Collection::Generic::List<Content*> m_content;
				public:
					ContentSet(bool Not = false) :Content(0, Not) {}
					inline void Add(Content*Target) {
						m_content.Add(Target);
					}
					virtual bool Accept(wchar_t Char)const {
						bool flag = false;
						for (size_t i = 0; i < m_content.Count(); ++i)
							if (m_content[i]->Accept(Char))
							{
								flag = true;
								break;
							}
						return m_complementary ? !flag : flag;
					}
					virtual ~ContentSet() {
					}
				};
				//����������
				class InconsistentContent :public Content
				{
					Collection::Generic::Dictionary<wchar_t, wchar_t> m_set;
				public:
					InconsistentContent(bool Not = false) :Content(0, Not) {}
					inline void Add(wchar_t Char) {
						m_set.Add(Char, Char);
					}
					virtual bool Accept(wchar_t Char) {
						bool flag = m_set.Contains(Char);
						return m_complementary ? flag : !flag;
					}
					virtual ~InconsistentContent() {
					}
				};
				//����������
				enum class DescriptorType :short
				{
					Parallel,
					Joint,
					Repeat,
					Single,
					Set
				};
				//���򲶻���Ϣ
				struct RegexCapture
				{
					const wchar_t*CaptureName = nullptr;
					size_t CaptureArgument = std::numeric_limits<size_t>::max();
				};
				//������
				class Descriptor
				{
				public:
					Descriptor(DescriptorType type) :Type(type) {}
					RegexCapture CaptureInfo;
					DescriptorType Type;
					virtual ~Descriptor() {}
				};
				//����
				class DestriptorCollection :public Descriptor
				{
				public:
					DestriptorCollection(DescriptorType type) :Descriptor(type) {}
					Collection::Generic::List<Descriptor*> Descriptors;
					virtual ~DestriptorCollection() {}
				};
				//����
				class RepeatDescriptor : public Descriptor
				{
				public:
					RepeatDescriptor() :Descriptor(DescriptorType::Repeat) {}
					Descriptor* Target = nullptr;
					size_t Min = 0;
					size_t Max = 0;
					bool Greedy = true;
					virtual ~RepeatDescriptor() {}
				};
				//�ַ�����
				class ContentDescriptor :public Descriptor
				{
				public:
					ContentDescriptor() :Descriptor(DescriptorType::Set) {}
					Content* ContentPtr = nullptr;
					virtual ~ContentDescriptor() {}
				};
				
				//ת������
				enum class EscapeType
				{
					//�ڽ��ַ�����ת��
					BuiltInCharSet,
					//�ؼ���ת��
					KeyWord
				};

				class RegexParserResource
				{
					Collection::Generic::List<Descriptor*> Descriptors;
					Collection::Generic::List<Content*> Content;
					Collection::Generic::List<WString> NamedCapture;
				};

				//����������ʽ���ɳ�������
				class RegexParser
				{
					Collection::Generic::List<Descriptor*> m_objects;
					Collection::Generic::List<Content*> m_content;
					Collection::Generic::List<WString> m_named_capture;
					size_t m_anonymous_capture_index = 0;
					Tokenizer m_tokenizer;
					WString m_source;
				private:
					template<class U>
					inline U* AddContent(U* target) {
						m_content.Add(target);
						return target;
					}
					template<class U, class...Args>
					inline U* New(Args &&...args)
					{
						U* ret = new U(std::forward<Args>(args)...);
						m_objects.Add(ret);
						return ret;
					}
					//�հ��ַ����ϼ�:\\s
					inline Content* BlankSet(bool Complementary = false) {
						InconsistentContent* ret = AddContent(new InconsistentContent(Complementary));
						ret->Add(L'\f');
						ret->Add(L'\n');
						ret->Add(L'\r');
						ret->Add(L'\t');
						ret->Add(L'\v');
						return ret;
					}
					//�ǿհ��ַ����ϼ�:\\S
					inline Content* ComplementaryBlankSet(bool Complementary = false) {
						return BlankSet(!Complementary);
					}
					//�����ַ����ϼ�:\\d
					inline Content* DigitSet(bool Complementary = false) {
						return AddContent(new Content(L'0', L'9', Complementary));
					}
					//�������ַ����ϼ�:\\D
					inline Content* ComplementaryDigitSet(bool Complementary = false) {
						return AddContent(new Content(L'0', L'9', !Complementary));
					}
					inline Content* LowerAToZSet(bool Complementary = false) {
						return AddContent(new Content(L'a', L'z', Complementary));
					}
					inline Content* UpperAToZSet(bool Complementary = false) {
						return AddContent(new Content(L'A', L'Z', Complementary));
					}
					//�����ַ����ϼ�:\\w
					inline Content* WordSet(bool Complementary = false) {
						ContentSet *set = AddContent(new ContentSet(Complementary));
						set->Add(LowerAToZSet());
						set->Add(UpperAToZSet());
						set->Add(DigitSet());
						set->Add(AddContent(new Content(L'_')));
						return set;
					}
					//�ǵ����ַ����ϼ�:\\W
					inline Content* ComplementaryWordSet(bool Complementary = false) {
						return WordSet(!Complementary);
					}
				private:
					void Clear() {
						for (size_t i = 0; i < m_objects.Count(); ++i)
							delete m_objects[i];
						for (size_t i = 0; i < m_content.Count(); ++i)
							delete m_content[i];
						m_objects.Clear();
						m_content.Clear();
					}
				private:
					Descriptor* TryParseSuffix(Descriptor*Target) {
						size_t left = 0;
						size_t right = 0;
						Token token;
						m_tokenizer.Consume(token);
						switch (token.Type)
						{
						case TokenType::LCurly:
						{
							size_t left_count = 0;
							size_t right_count = 0;
							left_count = StringFunction::IntSniff(token.Begin + 1);//��̽��С����
							if (left_count > 0)
								left = StringFunction::StringToInt<size_t>(token.Begin + 1, left_count);
							else
								Exception::Throw<RegexParsingException>(L"Expected to be a min repeatition time");
							m_tokenizer.Move(left_count);//��������
							m_tokenizer.Consume(token);
							if (token.Type == TokenType::RCurly)//�����������,����
							{
								right = left;
								break;
							}
							else if (token.Type == TokenType::Comma)//���ż��
							{
								m_tokenizer.Consume(token);
								right_count = StringFunction::IntSniff(token.Begin);
								if (right_count > 0)
									right = StringFunction::StringToInt<size_t>(token.Begin, right_count);
								else//���û�����֣��������޲���
									right = std::numeric_limits<size_t>::max();
								m_tokenizer.Move(right_count - 1);
								m_tokenizer.Consume(token);
								if (token.Type != TokenType::RCurly)//�һ����ż��
									Exception::Throw<RegexParsingException>(L"Expected to be RCurly(})");
								break;
							}
						}
						case TokenType::Mul:
						{
							left = 0;
							right = std::numeric_limits<size_t>::max();
							break;
						}
						case TokenType::Plus:
						{
							left = 1;
							right = std::numeric_limits<size_t>::max();
							break;
						}
						case TokenType::QMark:
						{
							left = 0;
							right = 1;
							break;
						}
						default:
							m_tokenizer.Move(-1);
							return nullptr;
						}
						RepeatDescriptor* repeat = New<RepeatDescriptor>();
						repeat->Min = left;
						repeat->Max = right;
						repeat->Target = Target;
						repeat->Type = DescriptorType::Repeat;
						m_tokenizer.Peek(token);
						repeat->Greedy = true;
						if (token.Type == TokenType::QMark)//��̰��ƥ��
						{
							repeat->Greedy = false;
							m_tokenizer.Consume(token);
						}
						return repeat;
					}
					RegexCapture ParseCapture() {
						RegexCapture ret;
						Token token;
						m_tokenizer.Peek(token);
						if (token.Type == TokenType::QMark)
						{
							m_tokenizer.Consume(token);
							m_tokenizer.Peek(token);
							if (*token.Begin == L'<')
							{
								m_tokenizer.Abandon();
								wchar_t const* end = StringFunction::GoUntil(token.Begin, L'>');
								ret.CaptureName = token.Begin + 1;
								ret.CaptureArgument = end - token.Begin - 1;
								m_named_capture.Add(std::move(WString::From(ret.CaptureName, ret.CaptureArgument)));
								m_tokenizer.Move(ret.CaptureArgument + 1);
							}
							else if (*token.Begin == L':')
							{
								m_tokenizer.Abandon();
								return ret;
							}
							else
								Exception::Throw<RegexParsingException>(L"Invalid capture option");
						}
						else
							ret.CaptureArgument = m_anonymous_capture_index++;
						return ret;
					}
					Descriptor* ParseSet() {
						ContentSet* ret = AddContent(new ContentSet);
						Token token;
						m_tokenizer.Peek(token);//Peekһ��
						bool complementary = false;//ȡ��
						if (token.Type == TokenType::Pow)//ȡ��
						{
							complementary = true;
							m_tokenizer.Consume(token);//ǰ��
						}
						while (!m_tokenizer.Done())
						{
							m_tokenizer.Consume(token);
							if (token.Type == TokenType::RBracket)//�����˳�
								break;
							if (token.Type == TokenType::Escape)//��ת����Ļ�
							{
								switch (GetEscapeType())
								{
								case EscapeType::BuiltInCharSet:
									m_tokenizer.Consume(token);
									ret->Add(ParseBuiltInSet(*token.Begin, complementary));
									break;
								case EscapeType::KeyWord:
									m_tokenizer.Consume(token);
									ret->Add(AddContent(new Content(*token.Begin, complementary)));
									break;
								}
							}
							else
							{
								wchar_t left = *token.Begin;
								wchar_t right = L'\0';
								m_tokenizer.Peek(token);//�Ƿ�Ϊ��������
								if (token.Type == TokenType::Minus)
								{
									m_tokenizer.Move(1);//�ƶ���Range�Ҳ�߽�
									m_tokenizer.Consume(token);
									right = *token.Begin;
								}
								else
									right = left;
								Content* content = AddContent(new Content(left, right, complementary));
								ret->Add(content);
							}
						}
						Descriptor* set = FromContent(ret);
						Descriptor* suffix = TryParseSuffix(set);
						if (suffix != nullptr)
							return suffix;
						else
							return set;
					}
					Descriptor* ParseSingle(Token const&token) {
						Content* content = nullptr;
						if (*token.Begin == L'.')
							content = AddContent(new Content(true));
						else
							content = AddContent(new Content(*token.Begin));
						Descriptor* ret = FromContent(content);
						Descriptor* suffix = TryParseSuffix(ret);
						if (suffix != nullptr)
							return suffix;
						else
							return ret;
					}
					Descriptor* ParseSingle(wchar_t Char) {
						ContentDescriptor* single = New<ContentDescriptor>();
						single->ContentPtr = AddContent(new Content(Char));
						Descriptor* suffix = TryParseSuffix(single);
						if (suffix != nullptr)
							return suffix;
						else
							return single;
					}
					EscapeType GetEscapeType() {
						Token token;
						m_tokenizer.Peek(token);
						if (Algorithm::Any(*token.Begin, L'S', L's', L'd', L'D', L'W', L'w'))
							return EscapeType::BuiltInCharSet;
						else if (Algorithm::Any(*token.Begin,
							L'{', L'}', L'[', L']', L'(', L')', L'+', L'?', L'*', L'\\', L'-', L'.'))
							return EscapeType::KeyWord;
						else
							Exception::Throw<RegexParsingException>(L"Invalid escape character");
						return EscapeType::KeyWord;
					}
					Descriptor* FromContent(Content* Target) {
						ContentDescriptor* ret = New<ContentDescriptor>();
						ret->ContentPtr = Target;
						return ret;
					}
					Content* ParseBuiltInSet(wchar_t Char, bool Not = false) {
						switch (Char)
						{
						case L'S':
							return ComplementaryBlankSet(Not);
						case L's':
							return BlankSet(Not);
						case L'd':
							return DigitSet(Not);
						case L'D':
							return ComplementaryDigitSet(Not);
						case L'w':
							return WordSet(Not);
						case L'W':
							return ComplementaryWordSet(Not);
						default:
							break;
						}
						return nullptr;
					}
					Descriptor* ParseUnit(Token& Last) {
						DestriptorCollection * ret = New<DestriptorCollection>(DescriptorType::Joint);
						Token token;
						while (!m_tokenizer.Done())
						{
							m_tokenizer.Consume(token);
							switch (token.Type)
							{
							case TokenType::LBracket:
								ret->Descriptors.Add(ParseSet());
								break;
							case TokenType::Char:
								ret->Descriptors.Add(ParseSingle(token));
								break;
							case TokenType::Escape:
							{
								Descriptor*set = nullptr;
								switch (GetEscapeType())
								{
								case EscapeType::BuiltInCharSet:
									m_tokenizer.Consume(token);
									set = FromContent(ParseBuiltInSet(*token.Begin));
									break;
								case EscapeType::KeyWord:
									m_tokenizer.Consume(token);
									set = ParseSingle(*token.Begin);
									break;
								}
								Descriptor* suffix = TryParseSuffix(set);
								if (suffix != nullptr)
									ret->Descriptors.Add(suffix);
								else
									ret->Descriptors.Add(set);
								break;
							}
							default:
								goto Exit;
							}
						}
						token.Type = TokenType::EOS;
					Exit:
						Last = token;
						return Simplify(ret);
					}
					static Descriptor* Simplify(DestriptorCollection* Target) {
						if (Target->Descriptors.Count() == 0)
							return nullptr;
						else if (Target->Descriptors.Count() == 1 && Target->CaptureInfo.CaptureArgument != std::numeric_limits<size_t>::max())
							return Target->Descriptors[0];
						else
							return Target;
					}
					Descriptor* ParseParallel(Descriptor* Parsed) {
						DestriptorCollection* ret = New<DestriptorCollection>(DescriptorType::Parallel);
						ret->Descriptors.Add(Parsed);
						Token last_token;
						while (!m_tokenizer.Done())
						{
							size_t top = FindParallel(last_token);
							Descriptor* unit = Parse(top);
							switch (last_token.Type)
							{
							case TokenType::Or:
								ret->Descriptors.Add(unit); break;
							case TokenType::RParen:
							case TokenType::EOS:
								ret->Descriptors.Add(unit);
								return ret;
							}
						}
						return ret;
					}
					size_t FindParallel(Token&token)
					{
						size_t origin = m_tokenizer.Position();
						size_t cnt = 0;
						while (!m_tokenizer.Done())
						{
							m_tokenizer.Consume(token);
							if (token.Type == TokenType::LParen)
								cnt++;
							else if (token.Type == TokenType::RParen)
							{
								if (cnt == 0)
									goto Exit;
								cnt--;
							}
							else if (token.Type == TokenType::Or)
								if (cnt == 0)
									goto Exit;
						}
						token.Type = TokenType::EOS;
					Exit:
						size_t ret = token.Type == TokenType::EOS ? m_tokenizer.Position() : m_tokenizer.Position() - 1;
						m_tokenizer.Position(origin);
						return ret;
					}
					Descriptor* Parse(size_t Top)
					{
						DestriptorCollection* ret = New<DestriptorCollection>(DescriptorType::Joint);
						Token token;
						while (m_tokenizer.Position() < Top)
						{
							Descriptor* current = ParseUnit(token);
							if (m_tokenizer.Position() >= Top)
							{
								if (current != nullptr)
									ret->Descriptors.Add(current);
								goto Exit;
							}
							switch (token.Type)
							{
							case TokenType::LParen:
							{
								if (current != nullptr)
									ret->Descriptors.Add(current);
								RegexCapture capture = ParseCapture();
								Descriptor* descriptor = Parse(Top);
								descriptor->CaptureInfo = capture;
								ret->Descriptors.Add(descriptor);
								break;
							}
							case TokenType::Or:
								ret->Descriptors.Add(ParseParallel(current));
								goto Exit;
							case TokenType::RParen:
								if (current != nullptr)
									ret->Descriptors.Add(current);
								goto Exit;
							case TokenType::EOS:
								if (current != nullptr)
									ret->Descriptors.Add(current);
								goto Exit;
							}
						}
					Exit:
						return Simplify(ret);
					}
				public:
					inline Descriptor* Parse() {
						return Parse(this->m_tokenizer.Count());
					}
					RegexParser(WString const& Source) :m_source(Source) {
						m_tokenizer.SetSource(m_source.GetNativePtr(), m_source.Length());
					}
					RegexParser(RegexParser const&) = delete;
					RegexParser(RegexParser&&) = default;
					inline Collection::Generic::List<WString> const& GetNamedCaptureList()const noexcept{
						return this->m_named_capture;
					}
					inline size_t GetAnonymousCaptureCount()const noexcept {
						return this->m_anonymous_capture_index;
					}
					inline WString const& GetPattern()const noexcept {
						return this->m_source;
					}
					~RegexParser() {
						Clear();
					}
				};
			}
		}
	}
}