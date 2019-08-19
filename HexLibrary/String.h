#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			namespace StringFunction
			{
				template<class CharT>
				static size_t StringLength(const CharT *string)noexcept {
					size_t cnt = 0;
					if (string == nullptr)
						return cnt;
					for (; *string != CharSet::CharBase<CharT>::EndOfString; string++, cnt++);
					return cnt;
				}
				template<class CharT>
				bool StringEquals(const CharT*target_string, const CharT* source, size_t target_length, size_t source_length)noexcept {
					if (target_length != source_length)
						return false;
					//ิคละ
					if (target_string[0] == source[0] && target_string[target_length - 1] == source[target_length - 1]) {
						for (size_t i = 1; i < target_length - 1; ++i) {
							if (target_string[i] != source[i])
								return false;
						}
						return true;
					}
					return false;
				}
				template<class CharT>
				const CharT* IndexOf(const CharT* target, const CharT* substring)
				{
					const CharT* cp = target;
					const CharT* left, *right;
					if (!*substring)
						return (target);
					while (*cp)
					{
						left = cp;
						right = right;
						while (*left && *right && !(*left - *right))
							left++, left++;
						if (!*right)
							return (cp);
						cp++;
					}
					return nullptr;
				}

				template<class CharT>
				static inline bool IsLower(CharT c)noexcept {
					return c >= CharSet::CharBase<CharT>::LowerA&&c <= CharSet::CharBase<CharT>::LowerZ;
				}
				template<class CharT>
				static inline bool IsUpper(CharT c)noexcept {
					return c >= CharSet::CharBase<CharT>::UpperA&&c <= CharSet::CharBase<CharT>::UpperZ;
				}
				template<class CharT>
				static inline CharT UpperDelta()noexcept {
					return CharSet::CharBase<CharT>::UpperA - CharSet::CharBase<CharT>::LowerA;
				}
				template<class CharT>
				static inline bool IsAlpha(CharT c)noexcept {
					return IsLower(c) || IsUpper(c);
				}
				template<class CharT>
				static inline bool IsDigit(CharT c)noexcept {
					return (c >= CharSet::CharBase<CharT>::DigitZero&&c <= CharSet::CharBase<CharT>::DigitNine);
				}
				template<class CharT>
				static inline bool IsWord(CharT c)noexcept {
					return IsAlpha(c) || IsDigit(c);
				}
				template<class CharT>
				static inline short IsHex(CharT c)noexcept {
					if (c <= (CharT)'f'&&c >= (CharT)'a')
						return 1;
					else if (c <= (CharT)'F'&&c >= (CharT)'A')
						return 2;
					else
						return 0;
				}
				template<class CharT>
				static inline bool IsWhiteSpace(CharT c)noexcept {
					return c == (CharT)' ' || c == (CharT)'\n' || c == (CharT)'\t' || c == (CharT)'\f' || c == '\r';
				}
				template<class CharT>
				static CharT ToUpper(CharT c)noexcept {
					if (IsLower(c))
						c += UpperDelta<CharT>();
					return c;
				}
				template<class CharT>
				static CharT ToLower(CharT c)noexcept {
					if (IsUpper(c))
						c -= UpperDelta<CharT>();
					return c;
				}
				template<class CharT>
				void StringToUpper(CharT* target_string)noexcept {
					while (*target_string != CharSet::CharBase<CharT>::EndOfString)
						*target_string++ = ToUpper(*target_string);
				}
				template<class CharT>
				void StringToLower(CharT* target_string)noexcept {
					while (*target_string != CharSet::CharBase<CharT>::EndOfString)
						*target_string++ = ToLower(*target_string);
				}
				template<class CharT>
				static size_t UIntSniff(CharT const*string) {
					size_t out = 0;
					while (IsDigit(*string))
					{
						++string;
						++out;
					}
					return out;
				}
				template<class CharT>
				static size_t IntSniff(CharT const*string) {
					size_t out = 0;
					if (*string == (CharT)'-' || IsDigit(*string))
					{
						string++;
						out++;
						while (IsDigit(*string))
						{
							++string;
							++out;
						}
					}
					return out;
				}
				template<class CharT>
				static size_t FloatSniff(CharT const*string) {
					size_t out = 0;
					short dot_times = 0;
					if (*string == (CharT)'-' || IsDigit(*string))
					{
						string++;
						out++;
						for (;;)
						{
							if (*string == (CharT)'.')
							{
								++dot_times;
								if (dot_times > 1)
									return 0;
							}
							else

							{
								if (!IsDigit(*string))
									break;
							}
							++string;
							++out;
						}
					}
					return out;
				}
				template<class FloatT, class CharT>
				static FloatT StringToFloat(CharT const*string, size_t count) {
					int sign = 1;
					if (string[0] == (CharT)'-')
						sign = -1;
					FloatT int_part = 0;
					FloatT float_part = 0;
					FloatT pow_iter = 10;
					bool dot_sign = false;
					size_t iter = 0;
					if (sign < 0)
						iter = 1;
					for (size_t i = iter; i < count; ++i)
					{
						if (string[i] == (CharT)'.')
						{
							dot_sign = true;
							continue;
						}
						if (IsDigit(string[i]))
						{
							if (!dot_sign)
								int_part = int_part * 10 + (FloatT)(string[i] - CharSet::CharBase<CharT>::DigitZero);
							else
							{
								float_part += (FloatT)(string[i] - CharSet::CharBase<CharT>::DigitZero) / pow_iter;
								pow_iter *= 10;
							}
						}
						else
							return (int_part + float_part)*sign;
					}
					return (int_part + float_part)*sign;
				}
				template<class CharT>
				static size_t HexToInt(CharT ch) {
					short type = IsHex(ch);
					if (type == 1)
						return ch - CharSet::CharBase<CharT>::LowerA + 10;
					else if (type == 2)
						return ch - CharSet::CharBase<CharT>::UpperA + 10;
					else
						return ch - CharSet::CharBase<CharT>::DigitZero;
				}
				template<class IntT, class CharT>
				static IntT StringToInt(CharT const*string, size_t count) {
					IntT ret_value = 0;
					int sign = 1;
					if (string[0] == (CharT)'-')
						sign = -1;
					size_t iter = 0;
					if (sign < 0)
						iter = 1;
					for (size_t i = iter; i < count; i++)
					{
						if (IsDigit(string[i]))
							ret_value = ret_value * 10 + (IntT)(string[i] - (CharT)'0');
						else
							return ret_value * sign;
					}
					return ret_value * sign;
				}
				template<class CharT>
				static CharT const* GoUntil(CharT const*string, CharT endchar)
				{
					for (; *string != endchar; string++) {}
					return string;
				}
			}
			template<class U>
			class BasicString :public Hash::IHash
			{
				Collection::LinearMemoryManager<U> m_string;
				inline void SetIncFactor() {
					m_string.IncFactor(1);
				}
			public:
				BasicString() {
					SetIncFactor();
				}
				BasicString(const U* string) {
					SetIncFactor();
					if (string == nullptr)
						return;
					size_t length = StringFunction::StringLength(string);
					if (length == 0)
						return;
					m_string.Append(string, length + 1);
				}
				BasicString(Collection::LinearMemoryManager<U>&&sequence)
					:m_string(std::move(sequence)) {
					if (m_string.UsedCount() > 0)
						if (m_string.At(m_string.UsedCount() - 1) != (U)0)
							m_string.Append((U)0);
					SetIncFactor();
					m_string.Shrink();
				}
				BasicString(Collection::LinearMemoryManager<U> const&sequence)
					:m_string(sequence) {
					if (m_string.UsedCount() > 0)
						if (m_string.At(m_string.UsedCount() - 1) != (U)0)
							m_string.Append((U)0);
					SetIncFactor();
					m_string.Shrink();
				}
				BasicString(BasicString const&) = default;
				BasicString(BasicString &&)noexcept = default;
				BasicString& operator=(BasicString const&) = default;
				BasicString& operator=(BasicString &&) = default;
				BasicString& operator=(const U* string) {
					Clear();
					if (string == nullptr)
						return *this;
					size_t length = StringFunction::StringLength(string);
					this->Append(string, length + 1);
					return *this;
				}
				BasicString& operator=(Collection::LinearMemoryManager<U>&&sequence){
					m_string = std::move(sequence);
					if (m_string.UsedCount() == 0)
						return *this;
					if (m_string.At(m_string.UsedCount() - 1) != (U)0)
						m_string.Append((U)0);
					SetIncFactor();
					m_string.Shrink();
					return *this;
				}
				BasicString& operator=(Collection::LinearMemoryManager<U> const&sequence){
					m_string = sequence;
					if (m_string.UsedCount() == 0)
						return *this;
					if (m_string.At(m_string.UsedCount() - 1) != (U)0)
						m_string.Append((U)0);
					SetIncFactor();
					m_string.Shrink();
					return *this;
				}
				void Append(const U*string) {
					if (string == nullptr)
						return;
					size_t length = StringFunction::StringLength(string);
					if (length == 0)
						return;
					m_string.UsedCount(Length());
					m_string.Append(string, length + 1);
				}
				void Append(const U*string, size_t count) {
					if (string == nullptr || count == 0)
						return;
					m_string.UsedCount(Length());
					m_string.Append(string, count + 1);
					m_string.At(Length()) = (U)0;
				}
				void Append(BasicString const&string) {
					if (string.Length() == 0)
						return;
					m_string.UsedCount(Length());
					m_string.Append(string.m_string.GetDataPtr(), string.m_string.UsedCount());
				}
				BasicString&operator+=(BasicString const&string) {
					Append(string);
					return *this;
				}
				BasicString&operator+=(const U * string) {
					Append(string);
					return *this;
				}
				friend BasicString operator+(BasicString const&lstring, BasicString const&rstring) {
					BasicString result = lstring;
					result.Append(rstring);
					return result;
				}
				friend BasicString operator+(BasicString const&lstring, const U* rstring) {
					BasicString result = lstring;
					result.Append(rstring);
					return result;
				}
				friend BasicString operator+(const U* lstring, BasicString const&rstring) {
					BasicString result = lstring;
					result.Append(rstring);
					return result;
				}
				friend bool operator==(BasicString const&lstring, BasicString const&rstring)noexcept
				{
					return StringFunction::StringEquals(lstring.GetNativePtr(), rstring.GetNativePtr(), lstring.Length(), rstring.Length());
				}
				friend bool operator==(BasicString const&lstring, const U* rstring)noexcept
				{
					return StringFunction::StringEquals(lstring.GetNativePtr(), rstring, lstring.Length(), StringFunction::StringLength(rstring));
				}
				friend bool operator==(const U* lstring, BasicString const& rstring)noexcept
				{
					return StringFunction::StringEquals(lstring, rstring.Length(), StringFunction::StringLength(lstring), rstring.Length());
				}
				inline size_t IndexOf(wchar_t const*target) {
					const U * result = StringFunction::IndexOf(GetNativePtr(), target);
					return result == nullptr ? Nop : result - GetNativePtr();
				}
				inline size_t IndexOf(BasicString const& string) {
					const U * result = StringFunction::IndexOf(GetNativePtr(), string.GetNativePtr());
					return result == nullptr ? Nop : result - GetNativePtr();
				}
				inline void Clear() {
					this->m_string.Empty();
				}
				inline size_t Length()const {
					return m_string.UsedCount() > 1 ? m_string.UsedCount() - 1 : 0;
				}
				inline const U* GetNativePtr()const {
					return this->m_string.GetDataPtr();
				}
				inline void ToUpper() {
					StringFunction::StringToUpper(this->m_string.GetDataPtr());
				}
				inline void ToLower() {
					StringFunction::StringToLower(this->m_string.GetDataPtr());
				}
				inline U const& operator[](size_t Index)const {
					return this->m_string.At(Index);
				}
				inline U & operator[](size_t Index) {
					return this->m_string.At(Index);
				}
				template<class StreamT>
				friend StreamT& operator<<(StreamT& Stream, BasicString const&string) {
					if (string.Length() > 0)
						Stream << string.m_string.GetDataPtr();
					return Stream;
				}
				virtual size_t GetHashCode()const {
					return Hash::GetSequenceHash((void*)this->m_string.GetDataPtr(), this->Length() * sizeof(U));
				}
				~BasicString() {
				}
				static BasicString<U> From(const U* string, size_t length) {
					BasicString<U> ret;
					ret.Append(string, length);
					return ret;
				}
				static BasicString<U> Format() {

				}
				static constexpr size_t Nop = std::numeric_limits<size_t>::max();
			};

			typedef BasicString<char16_t> U16String;
			typedef BasicString<char32_t> U32String;
			typedef BasicString<wchar_t> WString;
			typedef BasicString<char> String;
		}
	}
}