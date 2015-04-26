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
#include <mutex>

using namespace std;


MasterConnection::MasterConnection(ConnectionData* connection, LinkHashMap *link_map, NodeHashMap *node_map, std::mutex *map_lock) : Connection(connection),link_map(link_map), node_map(node_map), map_lock(map_lock) {
}

void MasterConnection::handleConnection() {
	int sock = mConnectionData->socket;
	
	mSock = std::make_unique<Socket>(sock);
	
	AddressDetails request;
	if (!this->handleRequest(request)) {
		return;
	}
}

bool MasterConnection::verifyVersion(char version) {
	if (version != Constants::Relay::Version::V1) {
		cerr << "Invalid master version provided." << endl;
		return false;
	}
	return true;
}


void MasterConnection::nodeJoin(){
	AddressDetails node;
	bytes latencyStr, response;
	
	if (!readAddressInformation(node)) {
		cerr << "Could not read address information" << endl;
		sendError();
		return;
	}
	
	map_lock->lock();
	if(node_map->count(node)<=0){
		std::vector<Link*> nodeLink;
		(*node_map)[node] = nodeLink;
	}
	map_lock->unlock();
	
	char temp;
	temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Recorded;
	response +=(temp);
	mSock->send(response);
	cerr << "New relay node joined." << endl;
}

void MasterConnection::nodeConnect(){
	AddressDetails node, server;
	bytes latencyStr, response;
	uint16_t latency=UINT16_MAX;
	
	if (!readAddressInformation(node)) {
		cerr << "Could not read address information" << endl;
		sendError();
		return;
	}
	if (!readAddressInformation(server)) {
		cerr << "Could not read joining node server address information" << endl;
		sendError();
		return;
	}
	
	if (!mSock->receive(latencyStr, 2) && latencyStr.size() != 2){
		sendError();
		return;
	}
	
	// horrible latency decoding. same as port...
	unsigned char h = (unsigned char)latencyStr[0];
	unsigned char l = (unsigned char)latencyStr[1];
	latency = (h << 8) + l;
	
	Link *link = new Link(node, server, latency, true);
	
	map_lock->lock();
	if(link_map->count(link->getServer())<=0){
		std::set<Link*, LinkComparator> links;
		links.insert(link);
		(*link_map)[link->getServer()] = links;
	} else {
		(*link_map)[link->getServer()].insert(link);
	}
	
	if(node_map->count(link->getNode())<=0){
		std::vector<Link*> nodeLink;
		nodeLink.push_back(link);
		(*node_map)[link->getNode()] = nodeLink;
	} else {
		(*node_map)[link->getNode()].push_back(link);
	}
	map_lock->unlock();
	
	char temp;
	temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Recorded;
	response +=(temp);
	mSock->send(response);

	cerr << "Relay opened new connection." << endl;
}

void MasterConnection::nodeFind(){
	AddressDetails server, node;
	bytes latencyStr, response;
	bool error = false;
	
	if (!readAddressInformation(server)) {
		cerr << "Could not read address information" << endl;
		sendError();
		return;
	}
	
	
	map_lock->lock();
	if(link_map->count(server)>0){
		if((*link_map)[server].size()>0)
			node = (*(*link_map)[server].begin())->getNode();
		else
			error = true;
	} else {
		error = true; //faster, prevent clogs
	}
	map_lock->unlock();
	
	if(error){
		sendError();
		return;
	}
	
	char temp;
	temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Result;
	response +=(temp);
	
	stringstream address;
	address << server;
	response.append(address.str());
	
	mSock->send(response);

	cerr << "Returned best relay server." << endl;
}

void MasterConnection::nodeDelete(){
	AddressDetails node;
	bytes latencyStr, response;
	std::vector<Link*> nodeLinks;
	
	if (!readAddressInformation(node)) {
		cerr << "Could not read address information" << endl;
		sendError();
		return;
	}
	
	map_lock->lock();
	if(node_map->count(node)<=0){
		nodeLinks = (*node_map)[node];
	} else {
		map_lock->unlock();
		sendError();
		return;
	}
	
	for(int i=0; i<nodeLinks.size(); i++){
		Link *l = nodeLinks[i];
		if(link_map->count(l->getServer())<=0){
			(*link_map)[l->getServer()].erase(l);
		} else {
			map_lock->unlock();
			sendError();
			return;
		}
	}
	node_map->erase(node);
	map_lock->unlock();
	
	
	char temp;
	temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Recorded;
	response +=(temp);
}


bool MasterConnection::handleRequest(AddressDetails & request) {
	bytes version, method;
	uint8_t methodNum = 0;

	while (true) {

		if (!mSock->receive(version, 1) || version.size() != 1)
			return false;

		if (!this->verifyVersion(version[0])) {
			return false;
		}

		if (!mSock->receive(method, 1) && method.size() != 1)
			return false;

		//hopefully same endinaness due to TCP correction.
		methodNum = (uint8_t) method[0];

		switch (methodNum) {
			case Constants::Server::Command::Join:
				nodeJoin();
				break;
			case Constants::Server::Command::Connect:
				nodeConnect();
				break;
			case Constants::Server::Command::Find:
				nodeFind();
				break;

			default:
				cerr << "Invalid node request\n!";
				break;
		}
	}
}


void MasterConnection::sendError() {
	std::string response;
	response.clear();

	char temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Error;
	response +=(temp);
	mSock->send(response);
}

MasterConnection::~MasterConnection() {
	//delete mConnectionData;
}