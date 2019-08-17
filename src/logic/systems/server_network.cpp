#include "server_network.h"

#include <iostream>

ServerNetworkSys::ServerNetworkSys(entt::registry& reg) : System(reg) {
    
    SteamNetworkingIdentity identity;
    identity.Clear();
    SteamNetworkingErrMsg error;
    GameNetworkingSockets_Init(&identity, error);
    std::cout << error << std::endl;
    
    net = SteamNetworkingSockets();
    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.SetIPv6LocalHost(25565);
    socket = net->CreateListenSocketIP(addr);
    
    callbacks.net = net;
    callbacks.socket = socket;
    
}

void ServerNetworkSys::preinit() {
    
}

void ServerNetworkSys::init() {
    
}

void ServerNetworkSys::tick() {
    
    net->RunCallbacks(&callbacks);
    
}

void ServerNetworkSys::Callbacks::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* inf) {
    
    if(inf->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
        std::cout << "connecting " << inf->m_info.m_addrRemote.GetIPv4() << std::endl;
        net->AcceptConnection(inf->m_hConn);
    } else if(inf->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
        std::cout << "connected " << inf->m_info.m_addrRemote.GetIPv4() << std::endl;
    } else {
        std::cout << inf->m_info.m_eState << " " << inf->m_info.m_addrRemote.GetIPv4() << std::endl;
    }
    
}

void ServerNetworkSys::finish() {
    
}

ServerNetworkSys::~ServerNetworkSys() {
    
    net->CloseListenSocket(socket);
    GameNetworkingSockets_Kill();
    
}
