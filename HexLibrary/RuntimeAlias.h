#pragma once
namespace HL::System::Runtime
{
	using Int8 = __int8;
	using Int16 = __int16;
	using Int32 = __int32;
	using Int64 = __int64;
	using UInt8 = unsigned __int8;
	using UInt16 = unsigned __int16;
	using UInt32 = unsigned __int32;
	using UInt64 = unsigned __int64;
#ifdef _M_AMD64
	using IntPtr = Int64;
#else
	using IntPtr = Int32;
#endif

#define RT HL::System::Runtime
#define RTC HL::System::Runtime::Core
#define RTD HL::System::Runtime::Core::Descriptor
#define RTO HL::System::Runtime::Core::ManagedObject
}