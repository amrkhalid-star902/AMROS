#ifndef __MYOS__DRIVERS_AMD_AM79C973_H
#define __MYOS__DRIVERS_AMD_AM79C973_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\drivers\driver.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>
#include <G:\MyOS\include\hardwarecommunication\pci.h>

namespace myos{
    namespace drivers{


        class amd_am79c973 ;

        class RawDataHandler{
            protected:
            amd_am79c973* backend;
            public:
            RawDataHandler(amd_am79c973* backend);
            ~RawDataHandler();

            virtual bool OnRawDataReceived(myos::common::uint8_t* buffer , myos::common::uint32_t size);
            void Send(myos::common::uint8_t* buffer , myos::common::uint32_t size);

        };

        class amd_am79c973 : public Driver , public hardwarecommunication::InterruptHandler
        {
            struct InitializationBlock{

                myos::common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                myos::common::uint64_t physicalAddress : 48;
                myos::common::uint16_t reserved3;
                myos::common::uint64_t logicalAddress;
                myos::common::uint32_t recvBufferDescrAddress;
                myos::common::uint32_t sendBufferDescrAddress;

            };


            struct BufferDescriptor{
            myos::common::uint32_t address;
            myos::common::uint32_t flags;
            myos::common::uint32_t flags2;
            myos::common::uint32_t avail;

            };

            //MAC address consist of 6 bytes
            myos::hardwarecommunication::Port16bit MACAdress0Port;
            myos::hardwarecommunication::Port16bit MACAdress2Port;
            myos::hardwarecommunication::Port16bit MACAdress4Port;
            myos::hardwarecommunication::Port16bit registerDataPort;
            myos::hardwarecommunication::Port16bit registerAddressPort;
            myos::hardwarecommunication::Port16bit resetPort;
            myos::hardwarecommunication::Port16bit busControlRegisterDataPort;

            InitializationBlock initBlock;

            BufferDescriptor* sendBufferDescr;
            myos::common::uint8_t sendBufferDescrMemory[2048+15];
            myos::common::uint8_t sendBuffers[2*1024+15][18];
            myos::common::uint8_t currentSendBuffer;
            BufferDescriptor* recvBufferDescr;
            myos::common::uint8_t recvBufferDescrMemory[2048+15];
            myos::common::uint8_t recvBuffers[2*1024+15][18];
            myos::common::uint8_t currentrecvBuffer;


            RawDataHandler* handler;
        public:
            amd_am79c973(myos::hardwarecommunication::PeripheralComponentInterconnectDeviceDescriptor*dev,
            myos::hardwarecommunication::InterruptManager* interrupts);
            ~amd_am79c973();
            void Activate();
            int Reset();
            myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);

            void Send(myos::common::uint8_t* buffer , int count);
            void Receive();

            void SetHandler(RawDataHandler* handler);
            myos::common::uint64_t GetMACAddress();
            void SetIPAddress(myos::common::uint32_t);
            myos::common::uint32_t GetIPAddress(); 
        };

    }
}

#endif