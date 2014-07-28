#include "stdafx.h"
#include "vgsip.h"

VGsip::VGsip()
{
	//ep.libCreate();
}

VGsip::~VGsip()
{
}

int VGsip::init() {
	ep.libCreate();

    EpConfig ep_cfg;
    ep.libInit( ep_cfg );

    TransportConfig tcfg;
    //tcfg.port = 5060;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library (worker threads etc)
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;
	return 0;
}

bool VGsip::reg_on_srv() {
	AccountConfig acfg;
	int buf_size = wcslen(extention);
	char* extention_c = new char[buf_size + 1];
	std::cout <<buf_size << "== " << pj_unicode_to_ansi(extention, buf_size + 1, extention_c, buf_size + 1);

	//acfg.idUri = s1;
	return true;
}