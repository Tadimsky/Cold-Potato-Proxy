//
// Created by jonno on 4/18/15.
//

#ifndef COLD_POTATO_PROXY_RELAYFORWARDER_H
#define COLD_POTATO_PROXY_RELAYFORWARDER_H


#include <ConnectionData.h>
#include "../Socket.h"
#include <thread>
#include <Util.h>

class RelayForwarder {
private:
    std::shared_ptr<Socket> mSock;

    AddressDetails mRelay;
    AddressDetails mFinalDestination;

    bool sendRequest(AddressDetails& dest);


public:
    RelayForwarder(const AddressDetails& finalDestination) : RelayForwarder(Util::getBestRelay(finalDestination), finalDestination) {}

    RelayForwarder(const AddressDetails& relay, const AddressDetails& finalDestination);

    std::shared_ptr<Socket> getSocket();

    bool connect();

};


#endif //COLD_POTATO_PROXY_RELAYFORWARDER_H
