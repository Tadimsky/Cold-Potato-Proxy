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
	AddressDetails node, server;
	bytes latencyStr, response;
	uint16_t latency=UINT16_MAX;
	
	if (!readAddressInformation(node)) {
		cerr << "Could not read address information" << endl;
		return;
	}
	if (!readAddressInformation(server)) {
		cerr << "Could not read joining node server address information" << endl;
		return;
	}
	
	if (!mSock->receive(latencyStr, 2) && latencyStr.size() != 2)
		return;
	
	// horrible latency decoding. same as port...
	unsigned char h = (unsigned char)latencyStr[0];
	unsigned char l = (unsigned char)latencyStr[1];
	latency = (h << 8) + l;

	Link link = Link::Link(node, server, latency, true);

	if(link_map->count(link.getServerStr())<=0){
		std::priority_queue<Link, std::vector<Link>, LinkComparator> links;
		links.push(link);
		(*link_map)[link.getServerStr()] = links;
	} else {
		(*link_map)[link.getServerStr()].push(link);
	}
	response.clear();
	response += Constants::Server::Version::V1;
	response += Constants::Server::Response::Recorded;
	//mSock->send(<#const bytes &d#>)
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
//	
}

void MasterConnection::nodeConnect(){
	std::string response;
	response.clear();
	
//	vector<string> tokenList = Util::split(request, '|');
//	if(tokenList.size()!=5){
//		cerr<<"Connect wrong number of parameters!\n";
//	}
//	
//	Node node =  Node::Node(tokenList[1], tokenList[2]);
//	
//	
//	Node server = Node::Node(tokenList[3], tokenList[4]);
//	
//	Link link = Link::Link(node, server, 0, true);
//	if(link_map.count(link.getLinkID())<=0){
//		
//		response.append(std::to_string(Constants::PMessages::NOT_FOUND));
//		
//	} else {
//		//Found!
//		link = link_map[link.getLinkID()].top();
//		response.append(std::to_string(Constants::PMessages::RESPONSE));
//		response.append("|");
//		response.append(link.getLinkID());
//	}
//	
//	sock->send(response);
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
		case Constants::Server::Command::Update:
			nodeUpdate();
			break;
			
		default:
			cerr <<	"Invalid node request\n!";
			break;
	}	return this->readAddressInformation(request);
}



MasterConnection::~MasterConnection() {
	//delete mConnectionData;
}