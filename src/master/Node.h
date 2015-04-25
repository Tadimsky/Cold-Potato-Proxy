//
//  Node.h
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#ifndef __Cold_Potato_Proxy__Node__
#define __Cold_Potato_Proxy__Node__
#include <string>


class Node {
	
public:
	Node(std::string ip, std::string port);
	std::string ip;
	std::string port;
};



#endif /* defined(__Cold_Potato_Proxy__Node__) */
