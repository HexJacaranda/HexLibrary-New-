#pragma once
namespace HL::System::Runtime::Core
{
	class EEClass
	{
		UInt16 mInstaceFieldsCount;
		UInt16 mStaticFieldsCount;
		UInt16 mGCPointersCount;
		MethodTable* pMethodTable;

	};
}