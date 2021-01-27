#include "Emitter.h"
#include ".\Page\ExecutablePage.h"

RTJE::EmitContext* RTJE::INativeEmitter::GetEmitContext()
{
    return m_context;
}

void RTJE::INativeEmitter::SetExecutablePage(RTIOS2EE::IExecutablePage* value)
{
    m_page = value;
}

RTIOS2EE::IExecutablePage* RTJE::INativeEmitter::GetExecutablePage()
{
    return m_page;
}

RT::Int32 RTJE::INativeEmitter::CurrentPosition()
{
    return m_page->Current();
}

RT::Int8* RTJE::INativeEmitter::CurrentPointer()
{
    return m_page->Current() + m_page->GetRawPage();
}
