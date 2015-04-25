#include "Socket.h"

#include "Common.h"

#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "relay/RelayServer.h"
#include "proxy/ProxyServer.h"
#include "ConnectionData.h"
#include "Util.h"
#include "Constants.h"

using namespace std;

struct Config
{
	Config() : port(1080), nodeType(Constants::Config::node){ }
	int port;
	bool nodeType;
};

// Print an error message, usage, and then exit.
void Usage(string errorMessage)
{
	cerr << errorMessage << "\n"
"Usage: oddsocks [-config <oddsocks.cfg (default)>] [-port <port, default 1080>]\n"
"Command line options supersedes  options in the config file.\n"
"Config file is simply the port.\n";
	exit(1);
}

Config ReadConfigFromFile(string filename)
{
	Config cfg;
	ifstream input(filename.c_str());
	input >> cfg.port;
	return cfg;
}

Config ParseCommandLine(int argc, char* argv[])
{
	Config cfg;
	cfg.port = -1;

	string configFile = "oddsocks.cfg";

	if (argc % 2 != 1)
	{
		Usage("Expected an even number of arguments.");
	}
	for (int i = 0; i < argc/2; ++i)
	{
		string key = argv[i*2+1];
		string value = argv[i*2+2];
		if (key == "-config")
		{
			configFile = value;
		}
		else if (key == "-port")
		{
			cfg.port = StoI(value, -1);
			if (cfg.port < 1 || cfg.port > 65535)
				Usage("Port must be between 1 and 65535. Read value " + value + " understood as " + ItoS(cfg.port));
		}
		else if (key == "-type"){
			if(value == "master"){
				cfg.nodeType = Constants::Config::masterServer;
			} else {
				cfg.nodeType = Constants::Config::node;
			}
		}
		else
		{
			Usage("Unknown option: " + key);
		}
	}

	// Try to read config file. Slightly dubious logic here!
	// I should make this more clear.
	Config cfgFromFile = ReadConfigFromFile(configFile);
	if (cfg.port == -1)
		cfg.port = cfgFromFile.port;
	if (cfg.port == -1)
		cfg.port = 1080;

	return cfg;
}

int main(int argc, char* argv[])
{
	// Command line options will be:
	// -type <master-node, node>
	// -config <file>
	// -password <pw>
	// -port <port>
	Config cfg = ParseCommandLine(argc, argv);

	AddressDetails t;
	t.addressType = DOMAIN_ADDRESS;
	t.address = "localhost";
	t.port = 443;

	stringstream ss;
	ss << t;
	string pp = ss.str();


	int port = cfg.port;

	if(cfg.nodeType == Constants::Config::node){
	thread p([&] {
		ProxyServer proxy = ProxyServer(port);
		proxy.Listen();
	});

	thread r([&] {
		RelayServer relay = RelayServer(1090);
		relay.Listen();
	});

	p.join();
	r.join();
	} else {
		thread r([&] {
			//RelayServer relay = RelayServer(1090);
			//relay.Listen();
		});
	}
	
	
	return 0;
}
