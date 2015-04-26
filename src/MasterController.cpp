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

    socketMutex.lock();
    cerr << "Connecting to Master Server." << endl;

    if (!mSock->connect(mMasterInformation)) {
        socketMutex.unlock();
        return false;
    }

    // setup connection
    if (!this->sendJoinRequest()) {
        socketMutex.unlock();
        return false;
    }
    cerr << "Connected to Master Server." << endl;
    // we have now connected to the master server
    socketMutex.unlock();
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
        socketMutex.unlock();
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
    socketMutex.lock();
    if (!this->isConnected()) {
        socketMutex.unlock();
        return mRelayInformation;
    }

    AddressDetails destCopy = destination;
    std::stringstream s;
    s << destCopy;
    std::string msg(s.str());

    using namespace Constants::Messages;
    if (!mSock->send(Master::Version + Master::Request::FindRelay + msg)) {
        socketMutex.unlock();
        return mRelayInformation;
    }

    cerr << "Sent request information" << endl;

    bytes response;
    if (!mSock->receive(response, 2)) {
        cerr << "Did not receive relay information from server." << endl;
        socketMutex.unlock();
        return mRelayInformation;
    }
    if (response[0] != Constants::Server::Version::V1) {
        cerr << "Master Server version is incorrect." << endl;
        socketMutex.unlock();
        return mRelayInformation;
    }

    if (response[1] != Constants::Server::Response::Result) {
        cerr << "Did not receive result response for relay." << endl;
        socketMutex.unlock();
        return mRelayInformation;
    }

    AddressDetails value;
    if (!Util::readAddressInformation(mSock, value)) {
        cerr << "Could not receive realy address information from server." << endl;
        socketMutex.unlock();
        return mRelayInformation;
    }
    socketMutex.unlock();
    cerr << "Got best relay from server" << endl;
    return value;
}

std::shared_ptr<MasterController> MasterController::getInstance() {
    if (!MasterController::mInstance) {
        mInstance = std::make_shared<MasterController>();
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
    socketMutex.lock();
    cerr << "Notifying new connection." << endl;
    if (!this->isConnected()) {
        socketMutex.unlock();
        return false;
    }
    std::stringstream relayStream;
    relayStream << mRelayInformation;
    std::string relay(relayStream.str());

    std::stringstream destStream;
    AddressDetails dest(destination);
    destStream.clear();
    destStream << dest;
    std::string dst(destStream.str());

    using namespace Constants::Messages;
    bytes msg = Master::Version + Master::Request::ConnectServer + relay + dst + Util::hexToString("0000");
    if (!mSock->send(msg)) {
        cerr << "Could not send message to master server." << endl;
        socketMutex.unlock();
        return false;
    }

    bytes response;
    if (!mSock->receive(response, 2)) {
        cerr << "Could not receive message from master." << endl;
        socketMutex.unlock();
        return false;
    }
    if (response[0] != Constants::Server::Version::V1) {
        cerr << "Invalid master server version" << endl;
        socketMutex.unlock();
        return false;
    }

    if (response[1] != Constants::Server::Response::Recorded) {
        cerr << "Did not record connection on master" << endl;
        socketMutex.unlock();
        return false;
    }
    cerr << "Recorded connection on master server." << endl;
    socketMutex.unlock();
    return true;
}
