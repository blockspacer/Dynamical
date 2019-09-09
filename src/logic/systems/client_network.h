#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "system.h"

#include "steam/isteamnetworkingsockets.h"
#include "steam/steamnetworkingsockets.h"

class ClientNetworkSys : public System, public ISteamNetworkingSocketsCallbacks {
public:
    ClientNetworkSys(entt::registry& reg);
    void preinit() override;
    void init() override;
    void tick() override;
    void finish() override;
    ~ClientNetworkSys() override;
    const char* name() override {return "ClientNetwork";};
    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t * ) override;
private:

    ISteamNetworkingSockets* net;
    HSteamNetConnection connection;

};

#endif
