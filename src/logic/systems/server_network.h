#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include "system.h"

#include "steam/isteamnetworkingsockets.h"
#include "steam/steamnetworkingsockets.h"

class ServerNetworkSys : public System, public ISteamNetworkingSocketsCallbacks {
public:
    ServerNetworkSys(entt::registry& reg);
    void preinit() override;
    void init() override;
    void tick() override;
    void finish() override;
    ~ServerNetworkSys() override;
    const char* name() override {return "ServerNetwork";}
    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t * ) override;
private:
    ISteamNetworkingSockets* net;
    HSteamListenSocket socket;
};

#endif
