#include "client_network.h"

ClientNetworkSys::ClientNetworkSys(entt::registry& reg) : System(reg) {
    
    SteamNetworkingIdentity identity;
    identity.Clear();
    SteamNetworkingErrMsg error;
    GameNetworkingSockets_Init(&identity, error);
    std::cout << error << std::endl;
    
    net = SteamNetworkingSockets();
    
    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.SetIPv6LocalHost(25565);
    connection = net->ConnectByIPAddress(addr);
    
    callbacks.net = net;
    callbacks.connection = connection;
    
}

void ClientNetworkSys::preinit() {
    
}

void ClientNetworkSys::init() {
    
}

void ClientNetworkSys::tick() {
    
    net->RunCallbacks(&callbacks);
    
}

void ClientNetworkSys::Callbacks::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* inf) {
    
    if(inf->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
        std::cout << "connected " << inf->m_info.m_addrRemote.GetIPv4() << std::endl;
    } else {
        std::cout << inf->m_info.m_eState << " " << inf->m_info.m_addrRemote.GetIPv4() << std::endl;
    }
    
}

void ClientNetworkSys::finish() {
    
}

ClientNetworkSys::~ClientNetworkSys() {
    
    net->CloseConnection(connection, 0, "", false);
    GameNetworkingSockets_Kill();
    
}
