#pragma once
#include "..\RuntimeAlias.h"

namespace RTC
{
	enum class Accessibility :UInt8 {
		Public,
		Private,
		Protected,
		Internal
	};
}