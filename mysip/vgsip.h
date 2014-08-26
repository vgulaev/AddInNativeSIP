//#include "stdafx.h"
#ifndef VGSIP_H
#define VGSIP_H

//#include <iostream>

using namespace pj;

std::string unicode_to_pj_str(const wchar_t* str);

class VGsip;
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
	MyAccount();
	VGsip *mySip;
	std::vector<Call *> calls;
	void (*onIncomingCallExternalEvent)(); 
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
        //prm.statusCode = (pjsip_status_code)200;
		prm.statusCode = (pjsip_status_code)180;
		prm.opt.audioCount = 1;
        //call->
		call->answer(prm);
		
		
		if (onIncomingCallExternalEvent != NULL)
		{
			(*onIncomingCallExternalEvent)();
		}
    }
};

class VGsip
{
public:
	VGsip();
	~VGsip();
	
	//for registration
	std::string extention;
	std::string domain;
	std::string realm;
	std::string user;
	std::string pass;
	std::string proxies;

	Endpoint ep;
	MyAccount *acc;

	int init();
	bool reg_on_srv();
	bool make_call(std::string dest);
	int demo();
	int destroy_client();
	int setNullDev();
	bool regIsActive();
	bool answer(std::string dest);
	
	virtual void onIncomingCall();
private:
};

#endif