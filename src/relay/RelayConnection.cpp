/*
 * RelayConnection.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#include <sstream>
#include "RelayConnection.h"
#include "Constants.h"

#include <iostream>
#include <master/MasterServer.h>
#include <MasterController.h>
#include <future>

using namespace std;

RelayConnection::RelayConnection(ConnectionData* connection) : Connection(connection) {
}

void RelayConnection::handleConnection() {
	int sock = mConnectionData->socket;

	mSock = std::make_unique<Socket>(sock);

	AddressDetails request;
	if (!this->handleRequest(request)) {
		return;
	}

	// Try to connect.
	auto outSock = setupForwardConnection(request);
	if (outSock == nullptr) {
		return;
	}

	this->relayTraffic(outSock);
}

bool RelayConnection::verifyVersion(char version) {
	if (version != Constants::Relay::Version::V1) {
		cerr << "Invalid relay version." << endl;
		return false;
	}
	return true;
}

bool RelayConnection::handleRequest(AddressDetails & request) {
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

std::shared_ptr<Socket> RelayConnection::setupForwardConnection(const AddressDetails & request) {
	// this is the relay, so we connect to the actual destination

	auto outSock = std::make_shared<Socket>();

	// Send reply.
	if (outSock->connect(request))
	{
		using namespace Constants::Messages::Relay::Request;
		cerr << "Relay connected to destination!" << endl;
		mSock->send(Response::RequestGranted + Response::Blank  + Constants::PlaceholderAddress);
	}
	else
	{
		using namespace Constants::Messages::Relay::Request;
		cerr << "Relay could not connect to destination." << endl;
		mSock->send(Response::HostUnreachable + Response::Blank + Constants::PlaceholderAddress);
		return nullptr;
	}
	// here we want to notify the master server that we have connection to the destination
	std::future<bool> notify = std::async(std::launch::async, &MasterController::notifyConnection, MasterController::getInstance(), request);
	//notify.get();
	return outSock;
}

RelayConnection::~RelayConnection() {
	//delete mConnectionData;
}
