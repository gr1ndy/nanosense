#include "Convar.hpp"

#include "../sdk.hpp"

#include "characterset.hpp"
#include "UtlBuffer.hpp"

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) 
#define stackalloc( _size )		_alloca( ALIGN_VALUE( _size, 16 ) )

ConCommandBase *ConCommandBase::s_pConCommandBases = NULL;
ConCommandBase *ConCommandBase::s_pRegisteredCommands = NULL;
IConCommandBaseAccessor	*ConCommandBase::s_pAccessor = NULL;
static int s_nDLLIdentifier = -1;
static int s_nCVarFlag = 0;
static bool s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
    virtual bool RegisterConCommandBase(ConCommandBase *pVar)
    {
        // Link to engine's list instead
        g_CVar->RegisterConCommand(pVar);
        return true;
    }
};

static CDefaultAccessor s_DefaultAccessor;

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommandBases with the ICVar
//-----------------------------------------------------------------------------
void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
    if(!g_CVar || s_bRegistered)
        return;

    assert(s_nDLLIdentifier < 0);
    s_bRegistered = true;
    s_nCVarFlag = nCVarFlag;
    s_nDLLIdentifier = g_CVar->AllocateDLLIdentifier();

    ConCommandBase *pCur, *pNext;

    ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
    pCur = ConCommandBase::s_pConCommandBases;

    while(pCur) {
        pNext = pCur->m_pNext;
        pCur->AddFlags(s_nCVarFlag);
        pCur->Init();

        ConCommandBase::s_pRegisteredCommands = pCur;

        pCur = pNext;
    }

    ConCommandBase::s_pConCommandBases = NULL;
}

void ConVar_Unregister()
{
    if(!g_CVar || !s_bRegistered)
        return;

    assert(s_nDLLIdentifier >= 0);
    g_CVar->UnregisterConCommands(s_nDLLIdentifier);
    s_nDLLIdentifier = -1;
    s_bRegistered = false;
}

ConCommandBase::ConCommandBase(void)
{
    m_bRegistered = false;
    m_pszName = NULL;
    m_pszHelpString = NULL;

    m_nFlags = 0;
    m_pNext = NULL;
}

ConCommandBase::ConCommandBase(const char *pName, const char *pHelpString /*=0*/, int flags /*= 0*/)
{
    Create(pName, pHelpString, flags);
}

ConCommandBase::~ConCommandBase(void)
{
}

bool ConCommandBase::IsCommand(void) const
{
    //	assert( 0 ); This can't assert. . causes a recursive assert in Sys_Printf, etc.
    return true;
}

CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier() const
{
    return s_nDLLIdentifier;
}

void ConCommandBase::Create(const char *pName, const char *pHelpString /*= 0*/, int flags /*= 0*/)
{
    static const char *empty_string = "";

    m_bRegistered = false;

    // Name should be static data
    m_pszName = pName;
    m_pszHelpString = pHelpString ? pHelpString : empty_string;

    m_nFlags = flags;

    if(!(m_nFlags & FCVAR_UNREGISTERED)) {
        m_pNext = s_pConCommandBases;
        s_pConCommandBases = this;
    } else {
        m_pNext = NULL;
    }
}

void ConCommandBase::Init()
{
    if(s_pAccessor) {
        s_pAccessor->RegisterConCommandBase(this);
    }
}

void ConCommandBase::Shutdown()
{
    if(g_CVar) {
        g_CVar->UnregisterConCommand(this);
    }
}

const char *ConCommandBase::GetName(void) const
{
    return m_pszName;
}

bool ConCommandBase::IsFlagSet(int flag) const
{
    return (flag & m_nFlags) ? true : false;
}

void ConCommandBase::AddFlags(int flags)
{
    m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
    m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags(void) const
{
    return m_nFlags;
}

const ConCommandBase *ConCommandBase::GetNext(void) const
{
    return m_pNext;
}

ConCommandBase *ConCommandBase::GetNext(void)
{
    return m_pNext;
}

char *ConCommandBase::CopyString(const char *from)
{
    int		len;
    char	*to;

    len = strlen(from);
    if(len <= 0) {
        to = new char[1];
        to[0] = 0;
    } else {
        to = new char[len + 1];
        strncpy_s(to, len + 1, from, len + 1);
    }
    return to;
}

const char *ConCommandBase::GetHelpText(void) const
{
    return m_pszHelpString;
}

bool ConCommandBase::IsRegistered(void) const
{
    return m_bRegistered;
}

static characterset_t s_BreakSet;
static bool s_bBuiltBreakSet = false;

CCommand::CCommand()
{
    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();
}

CCommand::CCommand(int nArgC, const char **ppArgV)
{
    assert(nArgC > 0);

    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();

    char *pBuf = m_pArgvBuffer;
    char *pSBuf = m_pArgSBuffer;
    m_nArgc = nArgC;
    for(int i = 0; i < nArgC; ++i) {
        m_ppArgv[i] = pBuf;
        int nLen = strlen(ppArgV[i]);
        memcpy(pBuf, ppArgV[i], nLen + 1);
        if(i == 0) {
            m_nArgv0Size = nLen;
        }
        pBuf += nLen + 1;

        bool bContainsSpace = strchr(ppArgV[i], ' ') != NULL;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }
        memcpy(pSBuf, ppArgV[i], nLen);
        pSBuf += nLen;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }

        if(i != nArgC - 1) {
            *pSBuf++ = ' ';
        }
    }
}

void CCommand::Reset()
{
    m_nArgc = 0;
    m_nArgv0Size = 0;
    m_pArgSBuffer[0] = 0;
}

characterset_t* CCommand::DefaultBreakSet()
{
    return &s_BreakSet;
}

bool CCommand::Tokenize(const char *pCommand, characterset_t *pBreakSet)
{
    Reset();
    if(!pCommand)
        return false;

    // Use default break Set
    if(!pBreakSet) {
        pBreakSet = &s_BreakSet;
    }

    // Copy the current command into a temp buffer
    // NOTE: This is here to avoid the pointers returned by DequeueNextCommand
    // to become invalid by calling AddText. Is there a way we can avoid the memcpy?
    int nLen = strlen(pCommand);
    if(nLen >= COMMAND_MAX_LENGTH - 1) {
        //Warning("CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n");
        return false;
    }

    memcpy(m_pArgSBuffer, pCommand, nLen + 1);

    // Parse the current command into the current command buffer
    CUtlBuffer bufParse(m_pArgSBuffer, nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
    int nArgvBufferSize = 0;
    while(bufParse.IsValid() && (m_nArgc < COMMAND_MAX_ARGC)) {
        char *pArgvBuf = &m_pArgvBuffer[nArgvBufferSize];
        int nMaxLen = COMMAND_MAX_LENGTH - nArgvBufferSize;
        int nStartGet = bufParse.TellGet();
        int	nSize = bufParse.ParseToken(pBreakSet, pArgvBuf, nMaxLen);
        if(nSize < 0)
            break;

        // Check for overflow condition
        if(nMaxLen == nSize) {
            Reset();
            return false;
        }

        if(m_nArgc == 1) {
            // Deal with the case where the arguments were quoted
            m_nArgv0Size = bufParse.TellGet();
            bool bFoundEndQuote = m_pArgSBuffer[m_nArgv0Size - 1] == '\"';
            if(bFoundEndQuote) {
                --m_nArgv0Size;
            }
            m_nArgv0Size -= nSize;
            assert(m_nArgv0Size != 0);

            // The StartGet check is to handle this case: "foo"bar
            // which will parse into 2 different args. ArgS should point to bar.
            bool bFoundStartQuote = (m_nArgv0Size > nStartGet) && (m_pArgSBuffer[m_nArgv0Size - 1] == '\"');
            assert(bFoundEndQuote == bFoundStartQuote);
            if(bFoundStartQuote) {
                --m_nArgv0Size;
            }
        }

        m_ppArgv[m_nArgc++] = pArgvBuf;
        if(m_nArgc >= COMMAND_MAX_ARGC) {
            //Warning("CCommand::Tokenize: Encountered command which overflows the argument buffer.. Clamped!\n");
        }

        nArgvBufferSize += nSize + 1;
        assert(nArgvBufferSize <= COMMAND_MAX_LENGTH);
    }

    return true;
}

const char* CCommand::FindArg(const char *pName) const
{
    int nArgC = ArgC();
    for(int i = 1; i < nArgC; i++) {
        if(!_stricmp(Arg(i), pName))
            return (i + 1) < nArgC ? Arg(i + 1) : "";
    }
    return 0;
}

int CCommand::FindArgInt(const char *pName, int nDefaultVal) const
{
    const char *pVal = FindArg(pName);
    if(pVal)
        return atoi(pVal);
    else
        return nDefaultVal;
}

int DefaultCompletionFunc(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
{
    return 0;
}

ConCommand::ConCommand(const char *pName, FnCommandCallbackV1_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallbackV1 = callback;
    m_bUsingNewCommandCallback = false;
    m_bUsingCommandCallbackInterface = false;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, FnCommandCallback_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallback = callback;
    m_bUsingNewCommandCallback = true;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;
    m_bUsingCommandCallbackInterface = false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, ICommandCallback *pCallback, const char *pHelpString /*= 0*/, int flags /*= 0*/, ICommandCompletionCallback *pCompletionCallback /*= 0*/)
{
    // Set the callback
    m_pCommandCallback = pCallback;
    m_bUsingNewCommandCallback = false;
    m_pCommandCompletionCallback = pCompletionCallback;
    m_bHasCompletionCallback = (pCompletionCallback != 0);
    m_bUsingCommandCallbackInterface = true;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::~ConCommand(void)
{
}

bool ConCommand::IsCommand(void) const
{
    return true;
}

void ConCommand::Dispatch(const CCommand &command)
{
    if(m_bUsingNewCommandCallback) {
        if(m_fnCommandCallback) {
            (*m_fnCommandCallback)(command);
            return;
        }
    } else if(m_bUsingCommandCallbackInterface) {
        if(m_pCommandCallback) {
            m_pCommandCallback->CommandCallback(command);
            return;
        }
    } else {
        if(m_fnCommandCallbackV1) {
            (*m_fnCommandCallbackV1)();
            return;
        }
    }

    // Command without callback!!!
    //AssertMsg(0, ("Encountered ConCommand without a callback!\n"));
}

int	ConCommand::AutoCompleteSuggest(const char *partial, CUtlVector< CUtlString > &commands)
{
    if(m_bUsingCommandCallbackInterface) {
        if(!m_pCommandCompletionCallback)
            return 0;
        return m_pCommandCompletionCallback->CommandCompletionCallback(partial, commands);
    }

    if(!m_fnCompletionCallback)
        return 0;

    char rgpchCommands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH];
    int iret = (m_fnCompletionCallback)(partial, rgpchCommands);
    for(int i = 0; i < iret; ++i) {
        CUtlString str = rgpchCommands[i];
        commands.AddToTail(str);
    }
    return iret;
}

bool ConCommand::CanAutoComplete(void)
{
    return m_bHasCompletionCallback;
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags /* = 0 */)
{
    Create(pName, pDefaultValue, flags);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString)
{
    Create(pName, pDefaultValue, flags, pHelpString);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
}

ConVar::~ConVar(void)
{
    //if(IsRegistered())
    //    convar->UnregisterConCommand(this);
    if(m_Value.m_pszString) {
        delete[] m_Value.m_pszString;
        m_Value.m_pszString = NULL;
    }
}

void ConVar::InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke)
{
    if(callback) {
        if(m_fnChangeCallbacks.GetOffset(callback) != -1) {
            m_fnChangeCallbacks.AddToTail(callback);
            if(bInvoke)
                callback(this, m_Value.m_pszString, m_Value.m_fValue);
        } else {
            //Warning("InstallChangeCallback ignoring duplicate change callback!!!\n");
        }
    } else {
        //Warning("InstallChangeCallback called with NULL callback, ignoring!!!\n");
    }
}

bool ConVar::IsFlagSet(int flag) const
{
    return (flag & m_pParent->m_nFlags) ? true : false;
}

const char *ConVar::GetHelpText(void) const
{
    return m_pParent->m_pszHelpString;
}

void ConVar::AddFlags(int flags)
{
    m_pParent->m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
    m_pParent->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
}

int ConVar::GetFlags(void) const
{
    return m_pParent->m_nFlags;
}

bool ConVar::IsRegistered(void) const
{
    return m_pParent->m_bRegistered;
}

const char *ConVar::GetName(void) const
{
    return m_pParent->m_pszName;
}

bool ConVar::IsCommand(void) const
{
    return false;
}

void ConVar::Init()
{
    BaseClass::Init();
}

const char *ConVar::GetBaseName(void) const
{
    return m_pParent->m_pszName;
}

int ConVar::GetSplitScreenPlayerSlot(void) const
{
    return 0;
}

void ConVar::InternalSetValue(const char *value)
{
    float fNewValue;
    char  tempVal[32];
    char  *val;

    auto temp = *(uint32_t*)&m_Value.m_fValue ^ (uint32_t)this;
    float flOldValue = *(float*)(&temp);

    val = (char *)value;
    fNewValue = (float)atof(value);

    if(ClampValue(fNewValue)) {
        snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
        val = tempVal;
    }

    // Redetermine value
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        ChangeStringValue(val, flOldValue);
    }
}

void ConVar::ChangeStringValue(const char *tempVal, float flOldValue)
{
    char* pszOldValue = (char*)stackalloc(m_Value.m_StringLength);
    memcpy(pszOldValue, m_Value.m_pszString, m_Value.m_StringLength);

    int len = strlen(tempVal) + 1;

    if(len > m_Value.m_StringLength) {
        if(m_Value.m_pszString) {
            delete[] m_Value.m_pszString;
        }

        m_Value.m_pszString = new char[len];
        m_Value.m_StringLength = len;
    }

    memcpy(m_Value.m_pszString, tempVal, len);

    // Invoke any necessary callback function
    for(int i = 0; i < m_fnChangeCallbacks.Count(); i++) {
        m_fnChangeCallbacks[i](this, pszOldValue, flOldValue);
    }

    if(g_CVar)
        g_CVar->CallGlobalChangeCallbacks(this, pszOldValue, flOldValue);
}

bool ConVar::ClampValue(float& value)
{
    if(m_bHasMin && (value < m_fMinVal)) {
        value = m_fMinVal;
        return true;
    }

    if(m_bHasMax && (value > m_fMaxVal)) {
        value = m_fMaxVal;
        return true;
    }

    return false;
}

void ConVar::InternalSetFloatValue(float fNewValue)
{
    if(fNewValue == m_Value.m_fValue)
        return;

    ClampValue(fNewValue);

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%f", m_Value.m_fValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetIntValue(int nValue)
{
    if(nValue == ((int)m_Value.m_nValue ^ (int)this))
        return;

    float fValue = (float)nValue;
    if(ClampValue(fValue)) {
        nValue = (int)(fValue);
    }

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&nValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%d", m_Value.m_nValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetColorValue(Color cValue)
{
    int color = (int)cValue.GetRawColor();
    InternalSetIntValue(color);
}

void ConVar::Create(const char *pName, const char *pDefaultValue, int flags /*= 0*/,
    const char *pHelpString /*= NULL*/, bool bMin /*= false*/, float fMin /*= 0.0*/,
    bool bMax /*= false*/, float fMax /*= false*/, FnChangeCallback_t callback /*= NULL*/)
{
    static const char *empty_string = "";

    m_pParent = this;

    // Name should be static data
    m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

    m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
    m_Value.m_pszString = new char[m_Value.m_StringLength];
    memcpy(m_Value.m_pszString, m_pszDefaultValue, m_Value.m_StringLength);

    m_bHasMin = bMin;
    m_fMinVal = fMin;
    m_bHasMax = bMax;
    m_fMaxVal = fMax;

    if(callback)
        m_fnChangeCallbacks.AddToTail(callback);

    float value = (float)atof(m_Value.m_pszString);

    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&value ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&value ^ (uint32_t)this;

    BaseClass::Create(pName, pHelpString, flags);
}

void ConVar::SetValue(const char *value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetValue(value);
}

void ConVar::SetValue(float value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetFloatValue(value);
}

void ConVar::SetValue(int value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetIntValue(value);
}

void ConVar::SetValue(Color value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetColorValue(value);
}

void ConVar::Revert(void)
{
    // Force default value again
    ConVar *var = (ConVar *)m_pParent;
    var->SetValue(var->m_pszDefaultValue);
}

bool ConVar::GetMin(float& minVal) const
{
    minVal = m_pParent->m_fMinVal;
    return m_pParent->m_bHasMin;
}

bool ConVar::GetMax(float& maxVal) const
{
    maxVal = m_pParent->m_fMaxVal;
    return m_pParent->m_bHasMax;
}

const char *ConVar::GetDefault(void) const
{
    return m_pParent->m_pszDefaultValue;
}

SpoofedConvar::SpoofedConvar(const char* szCVar)
{
	m_pOriginalCVar = g_CVar->FindVar(szCVar);
	Spoof();
}

SpoofedConvar::SpoofedConvar(ConVar* pCVar)
{
	m_pOriginalCVar = pCVar;
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		//Register it
		g_CVar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

SpoofedConvar::~SpoofedConvar()
{
	Restore();
}

bool SpoofedConvar::IsSpoofed()
{
	return m_pDummyCVar != nullptr;
}

void SpoofedConvar::Spoof()
{
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);
		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		g_CVar->RegisterConCommand(m_pDummyCVar);

		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

void SpoofedConvar::Restore()
{
	if (IsSpoofed())
	{
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		g_CVar->UnregisterConCommand(m_pDummyCVar);
		//Interfaces->Cvar->RegisterConCommand(m_pOriginalCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = nullptr;
	}
}
void SpoofedConvar::SetFlags(int flags)
{
	if (IsSpoofed()) {
		m_pOriginalCVar->m_nFlags = flags;
	}
}

int SpoofedConvar::GetFlags()
{
	return m_pOriginalCVar->m_nFlags;
}

void SpoofedConvar::SetInt(int iValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(iValue);
	}
}

void SpoofedConvar::SetBool(bool bValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(bValue);
	}
}

void SpoofedConvar::SetFloat(float flValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SpoofedConvar::SetString(const char* szValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(szValue);
	}
}
// Junk Code By Troll Face & Thaisen's Gen
void XOJXZwHILfbmWiknOsRGRmAQJkqsjdtTFosVDnBrqyHiB12399899() {     int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74603354 = -256549852;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn97866399 = -131559543;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54881901 = -754353410;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36507554 = -765938949;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74008472 = -158276018;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78188359 = -631529428;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94094762 = -662588072;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94193793 = -800229434;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36928633 = -601022843;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn19779858 = 67438983;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22707855 = -826544619;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn25030764 = 23141465;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn34675558 = -900919322;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54986345 = -632534910;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69385182 = -124073777;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49196996 = -515871995;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn23798120 = -140776803;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44044284 = 36350335;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn46913379 = -488542657;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22702576 = -812864474;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn79279504 = -9508657;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68912262 = -614423655;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn56730882 = -670101337;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81004892 = -658100325;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn53581531 = -903335326;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69550462 = -648996757;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48348376 = -735105927;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn2890261 = -323125085;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn90057587 = -990762630;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78203262 = -227505764;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn42000764 = 67194445;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4890363 = -233762560;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn88555715 = -809771217;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68902583 = -267928572;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn14797824 = -98728986;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn866501 = -495328566;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn45582310 = -301534781;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91321431 = 9965271;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn93484105 = -632576053;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87956800 = -133948832;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71840164 = -668977418;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn43199017 = -894269504;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78989842 = -283327974;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8608226 = -573834917;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn16135218 = 26718676;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4061426 = -266100647;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44722108 = -310322371;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn62520029 = -175115598;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89796958 = -896639177;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80135048 = -844843012;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65974013 = -972230908;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81233990 = -506659421;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77117539 = -188548163;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87133062 = -848015084;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn33120395 = -908996356;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn5691092 = -642126198;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn41135517 = -461458207;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn73877008 = 3746914;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn82926023 = -862603624;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4458011 = -509279262;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn29839983 = -896423502;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91204501 = -239462988;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4136206 = -809466805;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58725371 = -273517079;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77779094 = -999755463;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn17817492 = -492782059;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36475048 = -167087318;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65772975 = -532990751;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn40188521 = -433805924;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68518681 = -628745212;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn3614687 = -114337215;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn32476688 = -50742074;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50560178 = -331073612;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58956578 = -254593825;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50862412 = -43887056;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36080488 = -115239153;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89922420 = -231095682;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48122657 = 3733580;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn64869674 = -584819001;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49520106 = -537234680;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn24828354 = -238674386;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn85828346 = -459990330;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn13093302 = -426485909;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9922540 = -45919618;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn12229250 = -255274857;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn60766774 = -426146134;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn27772824 = 54785602;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn1422654 = -961756134;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn35782188 = -358932216;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9106733 = -456602789;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn59730983 = 66129641;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71705301 = -205281696;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8395409 = -127431106;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89026095 = -23296792;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58116818 = -237525331;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80635662 = -329514431;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn39062811 = 15197300;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn20264471 = 90189105;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn30829131 = -574079455;    int RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn98317726 = -256549852;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74603354 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn97866399;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn97866399 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54881901;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54881901 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36507554;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36507554 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74008472;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74008472 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78188359;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78188359 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94094762;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94094762 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94193793;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn94193793 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36928633;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36928633 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn19779858;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn19779858 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22707855;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22707855 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn25030764;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn25030764 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn34675558;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn34675558 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54986345;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn54986345 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69385182;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69385182 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49196996;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49196996 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn23798120;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn23798120 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44044284;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44044284 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn46913379;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn46913379 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22702576;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn22702576 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn79279504;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn79279504 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68912262;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68912262 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn56730882;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn56730882 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81004892;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81004892 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn53581531;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn53581531 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69550462;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn69550462 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48348376;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48348376 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn2890261;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn2890261 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn90057587;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn90057587 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78203262;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78203262 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn42000764;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn42000764 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4890363;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4890363 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn88555715;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn88555715 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68902583;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68902583 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn14797824;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn14797824 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn866501;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn866501 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn45582310;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn45582310 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91321431;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91321431 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn93484105;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn93484105 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87956800;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87956800 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71840164;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71840164 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn43199017;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn43199017 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78989842;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn78989842 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8608226;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8608226 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn16135218;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn16135218 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4061426;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4061426 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44722108;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn44722108 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn62520029;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn62520029 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89796958;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89796958 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80135048;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80135048 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65974013;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65974013 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81233990;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn81233990 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77117539;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77117539 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87133062;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn87133062 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn33120395;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn33120395 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn5691092;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn5691092 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn41135517;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn41135517 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn73877008;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn73877008 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn82926023;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn82926023 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4458011;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4458011 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn29839983;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn29839983 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91204501;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn91204501 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4136206;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn4136206 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58725371;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58725371 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77779094;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn77779094 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn17817492;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn17817492 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36475048;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36475048 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65772975;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn65772975 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn40188521;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn40188521 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68518681;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn68518681 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn3614687;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn3614687 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn32476688;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn32476688 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50560178;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50560178 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58956578;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58956578 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50862412;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn50862412 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36080488;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn36080488 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89922420;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89922420 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48122657;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn48122657 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn64869674;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn64869674 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49520106;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn49520106 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn24828354;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn24828354 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn85828346;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn85828346 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn13093302;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn13093302 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9922540;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9922540 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn12229250;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn12229250 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn60766774;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn60766774 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn27772824;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn27772824 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn1422654;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn1422654 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn35782188;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn35782188 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9106733;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn9106733 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn59730983;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn59730983 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71705301;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn71705301 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8395409;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn8395409 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89026095;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn89026095 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58116818;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn58116818 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80635662;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn80635662 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn39062811;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn39062811 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn20264471;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn20264471 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn30829131;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn30829131 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn98317726;     RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn98317726 = RIZInUGgtikOtNoFPZhmIghqOkaqaPSeSOrcRjvIn74603354;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GZwxaRyYInrUEkAptSprJxpvCKcrkcTEnFTuBlkImneomiMZeWYkLneJkBvPpHnfTnhzQFoIhgFVTAIUyDMN30410606() {     float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf57752268 = -799169500;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80443273 = -433159881;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf12395828 = -915917709;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf27643756 = -150008768;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28790039 = -544951112;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50871224 = -434934094;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20661175 = 67762271;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf52583723 = -514534200;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10618917 = -420574023;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30364358 = -288289404;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf29452207 = -165171286;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96958984 = -461651663;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99489543 = -145555746;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf4477536 = -722315390;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99845122 = -293204566;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf86572659 = -449892121;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51068377 = -137623419;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99171239 = -190086898;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf31887858 = -42198827;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf7514225 = -485804671;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf22394329 = 9733868;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf37945492 = -308551618;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf87402423 = -217879600;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58400584 = -964819770;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21238079 = -261044716;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98677194 = -837563981;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51219871 = -299398471;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28263124 = 989164;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21997752 = -156752801;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14546293 = -869177527;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80358665 = 43044978;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf68119572 = -494509901;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf75139846 = -441162021;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf18201983 = -388740595;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10543055 = -308052661;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62854110 = -842394266;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62900496 = -538025290;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96891503 = -595989250;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19142535 = -828516307;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77440187 = -366068212;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44009662 = -931894984;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf79079014 = -720540684;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf94032235 = -930863840;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43302332 = -499667835;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf82082091 = -836729656;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf40076457 = -770402480;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf32818480 = -656486478;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf1578326 = -282340527;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14262944 = -727468844;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65040244 = -694430703;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf5546502 = -194648869;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77098002 = -176489628;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98580925 = -880245450;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf16179652 = -487932299;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8016278 = -681489852;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19806776 = -390617883;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93040849 = -115280282;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf53995243 = -951097940;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf6405678 = -888964053;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30112845 = -707387131;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99651352 = -35535624;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf92398051 = -933226894;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30585971 = -257781400;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96072624 = -551396497;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50005693 = -231334382;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf61332635 = -670661386;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21819139 = 79510357;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81287560 = -756815152;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93934480 = -314262729;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf36991013 = -450810301;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf23672164 = -911866831;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54176874 = -541634169;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80028705 = -361570591;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54447671 = -676130615;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf63504562 = -553909687;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43315314 = -269725449;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43913257 = -377687778;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44100092 = -718211765;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf76318493 = -28090115;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81161621 = -490642236;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65858714 = -81077503;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf49641546 = 82942056;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14000181 = -271541992;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf56957507 = -462322098;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8999791 = -574528659;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf3260663 = -780465395;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69538647 = -614264451;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58960194 = -953229722;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10185706 = -707250744;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90736278 = -917434778;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69813260 = -627113984;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8905253 = -586927351;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90485825 = -707025198;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf89029690 = -21129176;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77788834 = -230532588;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51611610 = -998668091;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf48493043 = -362759285;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf97959611 = -279467344;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93296639 = -168333454;    float EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20749456 = -799169500;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf57752268 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80443273;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80443273 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf12395828;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf12395828 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf27643756;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf27643756 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28790039;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28790039 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50871224;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50871224 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20661175;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20661175 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf52583723;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf52583723 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10618917;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10618917 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30364358;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30364358 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf29452207;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf29452207 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96958984;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96958984 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99489543;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99489543 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf4477536;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf4477536 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99845122;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99845122 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf86572659;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf86572659 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51068377;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51068377 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99171239;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99171239 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf31887858;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf31887858 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf7514225;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf7514225 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf22394329;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf22394329 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf37945492;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf37945492 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf87402423;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf87402423 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58400584;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58400584 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21238079;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21238079 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98677194;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98677194 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51219871;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51219871 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28263124;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf28263124 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21997752;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21997752 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14546293;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14546293 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80358665;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80358665 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf68119572;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf68119572 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf75139846;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf75139846 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf18201983;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf18201983 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10543055;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10543055 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62854110;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62854110 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62900496;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf62900496 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96891503;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96891503 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19142535;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19142535 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77440187;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77440187 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44009662;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44009662 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf79079014;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf79079014 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf94032235;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf94032235 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43302332;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43302332 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf82082091;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf82082091 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf40076457;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf40076457 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf32818480;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf32818480 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf1578326;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf1578326 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14262944;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14262944 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65040244;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65040244 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf5546502;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf5546502 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77098002;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77098002 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98580925;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf98580925 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf16179652;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf16179652 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8016278;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8016278 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19806776;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf19806776 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93040849;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93040849 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf53995243;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf53995243 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf6405678;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf6405678 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30112845;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30112845 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99651352;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf99651352 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf92398051;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf92398051 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30585971;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf30585971 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96072624;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf96072624 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50005693;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf50005693 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf61332635;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf61332635 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21819139;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf21819139 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81287560;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81287560 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93934480;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93934480 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf36991013;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf36991013 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf23672164;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf23672164 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54176874;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54176874 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80028705;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf80028705 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54447671;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf54447671 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf63504562;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf63504562 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43315314;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43315314 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43913257;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf43913257 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44100092;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf44100092 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf76318493;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf76318493 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81161621;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf81161621 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65858714;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf65858714 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf49641546;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf49641546 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14000181;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf14000181 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf56957507;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf56957507 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8999791;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8999791 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf3260663;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf3260663 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69538647;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69538647 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58960194;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf58960194 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10185706;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf10185706 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90736278;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90736278 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69813260;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf69813260 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8905253;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf8905253 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90485825;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf90485825 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf89029690;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf89029690 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77788834;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf77788834 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51611610;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf51611610 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf48493043;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf48493043 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf97959611;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf97959611 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93296639;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf93296639 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20749456;     EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf20749456 = EjtzbKKApVTfzyVzAMIYiQCfiJqjsaRmuabjthYGvnSMNZVUfITlunfohOQRUbIODhkZXlGToBduxpSJnVDjHfrHunBfQGmUf57752268;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AMxLeOHikwYkUvGIJymhivbhZKomOtocgMFxuZyNoVAXPOVFhFr34459085() {     long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK5438980 = -261774321;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19034227 = -875506180;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83179883 = -840379240;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39096533 = -173197927;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK31706387 = 39794971;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4733934 = -925707272;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20843849 = -859429884;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83449116 = -436693499;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24161677 = -158616832;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24365902 = -247734228;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK8175128 = -781526262;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75464894 = -950063210;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41995340 = -499008903;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK77749972 = 91691013;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94696601 = 59733216;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK27689295 = -364498465;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50429079 = -617302767;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16038270 = 8368122;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76526141 = -688282707;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34525535 = -38196026;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92257002 = -168982111;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92705330 = -721039227;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK65987784 = -907504599;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK63828385 = -701174872;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK45098019 = -358115845;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75392794 = -191657359;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78781583 = -207806730;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK30609324 = -909607912;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK13608924 = -721683312;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK33400847 = -214057806;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16839675 = -722670620;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK85887691 = -78196705;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92232176 = -575928951;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK37894375 = -933199672;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK81658850 = -695933044;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94694969 = -464777514;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86777549 = -409646448;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86205011 = -116919533;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK32067200 = -79920981;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK88588487 = -387640890;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76655385 = -919585585;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK18442716 = 50837407;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK99832930 = -104784046;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK17481049 = -699650565;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90021532 = -460296526;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50344143 = -591802997;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK15418585 = -759304536;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK10602101 = 22580459;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75165456 = -126443537;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK35438561 = -946504026;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK70977952 = -432067776;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96750171 = -373740359;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39147336 = -111924191;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34359529 = -752300941;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK42202083 = 14057165;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK12733649 = -540735094;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK53046443 = -968001581;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19351499 = -39204368;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK93998513 = -815082082;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK56313593 = -768547671;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK25952350 = -617900542;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90234525 = -949821973;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK69840192 = -715010188;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90760830 = -944559027;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK7526228 = -525063608;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK22287437 = -603329557;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83232717 = -274134259;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4100965 = -565809232;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96091121 = -212375944;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK1632 = -475489270;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK40911746 = -954852018;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK64224067 = -400383234;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83971069 = -911710897;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK87937654 = -200641818;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57870150 = -118610442;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73814286 = -119819519;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92872399 = -516255182;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48506735 = -107854035;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73806853 = -140878346;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94753876 = -766312849;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK59974210 = -432352823;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68179483 = -130387190;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK55443867 = -683164375;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78170362 = -775179286;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62422894 = -781990030;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20089503 = -248930262;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK46740355 = -966272515;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57872647 = -823628011;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK95692291 = -847256837;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68925201 = -55197951;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41648527 = -496775934;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67426051 = -270442080;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92206497 = -301837451;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75753607 = -311373310;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62636137 = -769740348;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86420859 = -969763613;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48602524 = -234152405;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9072101 = -160225020;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9954822 = -74586957;    long vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67734096 = -261774321;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK5438980 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19034227;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19034227 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83179883;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83179883 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39096533;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39096533 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK31706387;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK31706387 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4733934;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4733934 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20843849;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20843849 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83449116;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83449116 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24161677;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24161677 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24365902;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK24365902 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK8175128;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK8175128 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75464894;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75464894 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41995340;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41995340 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK77749972;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK77749972 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94696601;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94696601 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK27689295;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK27689295 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50429079;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50429079 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16038270;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16038270 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76526141;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76526141 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34525535;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34525535 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92257002;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92257002 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92705330;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92705330 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK65987784;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK65987784 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK63828385;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK63828385 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK45098019;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK45098019 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75392794;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75392794 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78781583;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78781583 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK30609324;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK30609324 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK13608924;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK13608924 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK33400847;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK33400847 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16839675;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK16839675 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK85887691;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK85887691 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92232176;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92232176 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK37894375;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK37894375 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK81658850;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK81658850 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94694969;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94694969 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86777549;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86777549 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86205011;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86205011 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK32067200;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK32067200 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK88588487;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK88588487 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76655385;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK76655385 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK18442716;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK18442716 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK99832930;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK99832930 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK17481049;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK17481049 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90021532;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90021532 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50344143;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK50344143 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK15418585;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK15418585 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK10602101;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK10602101 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75165456;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75165456 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK35438561;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK35438561 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK70977952;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK70977952 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96750171;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96750171 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39147336;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK39147336 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34359529;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK34359529 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK42202083;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK42202083 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK12733649;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK12733649 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK53046443;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK53046443 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19351499;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK19351499 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK93998513;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK93998513 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK56313593;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK56313593 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK25952350;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK25952350 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90234525;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90234525 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK69840192;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK69840192 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90760830;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK90760830 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK7526228;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK7526228 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK22287437;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK22287437 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83232717;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83232717 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4100965;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK4100965 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96091121;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK96091121 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK1632;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK1632 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK40911746;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK40911746 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK64224067;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK64224067 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83971069;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK83971069 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK87937654;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK87937654 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57870150;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57870150 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73814286;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73814286 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92872399;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92872399 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48506735;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48506735 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73806853;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK73806853 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94753876;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK94753876 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK59974210;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK59974210 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68179483;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68179483 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK55443867;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK55443867 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78170362;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK78170362 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62422894;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62422894 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20089503;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK20089503 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK46740355;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK46740355 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57872647;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK57872647 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK95692291;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK95692291 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68925201;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK68925201 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41648527;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK41648527 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67426051;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67426051 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92206497;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK92206497 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75753607;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK75753607 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62636137;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK62636137 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86420859;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK86420859 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48602524;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK48602524 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9072101;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9072101 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9954822;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK9954822 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67734096;     vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK67734096 = vgIbEkQlUaVUqHwEHpJNkzYtMuESLoiEICtvXkANenemSBKPTK5438980;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void cRJFLyBgABiJpreXdFHiqoGaLAORWSbUBxXmWTyRRelgwARvCZSzKkZnLiuKjjJhZNHrUsAJHqQLCZeKjSOwzcJQXj28481912() {     double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75509571 = -945045174;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN11258840 = -187693090;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33389825 = -433058922;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN58095929 = -663065036;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN37217041 = -200693601;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN90882475 = -26805233;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97455930 = -910877579;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24555394 = -681538086;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76237650 = -187678714;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN8450788 = -318323820;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN34600211 = -274241673;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17019593 = -456959222;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN67435773 = -932008617;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70559271 = -894587867;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN48869412 = -296163123;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN344118 = -277170177;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN27539512 = -184069220;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN381984 = -993455355;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN22660191 = -678459848;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76090011 = -149234062;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN2837495 = -194418579;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN428520 = -243289092;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66305665 = -352689112;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17581028 = -941983093;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN68719552 = -840093014;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN98698426 = 56252072;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38543507 = -24201339;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN56568737 = -813142929;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN18451881 = -303906115;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN49457515 = -416949638;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN14317828 = -663248987;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53558931 = -509865748;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33089390 = -791011488;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17116873 = -90126465;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70183029 = -177226815;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89642793 = -992439031;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN60064999 = -614042246;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64103460 = -603106624;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN10956796 = -638712407;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5858949 = -900153439;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN31986313 = -904425802;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN39163640 = -132589254;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91325497 = -545799965;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70719835 = -675479165;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7953266 = -679636576;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN88926096 = 48545036;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24164983 = -581173158;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51916340 = -283414224;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89857069 = -907016884;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN12943337 = -34110047;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51908304 = -813840464;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97527226 = -642883250;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN45752325 = -611541163;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17951088 = 91689684;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN25644417 = -409549577;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75081052 = -601756082;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44953174 = -835003978;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15808797 = -491075829;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89376377 = -822972022;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38518614 = -156945673;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN52338969 = 97396106;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40887193 = 2265350;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN6103513 = -277631972;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26780135 = -770729076;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN94132959 = -655074834;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN81041280 = -764375925;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN83930202 = -665947735;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN50318900 = -741882153;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN376242 = -617361052;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN59226618 = -303724092;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40279118 = -663127931;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63436052 = -580962596;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89425187 = -254742949;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16801242 = -778306409;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44103698 = -244808260;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63673855 = 38170674;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9103022 = -697489128;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95585830 = -677209948;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9627762 = -162346517;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN79793455 = -788638051;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74533443 = -362574771;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN86627166 = -740787116;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66711667 = -906126046;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5508545 = -169796069;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97549211 = -603109175;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16790602 = 79634263;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7806606 = -898324586;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15138302 = -782701173;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91472456 = -680576888;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95101977 = -575470734;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44689620 = -57435053;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44256202 = -22966417;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74727083 = -780134602;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN72438181 = -381766735;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53519980 = -897549546;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91099119 = -806691152;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33060127 = -854957283;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64545363 = -775070889;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76586875 = 79595668;    double boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26911986 = -945045174;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75509571 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN11258840;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN11258840 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33389825;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33389825 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN58095929;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN58095929 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN37217041;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN37217041 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN90882475;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN90882475 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97455930;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97455930 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24555394;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24555394 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76237650;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76237650 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN8450788;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN8450788 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN34600211;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN34600211 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17019593;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17019593 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN67435773;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN67435773 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70559271;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70559271 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN48869412;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN48869412 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN344118;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN344118 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN27539512;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN27539512 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN381984;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN381984 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN22660191;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN22660191 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76090011;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76090011 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN2837495;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN2837495 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN428520;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN428520 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66305665;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66305665 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17581028;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17581028 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN68719552;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN68719552 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN98698426;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN98698426 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38543507;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38543507 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN56568737;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN56568737 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN18451881;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN18451881 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN49457515;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN49457515 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN14317828;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN14317828 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53558931;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53558931 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33089390;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33089390 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17116873;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17116873 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70183029;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70183029 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89642793;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89642793 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN60064999;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN60064999 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64103460;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64103460 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN10956796;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN10956796 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5858949;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5858949 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN31986313;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN31986313 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN39163640;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN39163640 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91325497;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91325497 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70719835;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN70719835 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7953266;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7953266 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN88926096;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN88926096 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24164983;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN24164983 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51916340;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51916340 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89857069;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89857069 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN12943337;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN12943337 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51908304;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN51908304 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97527226;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97527226 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN45752325;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN45752325 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17951088;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN17951088 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN25644417;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN25644417 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75081052;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75081052 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44953174;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44953174 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15808797;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15808797 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89376377;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89376377 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38518614;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN38518614 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN52338969;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN52338969 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40887193;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40887193 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN6103513;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN6103513 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26780135;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26780135 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN94132959;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN94132959 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN81041280;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN81041280 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN83930202;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN83930202 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN50318900;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN50318900 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN376242;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN376242 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN59226618;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN59226618 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40279118;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN40279118 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63436052;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63436052 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89425187;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN89425187 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16801242;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16801242 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44103698;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44103698 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63673855;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN63673855 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9103022;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9103022 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95585830;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95585830 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9627762;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN9627762 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN79793455;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN79793455 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74533443;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74533443 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN86627166;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN86627166 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66711667;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN66711667 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5508545;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN5508545 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97549211;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN97549211 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16790602;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN16790602 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7806606;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN7806606 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15138302;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN15138302 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91472456;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91472456 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95101977;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN95101977 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44689620;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44689620 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44256202;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN44256202 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74727083;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN74727083 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN72438181;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN72438181 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53519980;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN53519980 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91099119;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN91099119 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33060127;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN33060127 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64545363;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN64545363 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76586875;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN76586875 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26911986;     boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN26911986 = boYnhhUDHitZaKZIsUTNklDkbQUNiwErUlRMlTIiqzSwRKsEXqkpwKypkoMRMHVKeBFptN75509571;}
// Junk Finished
