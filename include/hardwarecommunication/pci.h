#ifndef __MYOS_HARDWARECOMMUNICATION__PCI_H
#define __MYOS_HARDWARECOMMUNICATION__PCI_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\drivers\driver.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>


namespace myos{
    namespace hardwarecommunication{

        enum BaseAddressRegisterType{
        MemoryMapping=0,
        InputOutput=1
        };

        class BaseAddressRegister{
            public:
                bool prefetchable;
                myos::common::uint8_t* address;
                myos::common::uint32_t size;
                BaseAddressRegisterType type; 
        };


        class PeripheralComponentInterconnectDeviceDescriptor{
            public:
                myos::common::uint32_t portbase;
                myos::common::uint32_t interrupt;

                myos::common::uint16_t bus;
                myos::common::uint16_t device;
                myos::common::uint16_t function;

                myos::common::uint16_t vendor_id;
                myos::common::uint16_t device_id;

                myos::common::uint8_t class_id;
                myos::common::uint8_t subclass_id;
                myos::common::uint8_t interface_id;

                myos::common::uint8_t revision;

                PeripheralComponentInterconnectDeviceDescriptor(myos::common:: uint16_t bus, myos::common:: uint16_t device, myos::common:: uint16_t function);
                ~PeripheralComponentInterconnectDeviceDescriptor();
        };

        class PeripheralComponentconnectController{
            Port32bit dataport;
            Port32bit commandport;


            public:
                PeripheralComponentconnectController();
                ~PeripheralComponentconnectController();
                myos::common::uint32_t Read(myos::common::uint16_t bus,myos::common::uint16_t device ,myos::common::uint16_t function ,myos::common::uint32_t registeroffset);
                void Write(myos::common::uint16_t bus,myos::common::uint16_t device ,myos::common::uint16_t function ,myos::common::uint32_t registeroffset, myos::common::int32_t value);
                bool DeviceHasFunction(myos::common::uint16_t bus,myos::common::uint16_t device);
                void SelectDriver(myos::drivers::DriverManager* driverManager, myos::hardwarecommunication::InterruptManager* interrupts); // this function insert drivers to driver manager
                myos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev,myos::hardwarecommunication::InterruptManager*interrupts);
                PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(myos::common::uint16_t bus,myos::common::uint16_t device ,myos::common::uint16_t function );
                BaseAddressRegister GetBaseAddressRegister(myos::common::uint16_t bus,myos::common::uint16_t device ,myos::common::uint16_t function , myos::common::uint16_t bar);

        };
    }
}


#endif