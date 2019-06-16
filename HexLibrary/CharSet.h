#pragma once
namespace HL
{
	namespace System
	{
		namespace CharSet
		{
			template<class CharT>
			struct CharBase
			{
				static constexpr CharT UpperA = (CharT)'A';
				static constexpr CharT UpperZ = (CharT)'Z';
				static constexpr CharT LowerA = (CharT)'a';
				static constexpr CharT LowerZ = (CharT)'z';
				static constexpr CharT DigitZero = (CharT)'0';
				static constexpr CharT DigitNine = (CharT)'9';
				static constexpr CharT EndOfString = (CharT)'\0';
			};
#define _CHAR_BASE_SPECIALIZE(CHAR_T,TRIMER)\
			template<>\
			struct CharBase<CHAR_T>\
			{\
				static constexpr CHAR_T UpperA = TRIMER##'A';\
				static constexpr CHAR_T UpperZ = TRIMER##'Z';\
				static constexpr CHAR_T LowerA = TRIMER##'a';\
				static constexpr CHAR_T LowerZ = TRIMER##'z';\
				static constexpr CHAR_T DigitZero = TRIMER##'0';\
				static constexpr CHAR_T DigitNine = TRIMER##'9';\
				static constexpr CHAR_T EndOfString = TRIMER##'\0';\
			}

			_CHAR_BASE_SPECIALIZE(wchar_t, L);
			_CHAR_BASE_SPECIALIZE(char16_t, u);
			_CHAR_BASE_SPECIALIZE(char32_t, U);
		}
	}
}