#pragma once
namespace HL::System::Runtime::JIT
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