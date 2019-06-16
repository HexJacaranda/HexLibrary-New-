#pragma once
#include <stdio.h>
namespace HL
{
	namespace System
	{
		namespace IO
		{
			class FileNotFoundException :public Exception::IException
			{
				wchar_t m_messgae[255 + 32];
			public:
				FileNotFoundException(wchar_t const* Path)
				{
					Text::TextWriter<wchar_t> writer(m_messgae);
					writer.Write(L"Path Not Found: ");
					writer.Write(Path);
				}
				virtual const wchar_t* Message()const {
					return m_messgae;
				}
			};

			enum class Encoding : short
			{
				ANSI = 0,
				Unicode = 1,
				UTF8 = 2,
				UTF16 = 3
			};

			enum class FileMode
			{
				Write,
				Read,
				Append,
				WritePlus,
				ReadPlus,
				AppendPlus
			};

			class FileHandle
			{
				FILE* m_handle = nullptr;
				FileMode m_mode;
				Encoding m_encoding;
				static void Option(wchar_t*buffer, FileMode mode, Encoding encoding)noexcept {
					Text::TextWriter<wchar_t> writer(buffer);
					switch (mode)
					{
					case HL::System::IO::FileMode::Write:
						writer.Write(L"w");
						break;
					case HL::System::IO::FileMode::Read:
						writer.Write(L"r");
						break;
					case HL::System::IO::FileMode::Append:
						writer.Write(L"a");
						break;
					case HL::System::IO::FileMode::WritePlus:
						writer.Write(L"w+");
						break;
					case HL::System::IO::FileMode::ReadPlus:
						writer.Write(L"r+");
						break;
					case HL::System::IO::FileMode::AppendPlus:
						writer.Write(L"a+");
						break;
					default:
						break;
					}

					switch (encoding)
					{
					case HL::System::IO::Encoding::Unicode:
						writer.Write(L", ccs=UNICODE");
						break;
					case HL::System::IO::Encoding::UTF8:
						writer.Write(L", ccs=UTF-8");
						break;
					case HL::System::IO::Encoding::UTF16:
						writer.Write(L", ccs=UTF-16LE");
						break;
					case HL::System::IO::Encoding::ANSI:
						break;
					}
				}
			public:
				typedef FILE* NativeHandle;
				typedef Pointer::ptr<FileHandle> SharedFile;
				FileHandle(const wchar_t*path, FileMode mode, Encoding encoding = Encoding::ANSI)
					:m_mode(mode),m_encoding(encoding) {
					if (path == nullptr)
						throw Exception::InvalidArgumentException();
					wchar_t option[64];
					Option(option, mode, encoding);
					_wfopen_s(&m_handle, path, option);
					if (m_handle == nullptr)
						throw FileNotFoundException(path);
				}
				FileHandle(FileHandle&&) = default;
				FileHandle(FileHandle const&) = delete;
				FileHandle& operator=(FileHandle&&lhs) {
					if (m_handle != nullptr)
					{
						fclose(m_handle);
						m_handle = nullptr;
					}
					Utility::move_assign(m_handle, lhs.m_handle);
					m_mode = lhs.m_mode;
					m_encoding = lhs.m_encoding;
					return *this;
				}
				FileHandle& operator=(FileHandle const&) = delete;
				~FileHandle() {
					if (m_handle != nullptr)
					{
						fclose(m_handle);
						m_handle = nullptr;
					}
				}
				inline NativeHandle GetNativeHandle()const noexcept {
					return m_handle;
				}
			};

			class File
			{
			public:
				static long TellFileSize(FileHandle const&handle)noexcept {
					fpos_t origin = 0;
					fpos_t begin = 0;
					FileHandle::NativeHandle file = handle.GetNativeHandle();
					fgetpos(file, &origin);
					fsetpos(file, &begin);
					fseek(file, 0, SEEK_END);
					long result = ftell(file);
					fsetpos(file, &origin);
					return result;
				}
				static long TellFileSize(const wchar_t*Path)noexcept {
					FILE*file_handle = nullptr;
					_wfopen_s(&file_handle, Path, L"r");
					if (file_handle == nullptr)
						return 0;
					fseek(file_handle, 0, SEEK_END);
					long r = ftell(file_handle);
					fclose(file_handle);
					return r;
				}
				static Text::WString Read(const wchar_t*Path) {
					FILE*file_handle = nullptr;
					_wfopen_s(&file_handle, Path, L"r");
					if (!file_handle)
						return Text::WString();
					long length = TellFileSize(Path) + 1;
					Collection::LinearMemoryManager<wchar_t> buffer(length);
					while (!feof(file_handle))
						buffer.Append(fgetwc(file_handle));
					fclose(file_handle);
					return std::move(buffer);
				}
				static Text::WString Read(FileHandle const&handle) {
					FileHandle::NativeHandle file = handle.GetNativeHandle();
					long length = TellFileSize(handle) + 1;
					Collection::LinearMemoryManager<wchar_t> buffer(length);
					while (!feof(file))
						buffer.Append(fgetwc(file));
					return std::move(buffer);
				}
				static Collection::Generic::List<Text::WString> ReadLines(FileHandle const&handle) {
					wchar_t buffer[1024];
					FileHandle::NativeHandle file = handle.GetNativeHandle();
					Collection::Generic::List<Text::WString> ret;
					while (!feof(file))
					{
						fgetws(buffer, 1024, file);
						Text::WString line;
						line.Append(buffer);
						ret.Add(std::move(line));
					}
					return std::move(ret);
				}
				inline static Pointer::ptr<Threading::Tasks::Task<Text::WString>> ReadAsync(FileHandle::SharedFile handle) {
					return Threading::Tasks::Async([=]() {
						return Read(handle.GetObject());
					});
				}
				inline static Pointer::ptr<Threading::Tasks::Task<Collection::Generic::List<Text::WString>>> ReadLinesAsync(FileHandle::SharedFile handle) {
					return Threading::Tasks::Async([=]() {
						return ReadLines(handle.GetObject());
					});
				}
				inline static Pointer::ptr<Threading::Tasks::Task<Text::WString>> ReadAsync(const wchar_t*Path) {
					return Threading::Tasks::Async([=]() {
						return Read(Path);
					});
				}
			};
		}
	}
}