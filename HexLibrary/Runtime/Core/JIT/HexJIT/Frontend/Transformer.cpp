#include "Transformer.h"
#include "..\..\OpCodes.h"

ForcedInline void RTJ::Hex::ILTransformer::DecodeInstruction(UInt8& opcode)
{
	opcode = *mCodePtr;
	mCodePtr++;
	if (mCodePtr >= mCodePtrBound)
		throw 0;
	mBaeIn = *(mCodePtr) & 0xF0;
	mBaeOut = *(mCodePtr) & 0x0F;
	mCodePtr++;
}

RTJ::Hex::CallNode* RTJ::Hex::ILTransformer::GenerateCallNode()
{
	TreeNode** arguments = nullptr;
	UInt16 argumentsCount = mBaeIn;
	if (mBaeIn > 0)
	{	
		//Overceed 7 arguments, Extended uint16 will be used
		if (mBaeIn == 0xF)
			argumentsCount = ReadAs<UInt16>();
		arguments = new TreeNode * [argumentsCount];
		for (int i = 0; i < argumentsCount; ++i)
			arguments[i] = mEvalStack.Pop();
	}
	//Read method reference token
	UInt32 methodRef = ReadAs<UInt32>();
	return new CallNode(methodRef, arguments, argumentsCount);
}

RTJ::Hex::TreeNode* RTJ::Hex::ILTransformer::GenerateLoadLocalVariable(UInt8 SLMode)
{
	LocalVariableNode* local = new LocalVariableNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>());
	if (SLMode == SLMode::Indirect)
		return new LoadNode(SLMode::Indirect, local);
	else
		return local;
}

RTJ::Hex::TreeNode* RTJ::Hex::ILTransformer::GenerateLoadArgument(UInt8 SLMode)
{
	auto local = nullptr;
	ArgumentNode* local = new ArgumentNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>());
	if (SLMode == SLMode::Indirect)
		return new LoadNode(SLMode::Indirect, local);
	else
		return local;
}

RTJ::Hex::TreeNode* RTJ::Hex::ILTransformer::GenerateLoadField(UInt8 SLMode)
{
	UnaryNode* field = nullptr;
	if (mBaeIn == 0)
		field = new StaticFieldNode(ReadAs<UInt32>());
	else if (mBaeIn == 1)
		field = new InstanceFieldNode(ReadAs<UInt32>(), mEvalStack.Pop());
	else
	{
		//Should never reach here
	}

	if (SLMode == SLMode::Indirect)
		return new LoadNode(SLMode::Indirect, field);
	else
		return field;
}

RTJ::Hex::TreeNode* RTJ::Hex::ILTransformer::GenerateLoadArrayElement(UInt8 SLMode)
{
	auto index = mEvalStack.Pop();
	auto array = mEvalStack.Pop();
	ArrayElementNode* arrayElement = new ArrayElementNode(array, index);
	if (SLMode == SLMode::Indirect)
		return new LoadNode(SLMode::Indirect, arrayElement);
	else
		return arrayElement;
}

RTJ::Hex::StoreNode* RTJ::Hex::ILTransformer::GenerateStoreField()
{
	if (mBaeIn == 1)
		//Static field store
		return new StoreNode(new StaticFieldNode(ReadAs<UInt32>()), mEvalStack.Pop());
	else if (mBaeIn == 2)
	{
		//Instance field store
		auto value = mEvalStack.Pop();
		auto object = mEvalStack.Pop();
		return new StoreNode(new InstanceFieldNode(ReadAs<UInt32>(), object), value);
	}
	else
	{
		//Should never reach here
	}
}

RTJ::Hex::StoreNode* RTJ::Hex::ILTransformer::GenerateStoreArgument()
{
	return new StoreNode(
		new ArgumentNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>()),
		mEvalStack.Pop());
}

RTJ::Hex::StoreNode* RTJ::Hex::ILTransformer::GenerateStoreLocal()
{
	return new StoreNode(
		new LocalVariableNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>()),
		mEvalStack.Pop());
}

RTJ::Hex::StoreNode* RTJ::Hex::ILTransformer::GenerateStoreArrayElement()
{
	auto value = mEvalStack.Pop();
	auto index = mEvalStack.Pop();
	auto array = mEvalStack.Pop();

	return new StoreNode(
		new ArrayElementNode(array, index),
		value);
}

RTJ::Hex::StoreNode* RTJ::Hex::ILTransformer::GenerateStoreToAddress()
{
	auto value = mEvalStack.Pop();
	auto address = mEvalStack.Pop();
	return new StoreNode(address, value);
}

RTJ::Hex::DuplicateNode* RTJ::Hex::ILTransformer::GenerateDuplicate()
{
	TreeNode* target = mEvalStack.Top();
	if (target->Is(NodeKinds::Duplicate))
	{
		//To avoid a second wrapping, we return target directly
		return target->As<DuplicateNode>();
	}
	return new DuplicateNode(target);
}

RTJ::Hex::Statement* RTJ::Hex::ILTransformer::TryGenerateStatement(TreeNode* value, bool isBalancedCritical)
{
	//Is eval stack already balanced?
	if (mEvalStack.IsBalanced())
		return new Statement(value);
	else
	{
		if (!isBalancedCritical)
			return nullptr;
		//Should never reach here, malformed IL
	}
}

RTJ::Hex::ILTransformer::ILTransformer(JITContext const& context) :mJITContext(context) {
	mCodePtr = mJITContext.CodeSegment;
	mCodePtrBound = mJITContext.CodeSegment + mJITContext.SegmentLength;
}

RTJ::Hex::BasicBlock* RTJ::Hex::ILTransformer::TransformILFrom()
{
	BasicBlock* basicBlockHead = nullptr;
	BasicBlock* basicBlockPrevious = nullptr;
	BasicBlock* basicBlockCurrent = nullptr;

	Statement* stmtHead = nullptr;
	Statement* stmtPrevious = nullptr;
	Statement* stmtCurrent = nullptr;

	auto threadLinkList = [](auto& head, auto& previous, auto current) {
		if (head == nullptr)
			head = previous = current;
		else
		{
			previous->Next = current;
			current->Prev = previous;
			previous = current;
		}
	};
	//Try generate statement for store operation and thread it if possible
#define IL_TRY_GEN_ST_STMT(OP) auto node = OP; \
							stmtCurrent = TryGenerateStatement(node); \
							if (stmtCurrent == nullptr) { mEvalStack.Push(node); } \
							else threadLinkList(stmtHead, stmtPrevious, stmtCurrent)

	bool generateBB = false;
	UInt8 instruction = OpCodes::Nop;
	while (mCodePtr < mCodePtrBound)
	{
		DecodeInstruction(instruction);
		switch (instruction)
		{
		case OpCodes::Call:
		case OpCodes::CallVirt:
		{
			auto node = GenerateCallNode();
			stmtCurrent = TryGenerateStatement(node, mBaeOut == 0);
			//Has return value
			if (mBaeOut == 1) mEvalStack.Push(node);
			break;
		}
		case OpCodes::StArg:
		{
			IL_TRY_GEN_ST_STMT(GenerateStoreArgument());
			break;
		}
		case OpCodes::StElem:
		{
			IL_TRY_GEN_ST_STMT(GenerateStoreArrayElement());
			break;
		}
		case OpCodes::StFld:
		{
			IL_TRY_GEN_ST_STMT(GenerateStoreField());
			break;
		}
		case OpCodes::StLoc:
		{
			IL_TRY_GEN_ST_STMT(GenerateStoreLocal());
			break;
		}
		case OpCodes::StTA:
		{
			IL_TRY_GEN_ST_STMT(GenerateStoreToAddress());
			break;
		}

		case OpCodes::LdArg:
			mEvalStack.Push(GenerateLoadArgument(SLMode::Direct));
			break;
		case OpCodes::LdArgA:
			mEvalStack.Push(GenerateLoadArgument(SLMode::Indirect));
			break;
		case OpCodes::LdLoc:
			mEvalStack.Push(GenerateLoadLocalVariable(SLMode::Direct));
			break;
		case OpCodes::LdLocA:
			mEvalStack.Push(GenerateLoadLocalVariable(SLMode::Indirect));
			break;
		case OpCodes::LdElem:
			mEvalStack.Push(GenerateLoadArrayElement(SLMode::Direct));
			break;
		case OpCodes::LdElemA:
			mEvalStack.Push(GenerateLoadArrayElement(SLMode::Indirect));
			break;

		case OpCodes::Dup:
			mEvalStack.Push(GenerateDuplicate());

		case OpCodes::Cmp:

		default:
			//You should never reach here

			break;
		}

		//If there is a generated basic block, link it to list
		if (generateBB)
			threadLinkList(basicBlockHead, basicBlockPrevious, basicBlockCurrent);
	}
	if (!mEvalStack.IsBalanced())
	{
		//Malformed IL
	}
	return basicBlockHead;

#undef IL_TRY_GEN_ST_STMT
}


