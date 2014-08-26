// mysip.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
//#include <stdint.h>
#include <pjsua2.hpp>
#include <iostream>
#include "vgsip.h"
#include "vgscl.h"

using namespace pj;

VGsip mysip;

int cred_for_mpc_aster(VGsip& mysip)
{
	mysip.extention		= "102";
	mysip.user			= "102";
	mysip.pass			= "admin";
	mysip.domain		= "10.10.0.209";
	mysip.realm			= "*";

	return 0;
}

int cred_for_sip2sip_vg(VGsip& mysip)
{
	mysip.extention		= "vgulaev";
	mysip.user			= "vgulaev";
	mysip.pass			= "28061984";
	mysip.domain		= "sip2sip.info";
	mysip.realm			= "*";
	mysip.proxies		= "proxy.sipthor.net";

	return 0;
}

int cred_for_sip2sip(VGsip& mysip)
{
	mysip.extention		= "sipfortest";
	mysip.user			= "sipfortest";
	mysip.pass			= "sipfortest";
	mysip.domain		= "sip2sip.info";
	mysip.realm			= "*";
	mysip.proxies		= "proxy.sipthor.net";

	return 0;
}

int print_main_menu()
{
	std::cout << std::endl;
	std::cout << "************************************" << std::endl;
	std::cout << "* 0 - exit" << std::endl;
	std::cout << "* cl - clear screen" << std::endl;
	std::cout << "* cr - check reg state" << std::endl;
	std::cout << "* 3 - new registration" << std::endl;
	std::cout << "* 4 - make new call" << std::endl;
	std::cout << "* 5 - make test to 3333" << std::endl;
	std::cout << "************************************" << std::endl;
	return 0;
}

int input_str(std::string& val, std::string label_for_val)
{
	std::cout << label_for_val << "(type null for empty str):";
	std::cin >> val;
	if (val == "null")
	{
		val = "";
	}
	return 0;
}
int new_registration()
{
	input_str(mysip.extention, "extention");
	input_str(mysip.user, "user");
	input_str(mysip.pass, "pass");
	input_str(mysip.domain, "domain");
	input_str(mysip.realm, "realm");
	input_str(mysip.proxies, "proxies");
	mysip.reg_on_srv();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	cred_for_sip2sip(mysip);
	//cred_for_mpc_aster(mysip);

	mysip.init();
	mysip.reg_on_srv();

	//mysip.make_call("sip:kiss@sip2sip.info");

	std::string x;

	while (true)
	{
		print_main_menu();
		std::cin >> x;
		if (x == "0")
		{
			break;
		}
		if (x == "1")
		{
			mysip.make_call("sip:101@10.10.0.209");
			mysip.make_call("sip:103@10.10.0.209");
		}
		if (x == "a")
		{
			mysip.answer("sip:vgulaev@sip2sip.info");
		}
		if (x == "vg")
		{
			mysip.make_call("sip:vgulaev@sip2sip.info");
		}
		if (x == "5")
		{
			mysip.make_call("sip:3333@sip2sip.info");
		}
		if (x == "cr")
		{
			std::cout << "Reg state: "<< mysip.regIsActive() << std::endl;
		}
		if (x == "hg")
		{
			mysip.ep.hangupAllCalls();
			std::cout << "Calls size: "<< mysip.acc->calls.size() << std::endl;
		}
		if (x == "2")
		{
			std::cout << "Calls size: "<< mysip.acc->calls.size() << std::endl;
		}
		if ((x == "?") || (x == "h"))
		{
			print_main_menu();
		}
		if (x == "cl")
		{
			system("cls");
		}
		if (x == "3")
		{
			new_registration();
		}
		if (x == "4")
		{
			std::string dest;
			std::cout << "dest uri(in sip:<ext>@<realm> format exampl: sip:101@10.10.0.209):";
			std::cin >> dest;
			mysip.make_call(dest);
		}
	}
	
	std::cout << mysip.extention;

	mysip.destroy_client();
	Endpoint::instance().hangupAllCalls();

	return 0;

    /*ep.libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep.libInit( ep_cfg );

    // Create SIP transport. Error handling sample is shown
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

	std::string sipserver;
	std::string sipext;
	sipserver = "10.10.0.209";
	//sipserver = "192.168.1.103";
	//sipserver = "localhost";
	//sipserver = "127.0.0.1";
	//sipserver = "10.10.0.231";
	sipext = "102";
	
	// Configure an AccountConfig
    AccountConfig acfg;
    acfg.idUri = "sip:" + sipext + "@" + sipserver;
    acfg.regConfig.registrarUri = "sip:" + sipserver;
	//acfg.regConfig.proxyUse = 0;
	//acfg.natConfig.sipStunUse = PJSUA_STUN_USE_DISABLED;
	//std::cout << "*****888888888**********" << acfg.natConfig.sipStunUse << std::endl;
    AuthCredInfo cred("digest", "*", sipext, 0, "admin");
    acfg.sipConfig.authCreds.push_back( cred );

    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(acfg);

	pj_thread_sleep(2000);

    // Make outgoing call
    /*Call *call = new MyCall(*acc);
    acc->calls.push_back(call);
    CallOpParam prm(true);
    prm.opt.audioCount = 1;
    prm.opt.videoCount = 0;
    call->makeCall("sip:102@192.168.1.103", prm);
    //call->makeCall("sip:102@10.10.0.216", prm);
    // Hangup all calls
    pj_thread_sleep(8000);
    ep.hangupAllCalls();
    pj_thread_sleep(4000);
	//play_med = ep.audDevManager().getPlaybackDevMedia();
	//cap_med = ep.audDevManager().getCaptureDevMedia();

	std::string x;
	while (true)
	{
		std::cin >> x;
		if (x == "0") {
			break;
		}
		if (x == "1")
		{
			ep.hangupAllCalls();
			Call *call = new MyCall(*acc);
			//CallInfo ci = call->getInfo();
			acc->calls.push_back(call);
			CallOpParam prm(true);
			prm.opt.audioCount = 1;
			prm.opt.videoCount = 0;

			call->makeCall("sip:101@" + sipserver, prm);
		}
		if (x == "2")
		{
			//play_med = ep.audDevManager().getPlaybackDevMedia();
			//cap_med = ep.audDevManager().getCaptureDevMedia();
			//cap_med.startTransmit(play_med);
		}

		std::cout << x << std::endl << "Ready!" << std::endl;
	}
	
    // Delete the account. This will unregister from server
    delete acc;

	return 0;*/
}

/*#include "stdafx.h"
#include <pjsua2.hpp>
#include <iostream>
#include <memory>
#include <pj/file_access.h>

#define THIS_FILE 	"pjsua2_demo.cpp"

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
};

class MyAccount : public Account
{
public:
    std::vector<Call *> calls;
    
public:
    MyAccount()
    {}

    ~MyAccount()
    {
        std::cout << "*** Account is being deleted: No of calls="
                  << calls.size() << std::endl;
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

    virtual void onRegState(OnRegStateParam &prm)
    {
	AccountInfo ai = getInfo();
	std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
		  << prm.code << std::endl;
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
        call->answer(prm);
    }
};

void MyCall::onCallState(OnCallStateParam &prm)
{
    PJ_UNUSED_ARG(prm);

    CallInfo ci = getInfo();
    std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
              << "]" << std::endl;
    
    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        myAcc->removeCall(this);
        // Delete the call
        delete this;
    }
}

static void mainProg1(Endpoint &ep) throw(Error)
{
    // Init library
    EpConfig ep_cfg;
    ep_cfg.logConfig.level = 4;
    ep.libInit( ep_cfg );

    // Transport
    TransportConfig tcfg;
    tcfg.port = 5060;
    ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

    // Start library
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Add account
    AccountConfig acc_cfg;
    acc_cfg.idUri = "sip:112@10.10.0.209";
    acc_cfg.regConfig.registrarUri = "sip:10.10.0.209";
    acc_cfg.sipConfig.authCreds.push_back( AuthCredInfo("digest", "*",
                                                        "112", 0, "admin") );
    std::auto_ptr<MyAccount> acc(new MyAccount);
    acc->create(acc_cfg);
    
    pj_thread_sleep(2000);
    
    // Make outgoing call
    Call *call = new MyCall(*acc);
    acc->calls.push_back(call);
    CallOpParam prm(true);
    prm.opt.audioCount = 1;
    prm.opt.videoCount = 0;
    call->makeCall("sip:111@10.10.0.209", prm);
    
    // Hangup all calls
    pj_thread_sleep(8000);
    ep.hangupAllCalls();
    pj_thread_sleep(4000);
    
    // Destroy library
    std::cout << "*** PJSUA2 SHUTTING DOWN ***" << std::endl;
}

static void mainProg2() throw(Error)
{
    string json_str;
    {
	EpConfig epCfg;
	JsonDocument jDoc;

	epCfg.uaConfig.maxCalls = 61;
	epCfg.uaConfig.userAgent = "Just JSON Test";
	epCfg.uaConfig.stunServer.push_back("stun1.pjsip.org");
	epCfg.uaConfig.stunServer.push_back("stun2.pjsip.org");
	epCfg.logConfig.filename = "THE.LOG";

	jDoc.writeObject(epCfg);
	json_str = jDoc.saveString();
	std::cout << json_str << std::endl << std::endl;
    }

    {
	EpConfig epCfg;
	JsonDocument rDoc;
	string output;

	rDoc.loadString(json_str);
	rDoc.readObject(epCfg);

	JsonDocument wDoc;

	wDoc.writeObject(epCfg);
	json_str = wDoc.saveString();
	std::cout << json_str << std::endl << std::endl;

	wDoc.saveFile("jsontest.js");
    }

    {
	EpConfig epCfg;
	JsonDocument rDoc;

	rDoc.loadFile("jsontest.js");
	rDoc.readObject(epCfg);
	pj_file_delete("jsontest.js");
    }
}


static void mainProg3(Endpoint &ep) throw(Error)
{
    const char *paths[] = { "../../../../tests/pjsua/wavs/input.16.wav",
			    "../../tests/pjsua/wavs/input.16.wav",
			    "input.16.wav"};
    unsigned i;
    const char *filename = NULL;

    // Init library
    EpConfig ep_cfg;
    ep.libInit( ep_cfg );

    for (i=0; i<PJ_ARRAY_SIZE(paths); ++i) {
       if (pj_file_exists(paths[i])) {
          filename = paths[i];
          break;
       }
    }

    if (!filename) {
	PJSUA2_RAISE_ERROR3(PJ_ENOTFOUND, "mainProg3()",
			   "Could not locate input.16.wav");
    }

    // Start library
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Create player and recorder
    {
	AudioMediaPlayer amp;
	amp.createPlayer(filename);

	AudioMediaRecorder amr;
	amr.createRecorder("recorder_test_output.wav");

	amp.startTransmit(ep.audDevManager().getPlaybackDevMedia());
	amp.startTransmit(amr);

	pj_thread_sleep(5000);
    }
}


static void mainProg() throw(Error)
{
    string json_str;

    {
	JsonDocument jdoc;
	AccountConfig accCfg;

	accCfg.idUri = "\"Just Test\" <sip:test@pjsip.org>";
	accCfg.regConfig.registrarUri = "sip:pjsip.org";
	SipHeader h;
	h.hName = "X-Header";
	h.hValue = "User header";
	accCfg.regConfig.headers.push_back(h);

	accCfg.sipConfig.proxies.push_back("<sip:sip.pjsip.org;transport=tcp>");
	accCfg.sipConfig.proxies.push_back("<sip:sip.pjsip.org;transport=tls>");

	accCfg.mediaConfig.transportConfig.tlsConfig.ciphers.push_back(1);
	accCfg.mediaConfig.transportConfig.tlsConfig.ciphers.push_back(2);
	accCfg.mediaConfig.transportConfig.tlsConfig.ciphers.push_back(3);

	AuthCredInfo aci;
	aci.scheme = "digest";
	aci.username = "test";
	aci.data = "passwd";
	aci.realm = "*";
	accCfg.sipConfig.authCreds.push_back(aci);

	jdoc.writeObject(accCfg);
	json_str = jdoc.saveString();
	std::cout << "Original:" << std::endl;
	std::cout << json_str << std::endl << std::endl;
    }

    {
	JsonDocument rdoc;

	rdoc.loadString(json_str);
	AccountConfig accCfg;
	rdoc.readObject(accCfg);

	JsonDocument wdoc;
	wdoc.writeObject(accCfg);
	json_str = wdoc.saveString();

	std::cout << "Parsed:" << std::endl;
	std::cout << json_str << std::endl << std::endl;
    }
}

//int main()
int _tmain(int argc, _TCHAR* argv[])
{
    int ret = 0;
    Endpoint ep;

    try {
	ep.libCreate();

	//mainProg3(ep);
	mainProg1(ep);
	ret = PJ_SUCCESS;
    } catch (Error & err) {
	std::cout << "Exception: " << err.info() << std::endl;
	ret = 1;
    }

    try {
	ep.libDestroy();
    } catch(Error &err) {
	std::cout << "Exception: " << err.info() << std::endl;
	ret = 1;
    }

    if (ret == PJ_SUCCESS) {
	std::cout << "Success" << std::endl;
    } else {
	std::cout << "Error Found" << std::endl;
    }

    return ret;
}*/