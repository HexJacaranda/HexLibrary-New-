#include "Transformer.h"

#include "..\..\OpCodes.h"

ForcedInline void RTJ::Hex::ILTransformer::DecodeInstruction(UInt8& opcode, UInt8& in, UInt8& out)
{
	opcode = *mCodePtr;
	in = *(mCodePtr + 1) & 0xF0;
	out = *(mCodePtr + 1) & 0x0F;
}

RTJ::Hex::BasicBlock* RTJ::Hex::ILTransformer::TransformILFrom()
{
	BasicBlock* basicBlockHead = nullptr;
	BasicBlock* basicBlockPrevious = nullptr;
	BasicBlock* basicBlockCurrent = nullptr;

	Statement* stmtHead = nullptr;
	Statement* stmtPrevious = nullptr;
	Statement* stmtCurrent = nullptr;

	
	const UInt8* codePtrBound = mJITContext.CodeSegment + mJITContext.SegmentLength;

	UInt8 instruction = 0xFF;
	UInt8 baeIn = 0;
	UInt8 baeOut = 0;
	while (mCodePtr < codePtrBound)
	{
		DecodeInstruction(instruction, baeIn, baeOut);
		switch (instruction)
		{
		case OpCodes::Call:
		case OpCodes::CallVirt:

		case OpCodes::StArg:
		case OpCodes::StElem:
		case OpCodes::StFld:
		case OpCodes::StLoc:
		case OpCodes::StTA:

		case OpCodes::Add:

		default:
			break;
		}
	}
	return basicBlockHead;
}