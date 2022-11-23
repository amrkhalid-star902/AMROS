#ifndef __MYOS__NET__ETHERFRAME_H
#define __MYOS__NET__ETHERFRAME_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\drivers\amd_am79c973.h>
#include <G:\MyOS\include\memorymangement.h>



namespace myos{
    namespace net{
        struct EtherFrameHeader
        {
            myos::common::uint64_t dstMAC_BE : 48; //distnation MAC address in Big Endian Format
            myos::common::uint64_t srcMAC_BE : 48; //Source MAC address in Big Endian Format 
            myos::common::uint16_t etherType_BE;
        }__attribute((packed));

        typedef myos::common::uint32_t EtherFrameFooter;

        class EtherFrameProvider;

        class EtherFrameHandler{
            protected:
                EtherFrameProvider* backend;
                myos::common::uint16_t etherType_BE;

            public:
                EtherFrameHandler(EtherFrameProvider* backend , myos::common::uint16_t etherType);
                ~EtherFrameHandler();

                virtual bool OnEtherFrameReceived(myos::common::uint8_t* etherframepayload , myos::common::uint32_t size);
                void Send(myos::common::uint64_t dstMAC_BE , myos::common::uint8_t* etherframepayload , myos::common::uint32_t size);
                myos::common::uint32_t GetIPAddress();
        };

        class EtherFrameProvider : public myos::drivers::RawDataHandler{
            friend class EtherFrameHandler;
            protected:
                EtherFrameHandler* handlers[65535];

            public:
                EtherFrameProvider(drivers::amd_am79c973* backend);
                ~EtherFrameProvider();

                bool OnRawDataReceived(myos::common::uint8_t* buffer , myos::common::uint32_t size);
                void Send(myos::common::uint64_t dstMAC_BE , myos::common::uint16_t etherType_BE,myos::common::uint8_t* buffer , myos::common::uint32_t size);
                myos::common::uint64_t GetMACAddress();
                myos::common::uint32_t GetIPAddress();
        };
    }
}



#endif