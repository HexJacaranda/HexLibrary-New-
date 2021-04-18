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

RTJ::Hex::CallNode* RTJ::Hex::ILTransformer::GenerateCall()
{
	TreeNode** arguments = nullptr;
	UInt16 argumentsCount = mBaeIn;
	if (mBaeIn > 0)
	{	
		//Overceed 14 arguments, Extended uint16 will be used
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
	auto local = new LocalVariableNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>());
	if (SLMode == SLMode::Indirect)
		return new LoadNode(SLMode::Indirect, local);
	else
		return local;
}

RTJ::Hex::TreeNode* RTJ::Hex::ILTransformer::GenerateLoadArgument(UInt8 SLMode)
{
	auto local = new ArgumentNode(OpCodeOperandTypeConstant::I1, ReadAs<Int16>());
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
	auto arrayElement = new ArrayElementNode(array, index);
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

RTJ::Hex::NewNode* RTJ::Hex::ILTransformer::GenerateNew()
{
	return new NewNode(ReadAs<UInt32>());
}

RTJ::Hex::NewArrayNode* RTJ::Hex::ILTransformer::GenerateNewArray()
{
	TreeNode** dimensions = nullptr;
	UInt32 dimensionCount = mBaeIn;
	if (mBaeIn > 0)
	{
		//Overceed 14 dimensions, Extended uint32 will be used
		if (mBaeIn == 0xF)
			dimensionCount = ReadAs<UInt32>();
		dimensions = new TreeNode * [dimensionCount];
		for (int i = 0; i < dimensionCount; ++i)
			dimensions[i] = mEvalStack.Pop();
	}
	//Read method reference token
	UInt32 typeRef = ReadAs<UInt32>();
	return new NewArrayNode(typeRef, dimensions, dimensionCount);
}

RTJ::Hex::CompareNode* RTJ::Hex::ILTransformer::GenerateCompare()
{
	auto right = mEvalStack.Pop();
	auto left = mEvalStack.Pop();
	return new CompareNode(ReadAs<UInt8>(), left, right);
}

RTJ::Hex::DuplicateNode* RTJ::Hex::ILTransformer::GenerateDuplicate()
{
	auto target = mEvalStack.Top();
	if (target->Is(NodeKinds::Duplicate))
	{
		//To avoid a second wrapping and allocation, we return target directly
		//and increment its count to ensure duplicate node won't be released multiple times.
		return target->As<DuplicateNode>()->Duplicate();
	}
	return new DuplicateNode(target);
}

RTJ::Hex::ReturnNode* RTJ::Hex::ILTransformer::GenerateReturn()
{
	TreeNode* ret = nullptr;
	if (mBaeIn == 1)
		ret = mEvalStack.Pop();
	return new ReturnNode(ret);
}

RTJ::Hex::Statement* RTJ::Hex::ILTransformer::TryGenerateStatement(TreeNode* value, bool isBalancedCritical)
{
	//Is eval stack already balanced?
	if (mEvalStack.IsBalanced())
		return new Statement(value, mCodePtr - mJITContext.CodeSegment);
	else
	{
		if (!isBalancedCritical)
			return nullptr;
		//Should never reach here, malformed IL
	}
}

RTJ::Hex::Statement* RTJ::Hex::ILTransformer::TransformToUnpartitionedStatements(std::vector<BasicBlockPartition>& partitions)
{
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
#define IL_TRY_GEN_STMT_CRITICAL(OP, CRITICAL) auto node = OP; \
							stmtCurrent = TryGenerateStatement(node, CRITICAL); \
							if (stmtCurrent == nullptr) { mEvalStack.Push(node); } \
							else threadLinkList(stmtHead, stmtPrevious, stmtCurrent)

	//Try generate statement for operation and thread it if possible
#define IL_TRY_GEN_STMT(OP) IL_TRY_GEN_STMT_CRITICAL(OP, false)

	UInt8 instruction = OpCodes::Nop;
	while (mCodePtr < mCodePtrBound)
	{
		DecodeInstruction(instruction);
		switch (instruction)
		{
		case OpCodes::Call:
		case OpCodes::CallVirt:
		{
			IL_TRY_GEN_STMT_CRITICAL(GenerateCall(), mBaeOut == 0);
			break;
		}
		case OpCodes::Ret:
		{
			IL_TRY_GEN_STMT_CRITICAL(GenerateReturn(), true);
			break;
		}
		case OpCodes::Jmp:
		{

		}
		case OpCodes::Jcc:
		{

		}

		case OpCodes::StArg:
		{
			IL_TRY_GEN_STMT(GenerateStoreArgument());
			break;
		}
		case OpCodes::StElem:
		{
			IL_TRY_GEN_STMT(GenerateStoreArrayElement());
			break;
		}
		case OpCodes::StFld:
		{
			IL_TRY_GEN_STMT(GenerateStoreField());
			break;
		}
		case OpCodes::StLoc:
		{
			IL_TRY_GEN_STMT(GenerateStoreLocal());
			break;
		}
		case OpCodes::StTA:
		{
			IL_TRY_GEN_STMT(GenerateStoreToAddress());
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
			break;
		case OpCodes::Pop:
		{
			// Pop is a special instruction that may be introduced in scenario
			// where the result of expression is discarded (usually method call with return). 
			// So there is no need for a real 'PopNode' to exist. We will just treat this as
			// a way to balance eval stack.

			// In another word, pop means eval stack must be balanced after this operation
			// and this does impose limitations on some special and unregulated IL arrangements
			// For example:
			// ------------------
			// New System::String
			// New System::Object
			// Pop
			// Pop
			// Ret
			// ------------------
			// is not allowed.

			IL_TRY_GEN_STMT_CRITICAL(mEvalStack.Pop(), true);
			break;
		}

		case OpCodes::Cmp:
			mEvalStack.Push(GenerateCompare());
			break;

		case OpCodes::New:
			mEvalStack.Push(GenerateNew());
			break;
		case OpCodes::NewArr:
			mEvalStack.Push(GenerateNewArray());
			break;


		default:
			//You should never reach here

			break;
		}
	}
	if (!mEvalStack.IsBalanced())
	{
		//Malformed IL
	}
	return stmtHead;

#undef IL_TRY_GEN_STMT
#undef IL_TRY_GEN_STMT_CRITICAL
}

RTJ::Hex::ILTransformer::ILTransformer(JITContext const& context) :mJITContext(context) {
	mCodePtr = mJITContext.CodeSegment;
	mCodePtrBound = mJITContext.CodeSegment + mJITContext.SegmentLength;
}

RTJ::Hex::BasicBlock* RTJ::Hex::ILTransformer::TransformILFrom()
{


}


