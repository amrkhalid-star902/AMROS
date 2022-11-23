#ifndef __MYOS__NET__ICMP_H
#define __MYOS__NET__ICMP_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\net\ipv4.h>

namespace myos{
    namespace net{
        struct InternetControlMessageProtocolMessage{

            myos::common::uint8_t type;
            myos::common::uint8_t code;

            myos::common::uint16_t checksum;
            myos::common::uint32_t data;
        }__attribute((packed));

        class InernetControlMessageProtocol : InternetProtocolHandler{
            public:
                InernetControlMessageProtocol(InternetProtocolProvider* backend);
                ~InernetControlMessageProtocol();

                virtual bool OnInternetProtocolReceived(myos::common::uint32_t srcIP_BE , myos::common::uint32_t dstIP_BE,
                myos::common::uint8_t* internetprotocolPayload , myos::common::uint32_t size);

                void RequestEchoReply(myos::common::uint32_t ip_be); 
        };
    }
}


#endif