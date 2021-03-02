#include "Emitter.h"
#include ".\Page\ExecutablePage.h"

RTJE::EmitContext* RTJE::INativeEmitter::GetEmitContext()
{
    return mContext;
}

void RTJE::INativeEmitter::SetExecutablePage(RTIOS2EE::IExecutablePage* value)
{
    mPage = value;
}

RTIOS2EE::IExecutablePage* RTJE::INativeEmitter::GetExecutablePage()
{
    return mPage;
}

RT::Int32 RTJE::INativeEmitter::CurrentPosition()
{
    return mPage->Current();
}

RT::Int8* RTJE::INativeEmitter::CurrentPointer()
{
    return mPage->Current() + mPage->GetRawPage();
}
