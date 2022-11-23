#ifndef __MYOS__NET__ARP_H
#define __MYOS__NET__ARP_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\net\etherframe.h>


namespace myos{
    namespace net{
        struct AddressResolutionProtocolMessage{

            myos::common::uint16_t hardwareType;
            myos::common::uint16_t protocol;
            myos::common::uint8_t hardwareAddressSize;
            myos::common::uint8_t protocolAddressSize;
            myos::common::uint16_t command;


            myos::common::uint64_t srcMAC : 48;
            myos::common::uint32_t srcIP;
            myos::common::uint64_t dstMAC : 48;
            myos::common::uint32_t dstIP; 

        }__attribute__((packed));

        class AddressResolutionProtocol : public EtherFrameHandler{
            myos::common::uint32_t IPcache[128];
            myos::common::uint64_t MACcache[128];
            int numCacheEntries;

            public:
                AddressResolutionProtocol(EtherFrameProvider* backend);
                ~AddressResolutionProtocol();

                bool OnEtherFrameReceived(myos::common::uint8_t* etherframepayload , myos::common::uint32_t size);


                void RequestMACAddress(myos::common::uint32_t IP_BE);
                myos::common::uint64_t GetMACFromCache(myos::common::uint32_t IP_BE);
                myos::common::uint64_t Resolve(myos::common::uint32_t IP_BE);
                void BroadcastMACAddress(myos::common::uint32_t IP_BE);
        };
    }
}




#endif