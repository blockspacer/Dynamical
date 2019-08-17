#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "system.h"

#include "steam/isteamnetworkingsockets.h"
#include "steam/steamnetworkingsockets.h"

class ClientNetworkSys : public System {
public:
    ClientNetworkSys(entt::registry& reg);
    void preinit() override;
    void init() override;
    void tick() override;
    void finish() override;
    ~ClientNetworkSys() override;
    const char* name() override {return "ClientNetwork";};
private:
    
    ISteamNetworkingSockets* net;
    HSteamNetConnection connection;
    
    class Callbacks : public ISteamNetworkingSocketsCallbacks {
    public:
        void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t * ) override;
        ISteamNetworkingSockets* net;
        HSteamNetConnection connection;
    } callbacks;
};

#endif
