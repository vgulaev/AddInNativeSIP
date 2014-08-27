#ifndef MYACCOUNT_H
#define MYACCOUNT_H

//#include "vgsip.h"

using namespace pj;

class MyAccount : public Account {
public:
	MyAccount();
	//pointer to main class
	int* ownerClass;
	std::vector<Call *> calls;
public:
	virtual void onRegState(OnRegStateParam &prm);
	void removeCall(Call *call);
	virtual void onIncomingCall(OnIncomingCallParam &iprm);
};

#endif