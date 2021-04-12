#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	/// <summary>
	/// Core types are introduced for convenience. It can be used to
	/// fasten check for special types (primitives, array, object, etc.)
	/// and to help jit frontend check type safety without querying type
	/// system.
	/// </summary>
	class CoreTypes
	{
	public:
		//Primitives

		static constexpr UInt8 I1 = 0x00;
		static constexpr UInt8 I2 = 0x01;
		static constexpr UInt8 I4 = 0x02;
		static constexpr UInt8 I8 = 0x03;

		static constexpr UInt8 U1 = 0x04;
		static constexpr UInt8 U2 = 0x05;
		static constexpr UInt8 U4 = 0x06;
		static constexpr UInt8 U8 = 0x07;

		/// <summary>
		/// Half
		/// </summary>
		static constexpr UInt8 R2 = 0x08;
		/// <summary>
		/// Single
		/// </summary>
		static constexpr UInt8 R4 = 0x09;
		/// <summary>
		/// Double
		/// </summary>
		static constexpr UInt8 R8 = 0x0A;
		/// <summary>
		/// Custom structure
		/// </summary>
		static constexpr UInt8 Struct = 0x0B;

		/// <summary>
		/// Pointer
		/// </summary>
		static constexpr UInt8 Ref = 0x0C;

		//Detailed type representation

		static constexpr UInt8 Object = 0x10;
		static constexpr UInt8 SZArray = 0x11;
		static constexpr UInt8 Array = 0x12;
		static constexpr UInt8 String = 0x13;
		static constexpr UInt8 Delegate = 0x14;
	};
}