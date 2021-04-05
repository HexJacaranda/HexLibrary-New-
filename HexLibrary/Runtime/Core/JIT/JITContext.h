#pragma once
#include "..\..\RuntimeAlias.h"

namespace RTJ
{
	class JITContext
	{
	public:
		class Performance
		{
		public:
			static constexpr UInt32 AggressiveInline = 0x00000001;
		};
		/// <summary>
		/// Performance settings
		/// </summary>
		UInt32 PerformanceSettings;
		/// <summary>
		/// Multiple tier compilation
		/// </summary>
		UInt16 TierSettings;
		/// <summary>
		/// OpCode segment
		/// </summary>
		UInt8* CodeSegment;
		/// <summary>
		/// Segment length
		/// </summary>
		Int32 SegmentLength;
	};
}