#ifndef __MYOS__NET_UDP_H
#define __MYOS__NET_UDP_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\net\ipv4.h>
#include <G:\MyOS\include\memorymangement.h>

namespace myos{
    namespace net{

        struct UserDatagramProtocolHeader{

            myos::common::uint16_t srcPort;
            myos::common::uint16_t dstPort;
            myos::common::uint16_t length;
            myos::common::uint16_t checksum;
        }__attribute__((packed));

        class UserDatagramProtocolSocket;
        class UserDatagramProtocolProvider;


        class UserDatagramProtocolHandler{

            public:
                UserDatagramProtocolHandler();
                ~UserDatagramProtocolHandler();
                virtual void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket , myos::common::uint8_t* data , myos::common::uint16_t size);

        };


        class UserDatagramProtocolSocket{
            friend class UserDatagramProtocolProvider;
            protected:
                myos::common::uint16_t remotePort;
                myos::common::uint32_t remoteIP;
                myos::common::uint16_t localPort;
                myos::common::uint32_t localIP;
                UserDatagramProtocolProvider* backend;
                UserDatagramProtocolHandler* handler;
                bool listening;


            public:
                UserDatagramProtocolSocket(UserDatagramProtocolProvider* backend);
                ~UserDatagramProtocolSocket();
                virtual void HandleUserDatagramProtocolMessage(myos::common::uint8_t* data , myos::common::uint16_t size);
                virtual void Send(myos::common::uint8_t* data , myos::common::uint16_t size);
                virtual void Disconnect();
        };


        class UserDatagramProtocolProvider : InternetProtocolHandler{
            protected:
                UserDatagramProtocolSocket* sockets[65535];
                myos::common::uint16_t numSockets;
                myos::common::uint16_t freePort;

            public:
                UserDatagramProtocolProvider(InternetProtocolProvider* backend);
                ~UserDatagramProtocolProvider();

                virtual bool OnInternetProtocolReceived(myos::common::uint32_t srcIP_BE , myos::common::uint32_t dstIP_BE,
                myos::common::uint8_t* internetprotocolPayload , myos::common::uint32_t size); 

                virtual UserDatagramProtocolSocket* Connect(myos::common::uint32_t ip , myos::common::uint16_t port);
                virtual UserDatagramProtocolSocket* Listen(myos::common::uint16_t port);
                virtual void Disconnect(UserDatagramProtocolSocket* socket);
                virtual void Send(UserDatagramProtocolSocket* socket , myos::common::uint8_t* data , myos::common::uint16_t size);

                virtual void Bind(UserDatagramProtocolSocket* socket , UserDatagramProtocolHandler* handler);
        };
}
}


#endif
