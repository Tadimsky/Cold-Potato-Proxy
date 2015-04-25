//
//  Node.cpp
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#include "Node.h"

using namespace std;

Node::Node(std::string ip, std::string port)
{
	Node::ip = ip;
	Node::port = port;
}