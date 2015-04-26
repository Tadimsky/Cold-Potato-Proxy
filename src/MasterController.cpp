//
// Created by jonno on 4/25/15.
//

#include <sstream>
#include "MasterController.h"
#include "Constants.h"

using namespace std;

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

    std::stringstream s;
    s << mRelayInformation;
    std::string msg(s.str());

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::Join + msg)) {
        return false;
    }

    bytes response;
    if (!mSock->receive(response, 2)) {
        return false;
    }
    if (response[0] != 0x01) {
        return false;
    }

    if (response[1] != 0x0F) {
        cerr << "Did not record update on master" << endl;
        return false;
    }

    // connected!

    return true;
}

AddressDetails MasterController::getBestRelay(const AddressDetails &destination) {
    std::stringstream s;
    s << destination;
    std::string msg(s.str());

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::FindRelay + msg)) {
        return mRelayInformation;
    }

    bytes response;
    if (!mSock->receive(response, 2)) {
        return mRelayInformation;
    }
    if (response[0] != 0x01) {
        return mRelayInformation;
    }

    if (response[1] != 0x0F) {
        return mRelayInformation;
    }

    AddressDetails value;
    if (!Util::readAddressInformation(mSock, value)) {
        return mRelayInformation;
    }
    return value;
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

bool MasterController::isConnected() {
    return mSock->isConnected();
}