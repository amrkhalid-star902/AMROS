#ifndef __MYOS__NET__IPV4_H
#define __MYOS__NET__IPV4_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\net\etherframe.h>
#include <G:\MyOS\include\net\arp.h>


namespace myos{
    namespace net{


        struct InternetProtocolV4Message{

            myos::common::uint8_t headerLength : 4;
            myos::common::uint8_t version : 4;
            myos::common::uint8_t tos ; //type of service
            myos::common::uint16_t totalLength ;


            myos::common::uint16_t ident ; //identifier
            myos::common::uint16_t FlagsAndOffset ;

            myos::common::uint8_t TimeToLive;
            myos::common::uint8_t protocol; //whether we will use TCP or UDP or ICMP in transport layer
            myos::common::uint16_t checksum;

            myos::common::uint32_t srcIP;
            myos::common::uint32_t dstIP;
        }__attribute__((packed));

        class InternetProtocolProvider;

        class InternetProtocolHandler{
            protected:
                InternetProtocolProvider* backend;
                myos::common::uint8_t ip_protocol;

            public:
                InternetProtocolHandler(InternetProtocolProvider* backend , myos::common::uint8_t protocol);
                ~InternetProtocolHandler();

                virtual bool OnInternetProtocolReceived(myos::common::uint32_t srcIP_BE , myos::common::uint32_t dstIP_BE,
                myos::common::uint8_t* internetprotocolPayload , myos::common::uint32_t size);

                void Send(myos::common::uint32_t dstIP_BE , myos::common::uint8_t* internetprotocolPayload , myos::common::uint32_t size ); 
        };

        class InternetProtocolProvider : public EtherFrameHandler{
            friend class InternetProtocolHandler;
            protected:
                InternetProtocolHandler* handlers[255];
                AddressResolutionProtocol* arp;
                myos::common::uint32_t gatewayIP;
                myos::common::uint32_t subnetMask;


            public:
                InternetProtocolProvider(EtherFrameProvider* backend,
                AddressResolutionProtocol* arp,
                myos::common::uint32_t gatewayIP,
                myos::common::uint32_t subnetMask); 

                ~InternetProtocolProvider();

                bool OnEtherFrameReceived(myos::common::uint8_t* etherframePayload , myos::common::uint32_t size);

                void Send(myos::common::uint32_t dstIP_BE , myos::common::uint8_t protocol,myos::common::uint8_t* buffer , myos::common::uint32_t size); 

                static myos::common::uint16_t Checksum(myos::common::uint16_t* data , myos::common::uint32_t length);
        };
    }
}


#endif
