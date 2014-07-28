#include "stdafx.h"
#include <pjsua2.hpp>
#include <pj/unicode.h>
#include <iostream>

using namespace pj;

class MyAccount;

class MyCall : public Call
{
private:
    MyAccount *myAcc;

public:
    MyCall(Account &acc, int call_id = PJSUA_INVALID_ID)
    : Call(acc, call_id)
    {
        myAcc = (MyAccount *)&acc;
    }
    
    virtual void onCallState(OnCallStateParam &prm);
	virtual void onCallMediaState(OnCallMediaStateParam &prm);
};

// Subclass to extend the Account and get notifications etc.
class MyAccount : public Account {
public:
    std::vector<Call *> calls;

public:
    virtual void onRegState(OnRegStateParam &prm) {
        AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive? "*** Register:" : "*** Unregister:")
                  << " code=" << prm.code << std::endl;
    }
    void removeCall(Call *call)
    {
        for (std::vector<Call *>::iterator it = calls.begin();
             it != calls.end(); ++it)
        {
            if (*it == call) {
                calls.erase(it);
                break;
            }
        }
    }
    virtual void onIncomingCall(OnIncomingCallParam &iprm)
    {
        Call *call = new MyCall(*this, iprm.callId);
        CallInfo ci = call->getInfo();
        CallOpParam prm;

        std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                  << ci.stateText << "]" << std::endl;
        
        calls.push_back(call);
        prm.statusCode = (pjsip_status_code)200;
		prm.opt.audioCount = 1;
        call->answer(prm);
    }
};

class VGsip
{
public:
	VGsip();
	~VGsip();
	
	//for registration

	wchar_t* extention;
	wchar_t* domain;
	wchar_t* realm;
	wchar_t* user;
	wchar_t* pass;

	Endpoint ep;
	MyAccount *acc;

	int init();
	bool reg_on_srv();
	bool make_call(wchar_t* dest);
private:

	
};