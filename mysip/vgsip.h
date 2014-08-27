//#include "stdafx.h"
#ifndef VGSIP_H
#define VGSIP_H

#include "myaccount.h"

using namespace pj;

std::string unicode_to_pj_str(const wchar_t* str);

class VGsip;

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