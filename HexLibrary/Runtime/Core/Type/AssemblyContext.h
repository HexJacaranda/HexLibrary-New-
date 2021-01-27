#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class Type;
}
namespace RTC
{
	class AssemblyContextLoadingType
	{
	public:
		//Reserved for 0 - 3 bit
		static constexpr UInt32 Mask = 0x0000000F;
		//Assembly loaded by runtime(which also means it's shared and neutral)
		static constexpr UInt32 Core = 0x00000001;
		//Assembly loaded by user application in default way
		static constexpr UInt32 Normal = 0x00000002;
		//Assembly loaded by user application in shared way
		static constexpr UInt32 Shared = 0x00000003;	
	};
	//To store the metadata loaded in this assembly
	class AssemblyContext
	{
		Int32 mId;
		UInt32 mFlag;
	public:
		inline Int32 GetId()const;
		inline UInt32 GetLoadingType()const;
		void LoadType(Type* value);
	};
}