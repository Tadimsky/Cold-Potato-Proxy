//
//  Link.cpp
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#include "Link.h"
#include <string>

using namespace std;

Link::Link(Node node, Node server, int latency, bool valid)
: mServer(node), mNode(server), linkLatency(latency), valid(valid) {
}

Link::~Link(){
	
}

std::string Link::getLinkID(){
	std::string s;
	s.clear();
	s.append(mServer.ip);
	s.append("|");
	s.append(mServer.port);
	return s;
}
