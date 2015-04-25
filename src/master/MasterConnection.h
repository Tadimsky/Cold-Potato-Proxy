//
//  MasterConnection.h
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#ifndef __Cold_Potato_Proxy__MasterConnection__
#define __Cold_Potato_Proxy__MasterConnection__

#include "Socket.h"
#include "Connection.h"
#include "../ConnectionData.h"


class MasterConnection : public  Connection {
private:
	
	/**
	 * Verifies that the version that the client is requesting is allowed by our proxy server.
	 */
	bool verifyVersion(char version);
	
	/**
	 * Handles a client's connection request.
	 */
	bool handleRequest(AddressDetails & request);
	
public:
	MasterConnection(ConnectionData* connection);
	void handleConnection();
	virtual ~MasterConnection();
};



#endif /* defined(__Cold_Potato_Proxy__MasterConnection__) */
