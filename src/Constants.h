/*
 * Constants.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#include <string>
#include "Util.h"

namespace Constants {
	const std::string PlaceholderAddress = Util::hexToString("01cb007101abab");

	namespace Config{
		const bool masterServer = false;
		const bool node = true;
	}
	
	namespace SOCKS {
		namespace Version {
			const int V5 = 0x05;
		};
		namespace Authentication {
			const int NoAuth = 0x00;
			const int Invalid = 0xFF;
		};
		namespace Command {
			const int TCPConnection = 0x01;
		};
	};

	namespace Relay {
		namespace Version {
			const int V1 = 0x01;
		};
		namespace Command {
			const int TCPConnection = 0x01;
		};
		namespace Response {
			const int Granted = 0x00;
			const int InvalidConnection = 0x02;
			const int HostUnreachable = 0x04;
			const int InvalidAddressType = 0x08;
		};
	};
	
	namespace Server {
		namespace Version {
			const int V1 = 0x01;
		};
		namespace Command {
			const int Join = 0x01;
			const int Connect = 0x02;
			const int Find = 0x03;
			const int Response = 0x04;
		};
		namespace Response {
			const int Granted = 0x00;
			const int InvalidConnection = 0x02;
			const int HostUnreachable = 0x04;
			const int InvalidAddressType = 0x08;
			const int Recorded = 0x0F;
			const int Error = 0x10;
			const int Result = 0x20;
		};
	};
	
	namespace IP {
		namespace Type {
			const int IPV4 = 0x01;
			const int Domain = 0x03;
			const int IPV6 = 0x04;
		};
		namespace Length {
			const int IPV4 = 4;
			const int IPV6 = 16;
		};
	};

	namespace Messages {
		namespace SOCKS {
			namespace Auth {
				const std::string InvalidAuth = Util::hexToString("05FF");
				const std::string UseNoAuth = Util::hexToString("0500");
			};

			namespace Request {
				const std::string RequestGranted = Util::hexToString("0500");
				const std::string InvalidConnection = Util::hexToString("0502");
				const std::string HostUnreachable = Util::hexToString("0504");
				const std::string InvalidAddressType = Util::hexToString("0508");

				const std::string Blank = Util::hexToString("00");
				// IPv4, no address, 0 for port
				const std::string InvalidDestinationInformation = Util::hexToString("010000000000");



			};
		};
		namespace Relay {
			namespace Request {
				const std::string OpenConnection =  Util::hexToString("010100");

				namespace Response {
					const std::string RequestGranted = Util::hexToString("0100");
					const std::string InvalidConnection = Util::hexToString("0102");
					const std::string HostUnreachable = Util::hexToString("0104");
					const std::string InvalidAddressType = Util::hexToString("0108");
					const std::string Blank = Util::hexToString("00");
				};
			}
		};

		namespace Master {
			const std::string Version = Util::hexToString("01");
			namespace Request {
				const std::string Join = Util::hexToString("01");
				const std::string ConnectServer = Util::hexToString("02");
				const std::string FindRelay  = Util::hexToString("03");
			}
		}
	};
};

#endif /* SRC_CONSTANTS_H_ */
