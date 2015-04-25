//
//  MasterServer.h
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#ifndef __Cold_Potato_Proxy__MasterServer__
#define __Cold_Potato_Proxy__MasterServer__

#include "ListenServer.h"

class MasterServer : public ListenServer{
public:
	MasterServer(int port) : ListenServer(port) {};
	
	virtual void processConnection(ConnectionData *data) override;
};

#endif /* defined(__Cold_Potato_Proxy__MasterServer__) */
