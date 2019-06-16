#pragma once
namespace HL
{
	namespace System
	{
		namespace Value
		{
			template<class U>
			struct Max
			{
				static constexpr U Value = ((U)1 << sizeof(U) * 8) - 1;
			};
		}
	}
}