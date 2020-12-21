#pragma once
namespace Runtime
{
//Provide well-defined alias for runtime

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
	using Int = Int64;
#else
	using IntPtr = Int32;
	using Int = Int32;
#endif
	using RTString = const wchar_t*;
#define Text(T) L##T

#define RT  Runtime
#define RTC Runtime::Core
#define RTD Runtime::Core::Descriptor
#define RTO Runtime::Core::ManagedObject
#define RTB Runtime::Core::Builder
#define RTE Runtime::Exception
#define RTGC Runtime::Core::GC
#define RTJ Runtime::Core::JIT
#define RTJE Runtime::Core::JIT::Emit
#define RTIOS2EE Runtime::Core::Interfaces::OSToEE
	
#define ForcedInline __forceinline
}