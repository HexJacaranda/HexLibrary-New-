#pragma once
namespace HL::System::Runtime::JIT
{
	class HexJIT :public INativeJIT
	{
		Emit::INativeEmitter* m_il;

	public:
		virtual void* Compile(MethodInfo) {
			
			return nullptr;
		}
	};
}