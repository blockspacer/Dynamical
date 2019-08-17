#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include "system.h"

#include "steam/isteamnetworkingsockets.h"
#include "steam/steamnetworkingsockets.h"

class ServerNetworkSys : public System {
public:
    ServerNetworkSys(entt::registry& reg);
    void preinit() override;
    void init() override;
    void tick() override;
    void finish() override;
    ~ServerNetworkSys() override;
    const char* name() override {return "ServerNetwork";};
private:
    
    class Callbacks : public ISteamNetworkingSocketsCallbacks {
    public:
        void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t * ) override;
        ISteamNetworkingSockets* net;
        HSteamListenSocket socket;
    } callbacks;
    
    ISteamNetworkingSockets* net;
    HSteamListenSocket socket;
};

#endif
