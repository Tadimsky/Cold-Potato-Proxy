//
//  Link.h
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#ifndef __Cold_Potato_Proxy__Link__
#define __Cold_Potato_Proxy__Link__

#include <string>
#include "ConnectionData.h"

class Link {
private:
	 AddressDetails server, node;
	uint16_t linkLatency;

	
public:
	Link(AddressDetails node, AddressDetails server, uint16_t latency, bool valid);
	virtual ~Link();
	AddressDetails getServer();
	AddressDetails getNode();
	const uint16_t getLatency() const;
	std::string getServerStr();
	bool valid = false;
	
};

#endif /* defined(__Cold_Potato_Proxy__Link__) */
