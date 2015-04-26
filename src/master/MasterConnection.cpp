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
		cerr << "Invalid relay version." << endl;
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
	

	if(node_map->count(node)<=0){
		std::vector<Link*> nodeLink;
		(*node_map)[node] = nodeLink;
	}
	
	char temp;
	temp = Constants::Server::Version::V1;
	response.clear();
	response +=(temp);
	temp = Constants::Server::Response::Recorded;
	response +=(temp);
	mSock->send(response);
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
	
	Link *link = new Link::Link(node, server, latency, true);
	
	map_lock->lock();
	if(link_map->count(link->getServer())<=0){
		std::priority_queue<Link*, std::vector<Link*>, LinkComparator> links;
		links.push(link);
		(*link_map)[link->getServer()] = links;
	} else {
		(*link_map)[link->getServer()].push(link);
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
}

void MasterConnection::nodeUpdate(){
//	
//	std::string response;
//	response.clear();
//	
//	vector<string> tokenList = Util::split(request, '|');
//	if(tokenList.size()<3){
//		cerr<<"Join with insufficient parameters!\n";
//	}
//	
//	Node node =  Node::Node(tokenList[1], tokenList[2]);
//	int loops = (int)tokenList.size()/3;
//	if(loops<=0){
//		nodes.push_back(node);
//		return;
//	}
//	
//	for(int i=1; i<loops; i++){
//		Node server = Node::Node(tokenList[i*3], tokenList[i*3+1]);
//		int latency = std::stoi( tokenList[i*3+2]);
//		Link link = Link::Link(node, server, latency, true);
//		if(link_map.count(link.getLinkID())<=0){
//			std::priority_queue<Link, std::vector<Link>, LinkComparator> links;
//			links.push(link);
//			link_map[link.getLinkID()] = links;
//			nodes.push_back(node);
//		} else {
//			link_map[link.getLinkID()].push(link);
//		}
//	}
//	
//	
//	response.append(std::to_string(Constants::PMessages::OK));
//	sock->send(response);
}

bool MasterConnection::handleRequest(AddressDetails & request) {
	bytes version, method;
	uint8_t methodNum = 0;
	
	if (!mSock->receive(version, 1) && version.size() != 1)
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
//		case Constants::Server::Command::Delete:
//			nodeUpdate();
//			break;
//			
		default:
			cerr <<	"Invalid node request\n!";
			break;
	}	return this->readAddressInformation(request);
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