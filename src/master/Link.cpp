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

Link::Link(AddressDetails node, AddressDetails server, uint16_t latency, bool valid)
: node(node), server(server), linkLatency(latency), valid(valid) {
}

Link::~Link(){
	
}

AddressDetails Link::getServer(){
	return server;
}

std::string Link::getServerStr(){
	std::string s;
	s.clear();
	s.append(server.address);
	s.append("|");
	s.append(std::to_string(server.port));
	return s;
}


const uint16_t Link::getLatency() const{
	return linkLatency;
}

