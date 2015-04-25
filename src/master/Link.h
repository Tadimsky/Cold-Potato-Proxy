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
#include "Node.h"

class Link {
private:
	Node mServer, mNode;
	
public:
	Link(Node node, Node server, int latency, bool valid);
	virtual ~Link();
	std::string getLinkID();
	int linkLatency;
	bool valid = false;
};

#endif /* defined(__Cold_Potato_Proxy__Link__) */
