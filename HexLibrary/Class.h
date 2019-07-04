#pragma once
namespace HL
{
	namespace System
	{
		namespace GC
		{
			struct Buffer;
			struct MemorySlot;
			struct Block;
			struct MemorySpan;
		}
		namespace Concurrency
		{
			template<class T>
			class ConcurrentNodeWrapper;
			template<class T>
			class ConcurrentNode;
		}
		namespace Linq
		{
			template<class IteratorT, class FilterT>
			class WhereIterator;
			template<class IteratorT,class ConstIteratorT, class Derived>
			class LinqBase;
			template<class IteratorT>
			class LinqResult;
		}
		namespace Collection
		{
			namespace Generic
			{
				template<class TKey,class TValue>
				class Dictionary;
				template<class U>class Array;
				template<class U>class List;
				template<class U>class Stack;
			}
			template<class...Args>class Tuple;
		}
		namespace Text
		{
			namespace RegularExpression
			{
				class Regex;
				class MatchResult;

				class Edge;
				class Status;
				class ContentBase;
				class NFA;
				class Resource;
			}
		}
		namespace Delegation
		{
			template<class R, class...Args>class Delegate;
			template<class R, class...Args>class MulticastDelegate;
		}
		namespace Json
		{
			class IJsonValue;
			class JsonValue;
			class JsonArray;
			class JsonObject;
		}
		namespace Pointer
		{
			struct me_setter;
			template<class U>class me;
			template<class U>class ptr;
			template<class U>class weakptr;
		}
		namespace Threading
		{
			class Thread;
		}
		namespace Script
		{
			class Descriptor;
			class FunctionDescriptor;
			class VaribaleDescriptor;
			class ExpressionDescriptor;
			class ClassDescriptor;
			class FieldDescriptor;
			class PropertyDescriptor;
			class TypeDescriptor;
			class ConditionDescriptor;
			class ArgumentDescriptor;
		}
	}
}