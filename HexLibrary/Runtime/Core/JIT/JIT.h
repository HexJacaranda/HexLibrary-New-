#pragma once
#include "..\..\RuntimeAlias.h"
#include "JITContext.h"
namespace RTJ
{
	class INativeJIT
	{
	public:
		virtual void* Compile(JITContext) = 0;
	};
}