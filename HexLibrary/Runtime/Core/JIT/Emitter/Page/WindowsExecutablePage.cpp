#include "WindowsExecutablePage.h"
#include "..\..\..\Exception\RuntimeException.h"

#include <Windows.h>
RTIOS2EE::Windows::ExecutablePage::ExecutablePage(Int32 Length)
	:mIndex(0), mLength(Length) {
	if (Length == 0)
		mLength = 32;
	pPage = (Int8*)VirtualAlloc(nullptr, mLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

RT::Int8* RTIOS2EE::Windows::ExecutablePage::GetRawPage()
{
	return pPage;
}

RT::Int32 RTIOS2EE::Windows::ExecutablePage::Length()
{
	return mLength;
}

RT::Int32 RTIOS2EE::Windows::ExecutablePage::Current()
{
	return mIndex;
}

RT::Int8* RTIOS2EE::Windows::ExecutablePage::PrepareWrite(Int32 Value)
{
	if (mIndex + Value >= mLength) {
		Int8* newSpace = (Int8*)VirtualAlloc(nullptr, ((UInt64)mLength) << 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (newSpace == nullptr)
			RTE::Throw(Text("Unable to allocate virtual pages for native code generation"));
		memcpy_s(newSpace, ((UInt64)mLength) << 1, pPage, mIndex);
		VirtualFree(pPage, 0, MEM_RELEASE);
		pPage = newSpace;
		mLength <<= 1;
	}
	return pPage + mIndex;
}

void RTIOS2EE::Windows::ExecutablePage::CommitWrite(Int32 Value)
{
	mIndex += Value;
}

RTIOS2EE::Windows::ExecutablePage::~ExecutablePage()
{
	if (pPage != nullptr)
	{
		VirtualFree(pPage, 0, MEM_RELEASE);
		pPage = nullptr;
		mLength = mIndex = 0;
	}
}

