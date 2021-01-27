#pragma once
#include "RuntimeAlias.h"
namespace RTO
{
	class Object;
}

namespace RTO
{
	//Used to store additional information for 
	class ObjectStorage
	{
	private:
#ifdef X64
		UInt32 __X64Align;
#endif
		UInt32 mStorage;
	public:
		Object* GetObject()const;
	};
}