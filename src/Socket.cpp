#include "Socket.h"

#include <algorithm>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sstream>
#include <iostream>
#include <ifaddrs.h>
#include <string.h>
#include "Util.h"

Socket::Socket()
{
	// No socket.
	fd = -1;
}
Socket::Socket(int sock)
{
	fd = sock;
}

Socket::~Socket()
{
	if (fd != -1)
		::close(fd);
}


bool Socket::connect4(const bytes& ip, uint16_t port)
{
	Socket::close();

	fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in dest_addr;
	char* a = reinterpret_cast<char*>(&dest_addr.sin_addr.s_addr);
	a[0] = ip[0]; // Tested on sane little endian architecture.
	a[1] = ip[1]; // Does anyone still use big endian?
	a[2] = ip[2];
	a[3] = ip[3];
	dest_addr.sin_port = htons(port);
	dest_addr.sin_family = AF_INET;

	char output[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(dest_addr.sin_addr), output, INET_ADDRSTRLEN);
	std::cerr << "Connecting to " << output << ":" << port << std::endl;

	if (::connect(fd, reinterpret_cast<sockaddr*>(&dest_addr), sizeof(dest_addr)) == -1)
	{
		::close(fd);
		fd = -1;
		return false;
	}
	return true;
}


bool Socket::connect6(const bytes& ip, uint16_t port)
{
	Socket::close();

	fd = socket(AF_INET6, SOCK_STREAM, 0);

	sockaddr_in6 dest_addr;
	for (int i = 0; i < 16; ++i)
		dest_addr.sin6_addr.s6_addr[i] = ip[i];
	dest_addr.sin6_port = htons(port);
	dest_addr.sin6_family = AF_INET;
	dest_addr.sin6_flowinfo = 0; // No idea what these do.
	dest_addr.sin6_scope_id = 0;

	char output[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &(dest_addr.sin6_port), output, INET6_ADDRSTRLEN);
	std::cerr << "Connecting to " << output << ":" << port << std::endl;

	if (::connect(fd, reinterpret_cast<sockaddr*>(&dest_addr), sizeof(dest_addr)) == -1)
	{
		::close(fd);
		fd = -1;
		return false;
	}
	return true;
}

bool Socket::connect(const std::string& domain, uint16_t port)
{
	Socket::close();

	std::cerr << "Connecting to " << domain << ":" << port << std::endl;

	std::stringstream service;
	service << port;

	addrinfo* addrs = nullptr;
	int r = getaddrinfo(domain.c_str(), service.str().c_str(), nullptr, &addrs);
	if (r != 0)
	{
		return false;
	}

	if (!addrs)
	{
		return false;
	}

	for (addrinfo* p = addrs; p->ai_next; p = p->ai_next)
	{
		if ((p->ai_family == AF_INET /*|| p->ai_family == AF_INET6*/) // TODO: IPv6!
				&& (p->ai_socktype == SOCK_STREAM))
		{
			// Ok use this one.
			fd = socket(p->ai_family, SOCK_STREAM, 0);
			if (::connect(fd, p->ai_addr, p->ai_addrlen) == -1)
			{
				freeaddrinfo(addrs);
				::close(fd);
				fd = -1;
				return false;
			}

			freeaddrinfo(addrs);
			return true;
		}
	}
	return false;
}


bool Socket::send(const bytes& d)
{
	if (fd == -1)
	{
		errno = EBADF;
		return false;
	}
	int s = ::send(fd, d.data(), d.size(), 0);
	return s != -1;
}

bool Socket::receive(bytes& d, ssize_t size)
{
	int flags = size > 0 ? MSG_WAITALL : 0;

	if (size <= 0)
		size = 4096;

	char buffer[4096];
	d.clear();

	if (fd == -1)
	{
		errno = EBADF;
		return false;
	}
	ssize_t s = ::recv(fd, buffer, size, flags);
	if (s == -1)
		return false;
	d.append(buffer, s);
	return true;
}

int Socket::descriptor()
{
	return fd;
}

void Socket::close()
{
	if (fd == -1)
		return;
	::close(fd);
	fd = -1;
}

#ifdef __linux__
bool Socket::originalDestination(uint32_t& ip, uint16_t& port) const
{
	ip = 0;
	port = 0;

	if (fd == -1)
	{
		errno = EBADF;
		return false;
	}

	sockaddr_in dest_addr;
	socklen_t dest_addr_len = sizeof(dest_addr);
	if (getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, &dest_addr, &dest_addr_len) == -1)
		return false;

	ip = ntohl(dest_addr.sin_addr.s_addr);
	port = ntohs(dest_addr.sin_port);
	return true;
}
#endif

bool Socket::connect(const AddressDetails &a) {
	bool connected = false;

	switch (a.addressType) {
		case IPV4_ADDRESS:
			connected = this->connect4(a.address, a.port);
			break;
		case IPV6_ADDRESS:
			connected = this->connect6(a.address, a.port);
			break;
		case DOMAIN_ADDRESS:
			connected = this->connect(a.address, a.port);
			break;
		default:
			std::cerr << "No connection type specified." << std::endl;
			// send some error message

			break;
	}
	if (!connected) {
		fd = -1;
	}

	return connected;
}

std::string Socket::getLocalIPAddress(std::string& ipString, std::string& ipData) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	char* addressBuffer = nullptr;

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			addressBuffer = (char*)malloc(INET_ADDRSTRLEN * sizeof(char));
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			// need to get non loopback adapter
			if (strcmp(ifa->ifa_name, "lo") != 0) {
				ipString = addressBuffer;

				char data[INET_ADDRSTRLEN];
				memcpy(data, tmpAddrPtr, INET_ADDRSTRLEN);
				ipData = data;
				break;
			}
		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			addressBuffer = (char*)malloc(INET6_ADDRSTRLEN * sizeof(char));
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
		}
	}
	if (ifAddrStruct!=NULL) {
		freeifaddrs(ifAddrStruct);
	}
	return std::string(addressBuffer);
}

bool Socket::isConnected() {
	return this->fd != -1;
}
