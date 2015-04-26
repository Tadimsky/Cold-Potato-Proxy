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

    cerr << "Connecting to Master Server." << endl;

    if (!mSock->connect(mMasterInformation)) {
        return false;
    }

    // setup connection
    if (!this->sendJoinRequest()) {
        return false;
    }
    cerr << "Connected to Master Server." << endl;
    // we have now connected to the master server
    return true;
}

MasterController::MasterController() {
    mSock = std::make_shared<Socket>();
}

bool MasterController::sendJoinRequest() {
    if (!this->isConnected()) {
        return false;
    }
    // send the master controller the welcome message

    // need to get my information

    std::stringstream s;
    s << mRelayInformation;
    std::string msg(s.str());

    using namespace Constants::Messages;
    string f = Master::Version + Master::Request::Join + msg;
    cerr << Util::stringToHex(f);
    if (!mSock->send(f)) {
        return false;
    }

    bytes response;
    if (!mSock->receive(response, 2)) {
        cerr << "Did not receive master reply." << endl;
        return false;
    }
    if (response[0] != Constants::Server::Version::V1) {
        cerr << "Unsupported Master Server Version" << endl;
        return false;
    }

    if (response[1] != Constants::Server::Response::Recorded) {
        cerr << "Did not record update on master" << endl;
        return false;
    }
    return true;
}

AddressDetails MasterController::getBestRelay(const AddressDetails &destination) {
    cerr << "Getting Best Relay" << endl;
    if (!this->isConnected()) {
        return mRelayInformation;
    }

    AddressDetails destCopy = destination;
    std::stringstream s;
    s << destCopy;
    std::string msg(s.str());

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::FindRelay + msg)) {
        return mRelayInformation;
    }

    cerr << "Sent request information" << endl;

    bytes response;
    if (!mSock->receive(response, 2)) {
        return mRelayInformation;
    }
    if (response[0] != Constants::Server::Version::V1) {
        return mRelayInformation;
    }

    if (response[1] != Constants::Server::Response::Result) {
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

bool MasterController::notifyConnection(const AddressDetails &destination) {
    cerr << "Notifying new connection." << endl;
    if (!this->isConnected()) {
        return false;
    }
    std::stringstream s;
    s << mRelayInformation;
    std::string relay(s.str());

    AddressDetails dest(destination);
    s.clear();
    s << dest;
    std::string dst(s.str());

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::ConnectServer + relay + dst + Util::hexToString("00"))) {
        return false;
    }

    bytes response;
    if (!mSock->receive(response, 2)) {
        return false;
    }
    if (response[0] != Constants::Server::Version::V1) {
        return false;
    }

    if (response[1] != Constants::Server::Response::Recorded) {
        cerr << "Did not record connection on master" << endl;
        return false;
    }
    return true;
}
