# Cold Potato Proxy
This is a proxy server that routes traffic in its network as close to the destination as possible, before letting it exit to the rest of the web.

## Requirements
To build, please use ```cmake```. There is a cmake build file that will ensure that everything builds correctly.

## Instructions
There are two modes to the program, **Master Server** or **SOCKS Proxy + Relay Node**

### Master Server
To start a Master Server, run the following command: 
```Cold_Potato_Proxy -type master```

There are no config options for the master server, it listens on port 1337.

### SOCKS Proxy + Relay Node
If you do not run a master node, the other option is to run both a proxy and relay node.
These are the configuration options:
* ```-proxy port <port number>```
  * This will choose the port that the proxy server will listen on. Defaults to 1080.
* ```-relay port <port number>```
  * This will choose the port that the relay server will listen on. Defaults to 1090.
* ```-master <address:port>```
  * This specifies the address and port of the master server.
* ```-config <file>
  * This specifies a config file to read from. The config file has the same format as the parameters above.

**Command line arguments override config file parameters**

# Example
To start a master server, one proxy server, and one relay server, run the following in different terminals:

* Master Server
  * ```Cold_Potato_Proxy -type master```
* Proxy and Relay
  * ```Cold_Potato_Proxy -master 127.0.0.1:1337```
  * Defaults to proxy on port 1080, relay on port 1090

Then either configure  your browser's network settings to use a SOCKSv5 Proxy, with localhost:1080.
