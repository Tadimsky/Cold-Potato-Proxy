//
// Created by jonno on 4/25/15.
//

#include "MasterController.h"
#include "Constants.h"

std::shared_ptr<MasterController> MasterController::mInstance;

bool MasterController::connect(AddressDetails relay, AddressDetails master) {
    mRelayInformation = relay;
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

MasterController::MasterController() {
    mSock = std::make_shared<Socket>();
}

bool MasterController::sendJoinRequest() {
    // send the master controller the welcome message

    // need to get my information

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::Join /* + address information */)) {
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

AddressDetails MasterController::getBestRelay(const AddressDetails &destination) {
}

std::shared_ptr<MasterController> MasterController::getInstance() {
    if (!MasterController::mInstance) {
        MasterController* m = new MasterController();
        MasterController::mInstance = std::make_shared<MasterController>(*m);
    }
    return MasterController::mInstance;
}

bool MasterController::updateConnection(const AddressDetails &destination, int latency) {
    return false;
}
