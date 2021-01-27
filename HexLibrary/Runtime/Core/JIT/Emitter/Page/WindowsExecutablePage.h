#pragma once
#include "..\..\..\..\RuntimeAlias.h"
#include "ExecutablePage.h"

namespace RTIOS2EE::Windows
{
	class ExecutablePage :public IExecutablePage
	{
		Int32 mLength;
		Int8* pPage;
		Int32 mIndex;
	public:
		ExecutablePage(Int32 Length);
		virtual Int8* GetRawPage();
		virtual Int32 Length();
		virtual Int32 Current();
		virtual Int8* PrepareWrite(Int32 Value);
		virtual void CommitWrite(Int32 Value);
		virtual ~ExecutablePage();
	};
}