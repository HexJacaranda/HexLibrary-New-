#pragma once
#include "RuntimeAlias.h"
namespace RTJ
{
	class MethodInfo
	{
	public:

	};
	
	class INativeJIT
	{
	public:
		virtual void* Compile(MethodInfo) = 0;
	};
}