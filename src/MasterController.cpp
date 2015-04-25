//
// Created by jonno on 4/25/15.
//

#include "MasterController.h"

bool MasterController::connect(const AddressDetails &master) {
    mMasterInformation = master;

    if (!mSock->connect(mMasterInformation)) {
        return false;
    }

    // setup connection
    if (!this->sendJoinRequest()) {
        return false;
    }

    // we have now connected to the master server
    return true;
}

MasterController::MasterController(const AddressDetails &relayInformation) {
    mRelayInformation = relayInformation;
}

bool MasterController::sendJoinRequest() {
    // send the master controller the welcome message
    if (!mSock->send("")) {
        return false;
    }

    bytes response;
    if (!mSock->receive(response, 1)) {
        return false;
    }

    // verify the information
    // based on standard

    return true;
}
