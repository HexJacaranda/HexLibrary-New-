#pragma once
#include "..\..\RuntimeAlias.h"
#include "..\Type\Type.h"
namespace RTM
{
	/// <summary>
	/// Manager that responsible for metadata management
	/// </summary>
	class MetaManager
	{
	public:
		static Type* GetTypeFromToken(UInt32 typeReference);
		
	};
}