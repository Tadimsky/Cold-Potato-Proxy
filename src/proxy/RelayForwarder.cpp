//
// Created by jonno on 4/18/15.
//

#include <sstream>
#include <Constants.h>
#include "RelayForwarder.h"

using namespace std;

RelayForwarder::RelayForwarder(const AddressDetails &relay, const AddressDetails &finalDestination) {
    mSock = std::make_shared<Socket>();

    mFinalDestination = finalDestination;
    mRelay = relay;
}

bool RelayForwarder::connect() {
    if (!mSock->connect(mRelay)) {
        return false;
    }
    if (!this->sendRequest(mFinalDestination)) {
        return false;
    }
    // read the response from the relay once it has connected

    bytes message;
    if (!mSock->receive(message, 3)) {
        return false;
    }

    {
        using namespace Constants::Relay;

        if (message[0] != Version::V1) {
            cerr << "Unknown relay version" << endl;
            return false;
        }

        switch (message[1]) {
            case Response::Granted:
                // all is good
                cerr << "Connected to destination through relay." << endl;
                break;
            case Response::HostUnreachable:
                cerr << "Cannot connect to destination through relay." << endl;
                return false;
            case Response::InvalidAddressType:
                cerr << "Relay does not support address type." << endl;
                return false;
            case Response::InvalidConnection:
                cerr << "Relay does not support connection type." << endl;
                return false;
        }
    }

    AddressDetails ad;
    if (!Util::readAddressInformation(mSock, ad)) {
        cerr << "Could not read address information" << endl;
        return false;
    }

    return true;
}

bool RelayForwarder::sendRequest(AddressDetails &dest) {
    // send request information
    // read response
    // figure out what to do based on response

    // we know we are connected
    // v1 tcp_connection address
    stringstream address;
    address << dest;
    string data = address.str();
    mSock->send(Util::hexToString("010100") + data);

    return true;
}

std::shared_ptr<Socket> RelayForwarder::getSocket() {
    return mSock;
}
