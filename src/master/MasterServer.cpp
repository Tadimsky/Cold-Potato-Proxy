//
//  MasterServer.cpp
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#include "MasterServer.h"
#include "MasterConnection.h"


using namespace std;

void MasterServer::processConnection(ConnectionData *data) {
	
	if (!data)
	{
		// Invalid data.
		cerr << "NULL thread data." << endl;
		return;
	}
		
	MasterConnection connection(data, &link_map, &node_map, &map_lock);
	connection.handleConnection();
}
