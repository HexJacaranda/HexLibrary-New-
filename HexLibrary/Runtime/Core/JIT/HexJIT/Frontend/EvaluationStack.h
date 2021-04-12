#pragma once
#include "..\..\..\..\RuntimeAlias.h"
#include "IR.h"
namespace RTJ::Hex
{
	class EvaluationStack
	{
		/// <summary>
		/// Evaluation stack for tree construction
		/// </summary>
		TreeNode** mEvaluationStack = nullptr;
		/// <summary>
		/// Indicator
		/// </summary>
		Int32 mIndex = 0;
		/// <summary>
		/// Indicate max depth of evaluation stack
		/// </summary>
		static constexpr Int32 EvaluationStackDepth = 256;
	public:
		EvaluationStack() {
			mEvaluationStack = new TreeNode * [EvaluationStackDepth];
		}
		~EvaluationStack() {
			if (mEvaluationStack != nullptr)
			{
				delete[] mEvaluationStack;
				mEvaluationStack = nullptr;
			}
		}

		TreeNode* Push(TreeNode*);
		TreeNode* Pop();
		TreeNode* Top()const;
		bool IsBalanced();
	};
}
