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
#include "Link.h"
#include "Node.h"

#include <unordered_map>
#include <queue>
#include <shared_mutex>

struct LinkComparator
{
	bool operator() (const Link &lhs, const Link &rhs) const
	{
		return lhs.linkLatency > rhs.linkLatency;
	}
};


typedef struct std::unordered_map<std::string,std::priority_queue<Link, std::vector<Link>, LinkComparator>> HashMap;
class MasterServer : public ListenServer{
	
private:

	
public:
	MasterServer(int port) : ListenServer(port) {};
	
	virtual void processConnection(ConnectionData *data) override;
	HashMap link_map;
	std::shared_timed_mutex map_lock;
};

#endif /* defined(__Cold_Potato_Proxy__MasterServer__) */
