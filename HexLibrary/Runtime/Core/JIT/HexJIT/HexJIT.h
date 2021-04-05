#pragma once
#include "..\..\..\RuntimeAlias.h"
#include "..\JIT.h"
namespace RTE
{
	class INativeEmitter;
}
namespace RTJ::Hex
{
	class HexJIT :public INativeJIT
	{
		RTE::INativeEmitter* m_emitter;

	public:
		virtual void* Compile(JITContext) {
			
			return nullptr;
		}
	};
}