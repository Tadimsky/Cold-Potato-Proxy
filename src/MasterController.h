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

    MasterController();
public:
    // needs to be private

    static std::shared_ptr<MasterController> mInstance;

    bool connect(AddressDetails relayInformation, AddressDetails master);

    AddressDetails getBestRelay(const AddressDetails& destination);

    bool updateConnection(const AddressDetails& destination, int latency);

    bool notifyConnection(const AddressDetails& destination);

    static std::shared_ptr<MasterController> getInstance();

    bool isConnected();

};


#endif //COLD_POTATO_PROXY_MASTERCONTROLLER_H
