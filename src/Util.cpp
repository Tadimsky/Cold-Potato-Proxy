/*
 * Util.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: jonno
 */

#include "Util.h"
#include "Constants.h"

#include <stdexcept>

using namespace std;

namespace Util {



static const char* const lookupTable = "0123456789ABCDEF";

std::string hexToString(const std::string& hex) {

	size_t len = hex.length();
	if (len & 1) throw std::invalid_argument("Incorrect Length for hex value. Should not be odd.");

	std::string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = toupper(hex[i]);
		const char* p = std::lower_bound(lookupTable, lookupTable + 16, a);
		if (*p != a) {
			throw std::invalid_argument("not a hex digit");
		}

		char b = toupper(hex[i + 1]);
		const char* q = std::lower_bound(lookupTable, lookupTable + 16, b);
		if (*q != b) {
			throw std::invalid_argument("not a hex digit");
		}

		output.push_back(((p - lookupTable) << 4) | (q - lookupTable));
	}
	return output;
}

std::string stringToHex(const std::string& str) {
	size_t len = str.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = str[i];
		output.push_back(lookupTable[c >> 4]);
		output.push_back(lookupTable[c & 15]);
	}
	return output;
}


AddressDetails getBestRelay(const AddressDetails &destination) {
	AddressDetails response;
	response.address = "localhost";
	response.port = 1090;
	response.addressType = DOMAIN_ADDRESS;

	return response;
}

	bool readAddressInformation(std::shared_ptr<Socket> socket, AddressDetails &rq) {
		bytes address;

		AddressType addressType;
		bytes type;

		// read the type of address


		if (!socket->receive(type, 1)) {
			return false;
		}

		// Get the type of address they want to connect to.
		switch (type[0])
		{
			case Constants::IP::Type::IPV4: {
				addressType = IPV4_ADDRESS;
				socket->receive(address, Constants::IP::Length::IPV4);
			}
				break;

			case Constants::IP::Type::Domain: {
				addressType = DOMAIN_ADDRESS;
				bytes len;
				// get length of domain name
				socket->receive(len, 1);
				// get domain name
				socket->receive(address, len[0]);
			}
				break;

			case Constants::IP::Type::IPV6: {
				addressType = IPV6_ADDRESS;
				socket->receive(address, Constants::IP::Length::IPV6);
			}
				break;

			default:
				cerr << "Invalid address type: " << hex << type << endl;
				{
					//using namespace Constants::Messages::Request;
					//mSock->send(InvalidAddressType + Blank + InvalidDestinationInformation);
				}
				return false;
		}

		// Get the port.
		bytes rawPort;
		if (!socket->receive(rawPort, 2)) {
			cerr << "Could not read the port" << endl;
			return false;
		}
		// horrible port decoding. oh well.
		unsigned char h = (unsigned char)rawPort[0];
		unsigned char l = (unsigned char)rawPort[1];
		uint16_t port = (h << 8) + l;

		rq.address = address;
		rq.addressType = addressType;
		rq.port = port;

		return true;
	}

} /* namespace Util */

