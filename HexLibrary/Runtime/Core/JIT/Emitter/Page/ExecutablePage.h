#pragma once
#include "..\..\..\..\RuntimeAlias.h"
namespace RTIOS2EE
{
	class IExecutablePage
	{
	public:
		virtual Int8* GetRawPage() = 0;
		virtual Int32 Length() = 0;
		virtual Int32 Current() = 0;
		virtual Int8* PrepareWrite(Int32) = 0;
		virtual void CommitWrite(Int32) = 0;
		virtual ~IExecutablePage() {}
	};
}