#ifndef __VG_CALL_H
#define __VG_CALL_H

#include <pjsua2.hpp>

#include "myaccount.h"

using namespace pj;

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

#endif
