#include "stdafx.h"

#include <iostream>
#include <pjsua2.hpp>

#include "call.h"

#include "myaccount.h"

class VGSip {
public:
	virtual void onIncomingCall();
};

MyAccount::MyAccount()
{
	//onIncomingCallExternalEvent = NULL;
}

void MyAccount::onRegState(OnRegStateParam &prm) {
	AccountInfo ai = getInfo();
	std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
		<< " code=" << prm.code << std::endl;
}

void MyAccount::removeCall(Call *call)
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
void MyAccount::onIncomingCall(OnIncomingCallParam &iprm)
{
	Call *call = new MyCall(*this, iprm.callId);
	CallInfo ci = call->getInfo();
	CallOpParam prm;

	std::cout << "*** Incoming Call: " << ci.remoteUri << " ["
		<< ci.stateText << "]" << std::endl;

	calls.push_back(call);
	prm.statusCode = (pjsip_status_code)180;
	prm.opt.audioCount = 1;
	call->answer(prm);

	VGSip* mc = (VGSip*) ownerClass;
	mc->onIncomingCall();
	//mySip->onIncomingCall();
}
