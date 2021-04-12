#include "EvaluationStack.h"

RTJ::Hex::TreeNode* RTJ::Hex::EvaluationStack::Push(TreeNode* target)
{
	mEvaluationStack[mIndex++] = target;
	return target;
}

RTJ::Hex::TreeNode* RTJ::Hex::EvaluationStack::Pop()
{
	return mEvaluationStack[--mIndex];
}

RTJ::Hex::TreeNode* RTJ::Hex::EvaluationStack::Top() const
{
	return mEvaluationStack[mIndex - 1];
}

bool RTJ::Hex::EvaluationStack::IsBalanced()
{
	return mIndex == 0;
}