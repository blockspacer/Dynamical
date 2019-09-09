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

}

void ClientNetworkSys::preinit() {
    
}

void ClientNetworkSys::init() {
    
}

void ClientNetworkSys::tick() {
    net->RunCallbacks(this);
    const int MESSAGE_BUFFER = 10;
    std::vector<SteamNetworkingMessage_t*> messages(MESSAGE_BUFFER);
    int received = net->ReceiveMessagesOnConnection(connection, messages.data(), MESSAGE_BUFFER);
    if(received == -1) {
        std::cerr << "OOF CAN'T RECEOVE ÙESSAGES" << std::endl;
    } else {
        for(int i = 0; i < received; ++i) {
            std::cout << *((uint32_t*) messages[i]->m_pData) << std::endl;
        }
    }

}

void ClientNetworkSys::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* inf) {
    
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
