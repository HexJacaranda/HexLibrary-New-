#pragma once
namespace HL
{
	namespace System
	{
		namespace Json
		{
			class JsonParsingException :public Exception::IException
			{
			public:
				JsonParsingException(const wchar_t* Msg) :Exception::IException(Msg) {}
			};

			enum class JsonValueType
			{
				String,
				Boolean,
				Digit,
				Array,
				Object,
				Null
			};
			class IJsonValue
			{
				JsonValueType m_value_type;
			public:
				IJsonValue(JsonValueType type) :m_value_type(type) {}
				inline JsonValueType GetType()const {
					return m_value_type;
				}
				virtual ~IJsonValue() = default;
			};
			class JsonValue :public IJsonValue
			{
				Text::WString m_string_value;
				double m_digit_value = 0;
			public:
				JsonValue(JsonValueType type) :IJsonValue(type) {}
				JsonValue(Text::WString const&text) :IJsonValue(JsonValueType::String), m_string_value(text) {}
				JsonValue(Text::WString &&text) :IJsonValue(JsonValueType::String), m_string_value(std::move(text)) {}
				JsonValue(double value) :IJsonValue(JsonValueType::Digit), m_digit_value(value) {}
				JsonValue(bool value) :IJsonValue(JsonValueType::Boolean), m_digit_value(value) {}
				JsonValue(nullptr_t) :IJsonValue(JsonValueType::Null) {}
				JsonValue(JsonValue const&) = default;
				JsonValue(JsonValue&&) = default;
				JsonValue&operator=(JsonValue const&) = default;
				JsonValue&operator=(JsonValue&&) = default;
				virtual ~JsonValue() = default;
				inline Text::WString const& AsString()const noexcept {
					return m_string_value;
				}
				inline double AsFloat()const noexcept {
					return m_digit_value;
				}
				inline bool AsBoolean()const noexcept {
					return m_digit_value != 0;
				}
				inline nullptr_t AsNull()const noexcept {
					return nullptr;
				}
			};
			class JsonArray :public IJsonValue
			{
			public:
				typedef Collection::Generic::List<Pointer::ptr<IJsonValue>> ListT;
			private:
				ListT m_list;
			public:
				JsonArray() :IJsonValue(JsonValueType::Array) {}
				JsonArray(JsonArray const&) = default;
				JsonArray(JsonArray &&) = default;
				JsonArray(ListT const&list) :IJsonValue(JsonValueType::Array), m_list(list) {}
				JsonArray(ListT &&list) :IJsonValue(JsonValueType::Array), m_list(std::move(list)) {}
				JsonArray& operator=(JsonArray const&) = default;
				JsonArray& operator=(JsonArray &&) = default;
				virtual ~JsonArray() = default;
				inline Pointer::ptr<IJsonValue> & operator[](size_t index) {
					return m_list[index];
				}
				inline Pointer::ptr<IJsonValue> const& operator[](size_t index)const {
					return m_list[index];
				}
				template<class U>
				inline Pointer::ptr<U> As(size_t index)const {
					return m_list[index];
				}
			};
			class JsonObject :public IJsonValue
			{
			public:
				typedef Collection::Generic::Dictionary<Text::WString, Pointer::ptr<IJsonValue>> TableType;
			private:
				TableType m_table;
			public:
				JsonObject() :IJsonValue(JsonValueType::Object) {}
				JsonObject(TableType const&table) :IJsonValue(JsonValueType::Object), m_table(table) {}
				JsonObject(TableType&&table) :IJsonValue(JsonValueType::Object), m_table(std::move(table)) {}
				JsonObject(JsonObject const&) = default;
				JsonObject(JsonObject &&) = default;
				JsonObject&operator=(JsonObject const&) = default;
				JsonObject&operator=(JsonObject &&) = default;
				virtual ~JsonObject() = default;
				inline Pointer::ptr<IJsonValue> & operator[](Text::WString const&Key) {
					return m_table[Key];
				}
				inline Pointer::ptr<IJsonValue> & operator[](size_t Hash) {
					return m_table.ByHash(Hash);
				}
				inline Pointer::ptr<IJsonValue> const& operator[](Text::WString const&Key)const {
					return m_table[Key];
				}
				inline Pointer::ptr<IJsonValue> const& operator[](size_t Hash)const {
					return m_table.ByHash(Hash);
				}
				template<class U>
				inline Pointer::ptr<U> As(Text::WString const&Key)const {
					return m_table[Key];
				}
			};

			enum class JsonTokenType
			{
				String,
				Digit,
				Null,
				Boolean,
				LBracket,
				RBracket,
				LCurly,
				RCurly,
				Comma,
				Colon
			};

			struct JsonToken
			{
				double Value = 0;
				Text::WString Content;
				JsonTokenType Type = JsonTokenType::Null;
			};

			class JsonTokenizer
			{
				wchar_t const* m_source = nullptr;
				size_t m_index = 0;
				size_t m_cnt = 0;
				inline void SetSingleToken(JsonToken& token,JsonTokenType type)noexcept {
					token.Type = type;
					token.Value = m_source[m_index++];
				}
				static bool IsEscapeChar(wchar_t wc, wchar_t&corresponding) {
					if (wc == L'n')
						corresponding = L'\n';
					else if (wc == L'b')
						corresponding = L'\b';
					else if (wc == L'r')
						corresponding = L'\r';
					else if (wc == L't')
						corresponding = L'\t';
					else if (wc == L'f')
						corresponding = L'\f';
					else if (wc == L'"')
						corresponding = L'"';
					else if (wc == L'\\')
						corresponding = L'\\';
					else if (wc == L'/')
						corresponding = L'/';
					else if (wc == L'u')
						corresponding = L'u';
					else
						return false;
					return true;
				}
				void ReadString(JsonToken & token) {
					Collection::LinearMemoryManager<wchar_t> string(16);
					token.Type = JsonTokenType::String;
					m_index++;
					for (;;) {
						if (m_source[m_index] == L'\\')//转义
						{
							m_index++;
							if (m_source[m_index] == L'u')//Unicode转义
							{
								m_index++;
								wchar_t unicode = 0;
								for (int i = 3; i >= 0; --i)
									unicode |= Text::StringFunction::HexToInt(m_source[m_index++]) << i * 4;
								string.Append(unicode);
							}
							else
							{
								wchar_t escape;
								if (!IsEscapeChar(m_source[m_index], escape))
									Exception::Throw<JsonParsingException>(L"Invalid escape character");
								string.Append(escape);
								m_index++;
							}
						}
						else if (m_source[m_index] == L'"')
						{
							token.Content = std::move(string);
							m_index++;
							return;
						}
						else
							string.Append(m_source[m_index++]);
					}
				}
				void ReadDigit(JsonToken & token) {
					size_t count = Text::StringFunction::FloatSniff(m_source + m_index);
					if (count == 0)
						Exception::Throw<JsonParsingException>(L"Nought-length number is not allowed");
					double first_part = Text::StringFunction::StringToFloat<double>(m_source + m_index, count);
					m_index += count;
					if (m_source[m_index] == L'E' || m_source[m_index] == L'e')
					{
						m_index++;
						size_t sec_count = Text::StringFunction::FloatSniff(m_source + m_index);
						if (sec_count == 0)
							Exception::Throw<JsonParsingException>(L"Nought-length exponent is not allowed");
						else
						{
							double second_part = Text::StringFunction::StringToFloat<double>(m_source + m_index, sec_count);
							m_index += sec_count;
							token.Value = pow(first_part, second_part);
						}
					}
					else
						token.Value = first_part;
					token.Type = JsonTokenType::Digit;
				}
				void ReadNull(JsonToken & token)noexcept
				{
					token.Type = JsonTokenType::Null;
					if(!Text::StringFunction::StringEquals(L"null", m_source + m_index, 4, 4))
						Exception::Throw<JsonParsingException>(L"Invalid key word - null");
					m_index += 4;
				}
				void ReadTrue(JsonToken & token)noexcept
				{
					token.Type = JsonTokenType::Boolean;
					token.Value = 1;
					if(!Text::StringFunction::StringEquals(L"true", m_source + m_index, 4, 4))
						Exception::Throw<JsonParsingException>(L"Invalid boolean value");
					m_index += 4;
				}
				void ReadFalse(JsonToken & token)noexcept
				{
					token.Type = JsonTokenType::Boolean;
					token.Value = 0;
					if (!Text::StringFunction::StringEquals(L"false", m_source + m_index, 5, 5))
						Exception::Throw<JsonParsingException>(L"Invalid boolean value");
					m_index += 5;
				}
			public:
				JsonTokenizer() = default;
				JsonTokenizer(JsonTokenizer const&) = default;
				JsonTokenizer(JsonTokenizer&&) = default;
				JsonTokenizer(wchar_t const*Source, size_t Cnt) :m_source(Source), m_cnt(Cnt) {}
				JsonTokenizer& operator=(JsonTokenizer const&) = default;
				JsonTokenizer& operator=(JsonTokenizer&&) = default;
				~JsonTokenizer() = default;
				void SetSource(wchar_t const* Source, size_t Cnt) {
					m_source = Source;
					m_cnt = Cnt;
					m_index = 0;
				}
				void Consume(JsonToken&token) {
					while (Text::StringFunction::IsWhiteSpace(m_source[m_index]))m_index++;
					switch (m_source[m_index])
					{
					case L'{':
						SetSingleToken(token, JsonTokenType::LCurly); break;
					case L'}':
						SetSingleToken(token, JsonTokenType::RCurly); break;
					case L'[':
						SetSingleToken(token, JsonTokenType::LBracket); break;
					case L']':
						SetSingleToken(token, JsonTokenType::RBracket); break;
					case L',':
						SetSingleToken(token, JsonTokenType::Comma); break;
					case L':':
						SetSingleToken(token, JsonTokenType::Colon); break;
					case L'"':
						ReadString(token); break;
					case L'-':
						ReadDigit(token); break;
					case L'n':
						ReadNull(token); break;
					case L't':
						ReadTrue(token); break;
					case L'f':
						ReadFalse(token); break;
					default:
						if (Text::StringFunction::IsDigit(m_source[m_index]))
							ReadDigit(token);
						break;
					}
				}
				inline bool Done()const noexcept {
					return m_index >= m_cnt;
				}
				void Repeek(size_t cnt) {
					m_index -= cnt;
				}
			};

			class JsonParser
			{
				JsonTokenizer m_tokenizer;
				Text::WString m_source;
			public:
				JsonParser(Text::WString const&Source) :m_source(Source) {
					m_tokenizer.SetSource(m_source.GetNativePtr(), m_source.Length());
				}
				JsonParser(Text::WString&& Source) :m_source(std::move(Source)) {
					m_tokenizer.SetSource(m_source.GetNativePtr(), m_source.Length());
				}
				Pointer::ptr<IJsonValue> ParseValue() {
					JsonToken token;
					if (m_tokenizer.Done())
						return nullptr;
					m_tokenizer.Consume(token);
					switch (token.Type)
					{
					case JsonTokenType::LCurly:
						m_tokenizer.Repeek(1);
						return ParseObject();
					case JsonTokenType::LBracket:
						m_tokenizer.Repeek(1);
						return ParseArray();
					case JsonTokenType::String:
						return Pointer::Reference::newptr<JsonValue>(std::move(token.Content));
					case JsonTokenType::Digit:
						return Pointer::Reference::newptr<JsonValue>(token.Value);
					case JsonTokenType::Boolean:
						return Pointer::Reference::newptr<JsonValue>((bool)token.Value);
					case JsonTokenType::Null:
						return Pointer::Reference::newptr<JsonValue>(nullptr);
					}
					return nullptr;
				}
				Pointer::ptr<JsonObject> ParseObject() {
					Collection::Generic::Dictionary<Text::WString, Pointer::ptr<IJsonValue>> table;
					JsonToken token;
					m_tokenizer.Consume(token);
					if (token.Type != JsonTokenType::LCurly)
						Exception::Throw<JsonParsingException>(L"Expected to be LCurly({)");
					while (!m_tokenizer.Done())
					{
						m_tokenizer.Consume(token);
						if (token.Type != JsonTokenType::String)
						{
							if (token.Type == JsonTokenType::RCurly)
								break;
							Exception::Throw<JsonParsingException>(L"Expected to be String");
						}
						Text::WString key = std::move(token.Content);
						m_tokenizer.Consume(token);
						if (token.Type != JsonTokenType::Colon)
							Exception::Throw<JsonParsingException>(L"Expected to be Colon(:)");
						Pointer::ptr<IJsonValue> value = std::move(ParseValue());
						table.Add({ std::move(key),std::move(value) });
						m_tokenizer.Consume(token);
						if (token.Type == JsonTokenType::RCurly)
							break;
						if (token.Type != JsonTokenType::Comma)
							Exception::Throw<JsonParsingException>(L"Expected to be Comma(,)");
					}
					return Pointer::Reference::newptr<JsonObject>(std::move(table));
				}
				Pointer::ptr<JsonArray> ParseArray() {
					Collection::Generic::List<Pointer::ptr<IJsonValue>> list;
					JsonToken token;
					m_tokenizer.Consume(token);
					if (token.Type != JsonTokenType::LBracket)
						Exception::Throw<JsonParsingException>(L"Expected to be LBracket([)");
					while (!m_tokenizer.Done())
					{
						Pointer::ptr<IJsonValue> value = std::move(ParseValue());
						if (value.IsNull())
							break;
						list.Add(std::move(value));
						m_tokenizer.Consume(token);
						if (token.Type == JsonTokenType::RBracket)
							break;
						if (token.Type != JsonTokenType::Comma)
							Exception::Throw<JsonParsingException>(L"Expected to be Comma(,)");
					}
					return Pointer::Reference::newptr<JsonArray>(std::move(list));
				}
			};
		}
	}
}