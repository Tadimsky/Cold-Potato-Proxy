//
//  MasterServer.h
//  Cold-Potato-Proxy
//
//  Created by Elder Yoshida on 4/25/15.
//  Copyright (c) 2015 Elder Yoshida. All rights reserved.
//

#ifndef __Cold_Potato_Proxy__MasterServer__
#define __Cold_Potato_Proxy__MasterServer__

#include "../ListenServer.h"
#include "Link.h"

#include <unordered_map>
#include <set>
#include <vector>
#include <mutex>

struct LinkComparator
{
	bool operator() (const Link *lhs, const Link *rhs) const
	{
		return lhs->getLatency() > rhs->getLatency();
	}
};

struct KeyHasher
{
	std::size_t operator()(const AddressDetails& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;
		
		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:
		
		return ((hash<int>()(k.addressType)
				 ^ (hash<string>()(k.address) << 1)) >> 1)
		^ (hash<int>()(k.port) << 1);

	}
};



typedef struct std::unordered_map<AddressDetails,std::set<Link*, LinkComparator>, KeyHasher> LinkHashMap;
typedef struct std::unordered_map<AddressDetails,std::vector<Link*>, KeyHasher> NodeHashMap;

class MasterServer : public ListenServer{
private:
	
public:
	MasterServer(int port) : ListenServer(port) {};
	
	virtual void processConnection(ConnectionData *data) override;
	LinkHashMap link_map;
	NodeHashMap node_map;
	std::mutex map_lock;

};

#endif /* defined(__Cold_Potato_Proxy__MasterServer__) */
