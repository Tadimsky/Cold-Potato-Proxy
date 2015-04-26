#include "Socket.h"

#include "Common.h"

#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <relay/RelayServer.h>
#include <iomanip>
#include "proxy/ProxyServer.h"
#include "ConnectionData.h"
#include "Util.h"
#include "MasterController.h"

using namespace std;

struct Config
{
	Config() {
		proxyPort = -1;
		relayPort = -1;
		masterAddress;
		masterPort = -1;
	}

	int proxyPort;
	int relayPort;
	std::string masterAddress;
	int masterPort;
};

void initMasterController(Config& config) {

	if (config.masterAddress.empty()) {
		return;
	}

	auto master = MasterController::getInstance();

	AddressDetails masterInfo;
	masterInfo.addressType = IPV4_ADDRESS;
	masterInfo.address = config.masterAddress;
	masterInfo.port = (uint16_t)config.masterPort;

	string address = Socket::getLocalIPAddress();
	if (address.empty()) {
		cerr << "Could not get Local IP Address: " << address << endl;
		exit(1);
	}

	AddressDetails relayInfo;
	relayInfo.addressType = IPV4_ADDRESS;
	relayInfo.address = address;
	relayInfo.port = (uint16_t)config.relayPort;

	master->connect(relayInfo, masterInfo);
}

// Print an error message, usage, and then exit.
void Usage(string errorMessage)
{
	cerr << errorMessage << "\n"
"Usage: [-config <config.cfg (default)>] [-proxy port <port, default 1080>] [-relay port <port, default 1090>] [-master <ip>:<port]\n"
"Command line options supersedes  options in the config file.\n";
	exit(1);
}

Config ReadConfigFromFile(string filename)
{
	Config cfg;
	ifstream input(filename.c_str());

	string key;
	string temp;
	while (true) {

		input >> key;
		if (input.eof() || key.empty())
			break;

		if (key == "-proxy") {
			string type;
			input >> type;
			if (type == "port") {
				input >> cfg.proxyPort;
				if (cfg.proxyPort < 1 || cfg.proxyPort > 65535) {
					Usage("Proxy Port must be between 1 and 65535. You specified " + cfg.proxyPort);
				}
			}
		}
		else if (key == "-relay") {
			string type;
			input >> type;
			if (type == "port") {
				input >> cfg.relayPort;
				if (cfg.relayPort < 1 || cfg.relayPort > 65535) {
					Usage("Relay Port must be between 1 and 65535. You specified " + cfg.relayPort);
				}
			}
		}
		else if (key == "-master") {
			string value;
			input >> value;
			cfg.masterAddress = value.substr(0, value.find(':'));
			cfg.masterPort = StoI(value.substr(value.find(':') + 1), -1);
			if (cfg.masterPort < 1 || cfg.masterPort > 65535) {
				Usage("Master Port must be between 1 and 65535. You specified " + cfg.masterPort);
			}
		}
	}

	return cfg;
}

Config ParseCommandLine(int argc, char* argv[])
{
	Config cfg;

	string configFile = "config.cfg";

	int i = 1;
	while (i < argc - 1) {
		string key = argv[i++];

		if (key == "-config") {
			configFile = argv[i++];
		}
		else if (key == "-proxy") {
			string type = argv[i++];
			if (type == "port") {
				cfg.proxyPort = StoI(argv[i++], -1);
				if (cfg.proxyPort < 1 || cfg.proxyPort > 65535) {
					Usage("Proxy Port must be between 1 and 65535. You specified " + cfg.proxyPort);
				}
			}
		}
		else if (key == "-relay") {
			string type = argv[i++];
			if (type == "port") {
				cfg.relayPort = StoI(argv[i++], -1);
				if (cfg.relayPort < 1 || cfg.relayPort > 65535) {
					Usage("Relay Port must be between 1 and 65535. You specified " + cfg.relayPort);
				}
			}
		}
		else if (key == "-master") {
			string value = argv[i++];
			// parse ip
			cfg.masterAddress = value.substr(0, value.find(':'));
			cfg.masterPort = StoI(value.substr(value.find(':') + 1), -1);
			if (cfg.masterPort < 1 || cfg.masterPort > 65535) {
				Usage("Master Port must be between 1 and 65535. You specified " + cfg.masterPort);
			}
		}
		else
		{
			Usage("Unknown option: " + key);
		}
	}


	Config cfgFromFile = ReadConfigFromFile(configFile);
	// Load in defaults
	if (cfg.relayPort == -1)
		cfg.relayPort = cfgFromFile.relayPort;
	if (cfg.relayPort == -1)
		cfg.relayPort = 1090;

	if (cfg.proxyPort == -1)
		cfg.proxyPort = cfgFromFile.proxyPort ;
	if (cfg.proxyPort == -1)
		cfg.proxyPort = 1080;

	if (cfg.masterPort == -1)
		cfg.masterPort = cfgFromFile.masterPort ;
	if (cfg.masterPort == -1)
		cfg.masterPort = 1090;

	if (cfg.masterAddress.empty())
		cfg.masterAddress = cfgFromFile.masterAddress ;
	if (cfg.masterAddress.empty())
		cfg.masterAddress = "127.0.0.1";

	return cfg;
}

int main(int argc, char* argv[])
{
	// Command line options will be:
	// -config <file>
	// -proxy port <port>
	// -relay port <port>
	// -master <address>:<port>
	Config cfg = ParseCommandLine(argc, argv);

	AddressDetails t;
	t.addressType = DOMAIN_ADDRESS;
	t.address = "localhost";
	t.port = 443;

	stringstream ss;
	ss << t;
	string pp = ss.str();

	cout << "Local IP Address: " << Socket::getLocalIPAddress() << endl;

	thread p([&] {
		ProxyServer proxy = ProxyServer(cfg.proxyPort);
		proxy.Listen();
	});

	thread r([&] {
		RelayServer relay = RelayServer(cfg.relayPort);
		relay.Listen();
	});
	initMasterController(cfg);

	p.join();
	r.join();

	return 0;
}
