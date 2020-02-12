#pragma once
namespace HL
{
	namespace System
	{
		namespace Text
		{
			namespace RegularExpression
			{
				namespace Runtime
				{
					class Executable
					{

					};

					class IEmitter
					{
					public:
						typedef Collection::Generic::Array<OpCode> CodeStream;
						virtual void Emit(Executable&, CodeStream const&) = 0;
						virtual void Emit(Executable&, OpCode const&) = 0;
					};
				}
			}
		}
	}
}