#include "stdafx.h"

#include "vgsip.h"

std::string unicode_to_pj_str(wchar_t* str)
{
	int buf_size = wcslen(str);
	char* temp_char = new char[buf_size];
	pj_unicode_to_ansi(str, buf_size, temp_char, buf_size+1);

	std::string res(temp_char);
	return res;
}

void MyCall::onCallMediaState(OnCallMediaStateParam &prm)
{
    CallInfo ci = getInfo();
    // Iterate all the call medias
    for (unsigned i = 0; i < ci.media.size(); i++) {
        if (ci.media[i].type==PJMEDIA_TYPE_AUDIO && getMedia(i)) {
            AudioMedia *aud_med = (AudioMedia *)getMedia(i);

            // Connect the call audio media to sound device
            AudDevManager& mgr = Endpoint::instance().audDevManager();
            aud_med->startTransmit(mgr.getPlaybackDevMedia());
            mgr.getCaptureDevMedia().startTransmit(*aud_med);
        }
    }
}

VGsip::VGsip()
{
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
	//std::cout <<buf_size << "== " << ;
	std::string domain_str	= unicode_to_pj_str(domain);
	std::string realm_str	= unicode_to_pj_str(realm);
	std::string user_str	= unicode_to_pj_str(user);
	std::string pass_str	= unicode_to_pj_str(pass);
	acfg.idUri = "sip:" + unicode_to_pj_str(extention) + "@" + domain_str;
	acfg.sipConfig.proxies.push_back("sip:proxy.sipthor.net;transport=udp");
	acfg.regConfig.registrarUri = "sip:" + domain_str;
	
	//acfg.regConfig.proxyUse
	AuthCredInfo cred("digest", realm_str, user_str, 0, pass_str);

	acfg.sipConfig.authCreds.push_back( cred );
	acc = new MyAccount;
	acc->create(acfg);
	
	return true;
}

bool VGsip::make_call(wchar_t* dest)
{
	Call *call = new MyCall(*acc);
	acc->calls.push_back(call);
	CallOpParam prm(true);
	prm.opt.audioCount = 1;
	prm.opt.videoCount = 0;
	//std::string domain_str	= unicode_to_pj_str(domain);

	call->makeCall(unicode_to_pj_str(dest), prm);

	return true;
}

int VGsip::demo()
{
	extention		= L"vgulaev";
	user			= L"vgulaev";
	pass			= L"28061984";
	domain			= L"sip2sip.info";
	realm			= L"*";

	init();

	reg_on_srv();

	make_call(L"sip:kiss@sip2sip.info");

	return 0;
}