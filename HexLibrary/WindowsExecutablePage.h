#pragma once
namespace HL::System::Runtime::Interfaces::OSToEE::Windows
{
	class ExecutablePage:public IExecutablePage
	{
		Int32 m_length;
		Int8* m_page;
		Int32 m_index;
	public:
		ExecutablePage(Int32 length) :m_index(0), m_length(length) {
			if (length == 0)
				m_length = 32;
			m_page = (Int8*)VirtualAlloc(nullptr, m_length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		}
		virtual Int8* GetRawPage() 
		{
			return m_page;
		}
		virtual Int32 Length() 
		{
			return m_length;
		}
		virtual Int32 Current()
		{
			return m_index;
		}
		virtual Int8* PrepareWrite(Int32 value) 
		{
			if (m_index + value >= m_length) {
				Int8* newer = (Int8*)VirtualAlloc(nullptr, m_length * 2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);			
				if (newer == nullptr)
				{

				}
				Memory::MemoryCopy(newer, m_page, m_index);
				VirtualFree(m_page, 0, MEM_RELEASE);
				m_page = newer;
				m_length = m_length * 2;
			}
			return m_page + m_index;
		}
		virtual void CommitWrite(Int32 value)
		{
			m_index += value;
		}
		virtual ~ExecutablePage()
		{
			if (m_page != nullptr)
			{
				VirtualFree(m_page, 0, MEM_RELEASE);
				m_page = nullptr;
				m_length = m_index = 0;
			}
		}
	};
}