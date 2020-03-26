#pragma once
namespace HL::System::Runtime::Interfaces::OSToEE::Windows
{
	class ExecutablePage:public IExecutablePage
	{
		Int32 m_length;
		Int8* m_page;
		Int32 m_index;
	public:
		virtual Int8* GetRawPage() 
		{
			return m_page;
		}
		virtual Int32 Length() 
		{
			return m_length;
		}
		virtual Int8* PrepareWrite(Int32 value) 
		{
			if (m_index + value >= m_length) {
				Int8* newer = (Int8*)VirtualAlloc(nullptr, m_length * 2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);			
				if (newer == nullptr)
				{
				
				}
				Memory::MemoryCopy(newer, m_page, m_index);
				VirtualFree(m_page, m_length, MEM_DECOMMIT);	
				m_page = newer;
				m_length = m_length * 2;
			}
			return m_page + m_index;
		}
		virtual void CommitWrite(Int32 value) 
		{
			m_index += value;
		}
	};
}