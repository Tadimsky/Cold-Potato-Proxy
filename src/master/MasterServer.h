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
#include <set>
#include <queue>

struct LinkComparator
{
	bool operator() (const Link &lhs, const Link &rhs) const
	{
		return lhs.linkLatency > rhs.linkLatency;
	}
};

class MasterServer : public ListenServer{
	
private:
	std::unordered_map<std::string,std::priority_queue<Link, std::vector<Link>, LinkComparator>> link_map;
	std::vector<Node> nodes;

	
public:
	MasterServer(int port) : ListenServer(port) {};
	
	virtual void processConnection(ConnectionData *data) override;
};

#endif /* defined(__Cold_Potato_Proxy__MasterServer__) */
