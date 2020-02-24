#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			template<class U>
			class TextWriter
			{
				U* m_iterator = nullptr;
				U* m_buffer = nullptr;
			public:
				TextWriter(U*buffer)noexcept
					:m_buffer(buffer), m_iterator(buffer) {}
				TextWriter(TextWriter&&) = default;
				TextWriter(TextWriter const&) = delete;
				TextWriter& operator=(TextWriter&&) = default;
				TextWriter& operator=(TextWriter const&) = delete;
				~TextWriter() = default;
				void Write(const U* string) {
					if (string == nullptr)
						return;
					while (*string != CharacterSet::CharBase<U>::EndOfString)
						*m_iterator++ = *string++;
					*m_iterator = CharacterSet::CharBase<U>::EndOfString;
				}
			};
		}
	}
}