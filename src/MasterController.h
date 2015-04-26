//
// Created by jonno on 4/25/15.
//

#ifndef COLD_POTATO_PROXY_MASTERCONTROLLER_H
#define COLD_POTATO_PROXY_MASTERCONTROLLER_H


#include <memory>
#include "Socket.h"
#include "Util.h"

class MasterController {
private:
    std::shared_ptr<Socket> mSock;

    AddressDetails mMasterInformation;
    AddressDetails mRelayInformation;

    bool sendJoinRequest();



public:
    MasterController(const AddressDetails& relayInformation);

    bool connect(const AddressDetails& master);

    AddressDetails getBestRelay(const AddressDetails& destination);

    bool updateConnection(const AddressDetails& destination, int latency);
};


#endif //COLD_POTATO_PROXY_MASTERCONTROLLER_H
