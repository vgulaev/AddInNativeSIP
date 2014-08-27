#include "stdafx.h"

#include <iostream>
#include <pjsua2.hpp>

#include "call.h"

void MyCall::onCallState(OnCallStateParam &prm)
{
	PJ_UNUSED_ARG(prm);

	CallInfo ci = getInfo();

	std::cout << "*** Call: " << ci.remoteUri << " [" << ci.stateText
		<< "]" << std::endl;

	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
		myAcc->removeCall(this);
		// Delete the call
		delete this;
	}
}

void MyCall::onCallMediaState(OnCallMediaStateParam &prm)
{
	CallInfo ci = getInfo();
	// Iterate all the call medias
	for (unsigned i = 0; i < ci.media.size(); i++) {
		if (ci.media[i].type == PJMEDIA_TYPE_AUDIO && getMedia(i)) {
			AudioMedia *aud_med = (AudioMedia *)getMedia(i);

			// Connect the call audio media to sound device
			AudDevManager& mgr = Endpoint::instance().audDevManager();
			aud_med->startTransmit(mgr.getPlaybackDevMedia());
			mgr.getCaptureDevMedia().startTransmit(*aud_med);
		}
	}
}