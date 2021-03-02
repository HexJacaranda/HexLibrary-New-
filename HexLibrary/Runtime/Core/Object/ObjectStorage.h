#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTO
{
	class Object;
}

namespace RTO
{
	//Used to store additional information
	class ObjectStorage
	{
	private:
#ifdef X64
		UInt32 __X64Align;
#endif
		/// <summary>
		/// Storage is actually a bit set to store additional information.
		/// <para>0 bit: GC visit mark.</para>
		/// <para>1 bit: Pin mark.</para>
		/// </summary>
		UInt32 mStorage;
	public:
		Object* GetObject()const;
	};
}