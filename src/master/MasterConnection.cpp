//
//  MasterConnection.cpp
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#include "MasterConnection.h"
#include "Constants.h"
#include <iostream>
#include <sstream>

using namespace std;


MasterConnection::MasterConnection(ConnectionData* connection, HashMap *link_map, std::shared_timed_mutex *map_lock) : Connection(connection),link_map(link_map), map_lock(map_lock) {
}

void MasterConnection::handleConnection() {
//	int sock = mConnectionData->socket;
//	
//	mSock = std::make_unique<Socket>(sock);
//	
//	AddressDetails request;
//	if (!this->handleRequest(request)) {
//		return;
//
}

bool MasterConnection::verifyVersion(char version) {
//	if (version != Constants::Relay::Version::V1) {
//		cerr << "Invalid relay version." << endl;
//		return false;
//	}
	return true;
}


bool MasterConnection::handleRequest(AddressDetails & request) {
	bytes header;
	
	if (!mSock->receive(header, 3))
	{
		return false;
	}
	
	if (header.size() != 3)
	{
		return false;
	}
	
	if (!this->verifyVersion(header[0])) {
		return false;
	}
	
	// We only support TCP CONNECT, so fail other commands.
	if (header[1] != Constants::Relay::Command::TCPConnection)
	{
		cerr << "Unsupported command: " << hex << header[1] << endl;
		// use this namespace for easier to read messages.
		using namespace Constants::Messages::Relay::Request::Response;
		// TODO: Process error messages better. + anything else??
		mSock->send(InvalidConnection +  Blank);
		
		return false;
	}
	
	return this->readAddressInformation(request);
}

MasterConnection::~MasterConnection() {
	//delete mConnectionData;
}