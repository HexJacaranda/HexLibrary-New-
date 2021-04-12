#pragma once
#include "..\..\RuntimeAlias.h"

namespace RTM
{
	struct TypeDefinitionSlot
	{
		UInt32 Token;
		UInt32 NameToken;
	};

	struct TypeReferenceSlot
	{
		UInt32 Token;
		//Referenced information

		UInt32 AssemblyReferenceToken;
		UInt32 TypeDefinitionToken;
	};

	/// <summary>
	/// Member reference could be field, method
	/// </summary>
	struct MemberReferenceSlot
	{
		UInt32 Token;
		//Referenced information

		UInt32 TypeReferenceToken;
		/// <summary>
		/// The type of member can be determined by JIT via opcodes.
		/// </summary>
		UInt32 MemberDefinitionToken;
	};


	class AssemblyManager
	{
	public:
		RTString* StringTable;

	};

	struct AssemblyReferenceSlot
	{
		UInt32 Token;
		UInt32 NameToken;
		AssemblyManager* Assembly;
	};
}