#pragma once
#include "..\..\..\..\RuntimeAlias.h"
#include "..\..\JITContext.h"

namespace RTJ::Hex
{
	struct TreeNode
	{
		Int16 Kind;
		union
		{
			TreeNode* Left;
			TreeNode** Args;
		};
		union
		{
			TreeNode* Right;
			TreeNode* Callee;
			Int64 Value;
		};
	};

	struct Statement
	{
		Statement* Next;
		Statement* Prev;
		TreeNode* Now;
	};

	struct BasicBlock
	{
		BasicBlock* Next;
		BasicBlock* Prev;
		Statement* Now;
	};

	struct NodeKind
	{
		static constexpr Int16 Assign = 0;
		static constexpr Int16 Call = 0;
		static constexpr Int16 Ret = 0;
		static constexpr Int16 Jmp = 0;
		static constexpr Int16 Jcc = 0;

		static constexpr Int16 Add = 0;
		static constexpr Int16 Sub = 0;
		static constexpr Int16 Mul = 0;
		static constexpr Int16 Div = 0;
	};
	/// <summary>
	/// Transform the opcode to basic block expression tree.
	/// </summary>
	class ILTransformer
	{
		JITContext mJITContext;
		UInt8* mCodePtr = nullptr;
		/// <summary>
		/// Decode the instruction at current memory
		/// </summary>
		/// <param name="opcode">opcode value</param>
		/// <param name="in">Bae group bringing in value</param>
		/// <param name="out">Bae group bringing out value</param>
		ForcedInline void DecodeInstruction(UInt8& opcode, UInt8& in, UInt8& out);
	public:
		ILTransformer(JITContext const& context) :mJITContext(context) {
			mCodePtr = mJITContext.CodeSegment;
		}
		BasicBlock* TransformILFrom();
	};
}