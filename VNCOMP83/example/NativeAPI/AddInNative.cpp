
#include "stdafx.h"


#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include <pjsua2.hpp>
#include <iostream>

//own headers
#include "vgsip.h"
//#include "vgsip.cpp"
#include "AddInNative.h"

#define TIME_LEN 34

#define BASE_ERRNO     7

static wchar_t *g_PropNames[] = {L"IsEnabled", L"IsTimerPresent", L"Version", L"Extention",
        L"Domain", L"Realm", L"User", L"Pass", L"Proxies"};
static wchar_t *g_MethodNames[] = {L"Enable", L"Disable", L"ShowInStatusLine", 
        L"StartTimer", L"StopTimer", L"LoadPicture", L"ShowMessageBox", L"Init", L"Reg", L"MakeCall",
		L"setNullDev", L"RegisterComplete"};

static wchar_t *g_PropNamesRu[] = {L"�������", L"����������", L"������", L"��������������", L"�����", L"����",
        L"������������", L"������", L"������������"};
static wchar_t *g_MethodNamesRu[] = {L"��������", L"���������", L"����������������������", 
        L"�����������", L"����������", L"�����������������", L"�����������������", L"����������������",
		L"������������������", L"���������", L"����������������������������NULL", L"���������������"};

static const wchar_t g_kClassNames[] = L"CAddInNative"; //"|OtherClass1|OtherClass2";
static IAddInDefBase *pAsyncEvent = NULL;

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative;
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
    if(!*pIntf)
        return -1;

    delete *pIntf;
    *pIntf = 0;
    return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    static WCHAR_T* names = 0;
    if (!names)
        ::convToShortWchar(&names, g_kClassNames);
    return names;
}
//---------------------------------------------------------------------------//
#ifndef __linux__
VOID CALLBACK MyTimerProc(
        HWND hwnd, // handle of window for timer messages
        UINT uMsg, // WM_TIMER message
        UINT idEvent, // timer identifier
        DWORD dwTime // current system time
);
#else
static void MyTimerProc(int sig);
#endif //__linux__

// CAddInNative
//---------------------------------------------------------------------------//
CAddInNative::CAddInNative()
{
    m_iMemory = 0;
    m_iConnect = 0;
	domain = "";
	realm = "";
	user = "";
	pass = "";
	proxies = "";
	//m_Version = L"v0.001 status:beta";
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{ 
    m_iConnect = (IAddInDefBase*)pConnection;
	m_Version = "v0.003 status:alpha";
	extention = "dfsdsdsv";
    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    // Component should put supported component technology version 
    // This component supports 2.0 version
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
	destroy_client();
	IAddInDefBaseEx* cnn = (IAddInDefBaseEx*)m_iConnect;
	IMsgBox* imsgbox = (IMsgBox*)cnn->GetInterface(eIMsgBox);
	imsgbox->Alert(L"�������!!!");
}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    wchar_t *wsExtension = L"AddInNativeSIPClient";
    int iActualSize = ::wcslen(wsExtension) + 1;
    WCHAR_T* dest = 0;

    if (m_iMemory)
    {
        if(m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
        return true;
    }

    return false; 
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = 0;

    ::convFromShortWchar(&propName, wsPropName);
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsPropName = NULL;
    int iActualSize = 0;

    switch(lPropAlias)
    {
    case 0: // First language
        wsCurrentName = g_PropNames[lPropNum];
        break;
    case 1: // Second language
        wsCurrentName = g_PropNamesRu[lPropNum];
        break;
    default:
        return 0;
    }
    
    iActualSize = wcslen(wsCurrentName)+1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
    }

    return wsPropName;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    WCHAR_T* names = NULL;
	switch(lPropNum)
    {
    case ePropIsEnabled:
        TV_VT(pvarPropVal) = VTYPE_BOOL;
        TV_BOOL(pvarPropVal) = m_boolEnabled;
        break;
    case ePropIsTimerPresent:
        TV_VT(pvarPropVal) = VTYPE_BOOL;
        TV_BOOL(pvarPropVal) = true;
        break;
	case ePropVersion:
		PutStrParam(pvarPropVal, m_Version);
		//PutStrParam(pvarPropVal, "sddsds");
		break;
    case ePropExtention:
        PutStrParam(pvarPropVal, extention);
		break;
    case ePropDomain:
        PutStrParam(pvarPropVal, domain);
		break;
    case ePropRealm:
        PutStrParam(pvarPropVal, realm);
		break;
    case ePropUser:
        PutStrParam(pvarPropVal, user);
		break;
    case ePropPass:
        PutStrParam(pvarPropVal, pass);
		break;
    case ePropProxies:
        PutStrParam(pvarPropVal, proxies);
		break;
    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
    
	switch(lPropNum)
    { 
    case ePropIsEnabled:
        if (TV_VT(varPropVal) != VTYPE_BOOL)
            return false;
        m_boolEnabled = TV_BOOL(varPropVal);
        break;
    case ePropExtention:
		{
		if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
		extention = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
		//m_sip_client.extention = unicode_to_pj_str(m_extention.c_str());
		}
        break;
    case ePropUser:
        {
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        user = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
        //m_sip_client.user = unicode_to_pj_str(m_user.c_str());
        }
        break;
    case ePropPass:
        {
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        pass = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
        //m_sip_client.pass = unicode_to_pj_str(m_pass.c_str());
        }
        break;
    case ePropRealm:
        {
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        realm = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
        //m_sip_client.realm = unicode_to_pj_str(m_realm.c_str());
        }
        break;
    case ePropDomain:
        {
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        domain = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
        //m_sip_client.domain = unicode_to_pj_str(m_domain.c_str());
        }
        break;
    case ePropProxies:
        {
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        proxies = unicode_to_pj_str(VariantToWStr(varPropVal).c_str());
        //m_sip_client.proxies = unicode_to_pj_str(m_proxies.c_str());
        }
        break;
    case ePropIsTimerPresent:
    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
    switch(lPropNum)
    { 
    case ePropIsEnabled:
    case ePropIsTimerPresent:
	case ePropVersion:
    case ePropExtention:
    case ePropDomain:
    case ePropRealm:
    case ePropUser:
    case ePropPass:
    case ePropProxies:
        return true;
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
    switch(lPropNum)
    { 
    case ePropIsEnabled:
    case ePropExtention:
    case ePropDomain:
    case ePropRealm:
    case ePropUser:
    case ePropPass:
    case ePropProxies:
        return true;
    case ePropIsTimerPresent:
        return false;
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    return eMethLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{ 
    if (lMethodNum >= eMethLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsMethodName = NULL;
    int iActualSize = 0;

    switch(lMethodAlias)
    {
    case 0: // First language
        wsCurrentName = g_MethodNames[lMethodNum];
        break;
    case 1: // Second language
        wsCurrentName = g_MethodNamesRu[lMethodNum];
        break;
    default: 
        return 0;
    }

    iActualSize = wcslen(wsCurrentName)+1;

    if (m_iMemory && wsCurrentName)
    {
        if(m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
    case eMethShowInStatusLine:
	case eMethMakeCall:
        return 1;
    case eMethLoadPicture:
        return 1;
    default:
        return 0;
    }
    
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                        tVariant *pvarParamDefValue)
{ 
    TV_VT(pvarParamDefValue)= VTYPE_EMPTY;

    switch(lMethodNum)
    { 
    case eMethEnable:
    case eMethDisable:
    case eMethShowInStatusLine:
    case eMethStartTimer:
    case eMethStopTimer:
    case eMethShowMsgBox:
	case eMethInit:
	case eMethReg:
	case eMethMakeCall:
	case eMethsetNullDev:
	case eMethRegisterComplete:
        // There are no parameter values by default 
        break;
    default:
        return false;
    }

    return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
    case eMethLoadPicture:
	case eMethRegisterComplete:
        return true;
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    switch(lMethodNum)
    { 
    case eMethEnable:
        m_boolEnabled = true;
        break;
    case eMethDisable:
        m_boolEnabled = false;
		//_tmain(0, NULL);
        break;
    case eMethShowInStatusLine:
        if (m_iConnect && lSizeArray)
        {
            tVariant *var = paParams;
            m_iConnect->SetStatusLine(var->pwstrVal);
#ifndef __linux__
            Sleep(5000);
#else
            sleep(5);
#endif
        }
        break;
    case eMethStartTimer:
        pAsyncEvent = m_iConnect;
        /* The task of length of turn of messages
        if (m_iConnect)
            m_iConnect->SetEventBufferDepth(4000);
        */
#ifndef __linux__
        m_uiTimer = ::SetTimer(NULL,0,100,(TIMERPROC)MyTimerProc);
#else
        struct sigaction sa;
        struct itimerval tv;
        memset(&tv, 0, sizeof(tv));

        sa.sa_handler = MyTimerProc;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART; 
        sigaction(SIGALRM, &sa, NULL);
        tv.it_interval.tv_sec = 1;
        tv.it_value.tv_sec = 1;
        setitimer(ITIMER_REAL, &tv, NULL);
#endif
        break;
    case eMethStopTimer:
#ifndef __linux__
        if (m_uiTimer != 0)
            ::KillTimer(NULL,m_uiTimer);
#else
        alarm(0);
#endif
        m_uiTimer = 0;
        pAsyncEvent = NULL;
        break;
    case eMethShowMsgBox:
        {
            if(eAppCapabilities1 <= g_capabilities)
            {
                IAddInDefBaseEx* cnn = (IAddInDefBaseEx*)m_iConnect;
                IMsgBox* imsgbox = (IMsgBox*)cnn->GetInterface(eIMsgBox);
                if (imsgbox)
                {
                    IPlatformInfo* info = (IPlatformInfo*)cnn->GetInterface(eIPlatformInfo);
                    assert(info);
                    const IPlatformInfo::AppInfo* plt = info->GetPlatformInfo();
                    if (!plt)
                        break;
                    tVariant retVal;
                    tVarInit(&retVal);
                    if(imsgbox->Confirm(plt->AppVersion, &retVal))
                    {
                        bool succeed = TV_BOOL(&retVal);
                        if (succeed)
						{
							imsgbox->Alert(L"OKFFFFF");
						}
                        else
                            imsgbox->Alert(L"Cancel");
                    }
                }
            }
        }
        break;
	case eMethInit:
		{
			init();
		}
		break;
	case eMethReg:
		{
			reg_on_srv();
		}
		break;
	case eMethMakeCall:
		{
			make_call(unicode_to_pj_str(paParams->pwstrVal));
		}
		break;
	case eMethsetNullDev:
		{
			setNullDev();
		}
		break;
    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    bool ret = false;
    FILE *file = 0;
    char *name = 0;
    int size = 0;
    char *mbstr = 0;
    wchar_t* wsTmp = 0;

    switch(lMethodNum)
    {
    case eMethLoadPicture:
        /*{
            if (!lSizeArray || !paParams)
                return false;
            
            switch(TV_VT(paParams))
            {
            case VTYPE_PSTR:
                name = paParams->pstrVal;
                break;
            case VTYPE_PWSTR:
                ::convFromShortWchar(&wsTmp, TV_WSTR(paParams));
                size = wcstombs(0, wsTmp, 0)+1;
                mbstr = new char[size];
                memset(mbstr, 0, size);
                size = wcstombs(mbstr, wsTmp, getLenShortWcharStr(TV_WSTR(paParams)));
                name = mbstr;
                break;
            default:
                return false;
            }
        }
                
        file = fopen(name, "rb");

        if (file == 0)
        {
            wchar_t* wsMsgBuf;
            uint32_t err = errno;
            name = strerror(err);
            size = mbstowcs(0, name, 0) + 1;
            wsMsgBuf = new wchar_t[size];
            memset(wsMsgBuf, 0, size * sizeof(wchar_t));
            size = mbstowcs(wsMsgBuf, name, size);

            addError(ADDIN_E_VERY_IMPORTANT, L"AddInNative", wsMsgBuf, RESULT_FROM_ERRNO(err));
            delete[] wsMsgBuf;
            return false;
        }

        fseek(file, 0, SEEK_END);
        size = ftell(file);
        
        if (size && m_iMemory->AllocMemory((void**)&pvarRetValue->pstrVal, size))
        {
            fseek(file, 0, SEEK_SET);
            size = fread(pvarRetValue->pstrVal, 1, size, file);
            pvarRetValue->strLen = size;
            TV_VT(pvarRetValue) = VTYPE_BLOB;
            
            ret = true;
        }
        if (file)
            fclose(file);

        if (mbstr)
            delete[] mbstr;*/

        break;
	case eMethRegisterComplete:
		{
			/*pAsyncEvent = m_iConnect;
			if (!pAsyncEvent){}
			else
			{
				pAsyncEvent->ExternalEvent(L"Hello", L"Hello", L"Hello");
			}*/
			TV_VT(pvarRetValue) = VTYPE_BOOL;
			pvarRetValue->bVal = regIsActive();
			ret = true;
		}
		break;
    }
    return ret; 
}
//---------------------------------------------------------------------------//
// This code will work only on the client!
#ifndef __linux__
VOID CALLBACK MyTimerProc(
HWND hwnd,    // handle of window for timer messages
UINT uMsg,    // WM_TIMER message
UINT idEvent, // timer identifier
DWORD dwTime  // current system time
)
{
    if (!pAsyncEvent)
        return;

    wchar_t *who = L"ComponentNative", *what = L"Timer";

    wchar_t *wstime = new wchar_t[TIME_LEN];
    if (wstime)
    {
        wmemset(wstime, 0, TIME_LEN);
        ::_ultow(dwTime, wstime, 10);
        pAsyncEvent->ExternalEvent(who, what, wstime);
        delete[] wstime;
    }
}
#else
void MyTimerProc(int sig)
{
    if (!pAsyncEvent)
        return;

    WCHAR_T *who = 0, *what = 0, *wdata = 0;
    wchar_t *data = 0;
    time_t dwTime = time(NULL);

    data = new wchar_t[TIME_LEN];
    
    if (data)
    {
        wmemset(data, 0, TIME_LEN);
        swprintf(data, TIME_LEN, L"%ul", dwTime);
        ::convToShortWchar(&who, L"ComponentNative");
        ::convToShortWchar(&what, L"Timer");
        ::convToShortWchar(&wdata, data);

        pAsyncEvent->ExternalEvent(who, what, wdata);

        delete[] who;
        delete[] what;
        delete[] wdata;
        delete[] data;
    }
}
#endif
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
    _wsetlocale(LC_ALL, loc);
#else
    //We convert in char* char_locale
    //also we establish locale
    //setlocale(LC_ALL, char_locale);
#endif
}
/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
void CAddInNative::addError(uint32_t wcode, const wchar_t* source, 
                        const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T *err = 0;
        WCHAR_T *descr = 0;
        
        ::convToShortWchar(&err, source);
        ::convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);
        delete[] err;
        delete[] descr;
    }
}
//---------------------------------------------------------------------------//
long CAddInNative::findName(wchar_t* names[], const wchar_t* name, 
                        const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//
bool CAddInNative::PutStrParam(tVariant* pvarPropVal,const std::string& param)
{
	bool bRes = true;

	WCHAR_T *wsPropValue = 0;
        
    TV_VT(pvarPropVal) = VTYPE_PWSTR;
    
    std::wstring wTemp(param.begin(),param.end());
 
	int iActualSize = wTemp.length() + 1;
    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropValue, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropValue, wTemp.c_str(), iActualSize);
    }
    TV_WSTR(pvarPropVal) = wsPropValue;
    pvarPropVal->wstrLen = iActualSize-1;
   
	return bRes;
}
std::wstring CAddInNative::VariantToWStr(tVariant* pvarPropVal)
{
	WCHAR_T* name = NULL;

	::convFromShortWchar(&name, TV_WSTR(pvarPropVal));

	std::wstring res(name);

	delete[] name;

	return res;
}

void CAddInNative::onIncomingCall(std::string dest)
{
	pAsyncEvent = m_iConnect;
	if (!pAsyncEvent){}
	else
	{
		wchar_t *who = L"VGSip", *what = L"IncomingCall";
		pAsyncEvent->ExternalEvent(who, what, L"Hello");
	}
}