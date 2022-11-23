
#include <G:\MyOS\include\hardwarecommunication\pci.h>
#include <G:\MyOS\include\memorymangement.h>
#include <G:\MyOS\include\drivers\amd_am79c973.h>


using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::drivers;

void printf(char*);
void printfHex(uint8_t);

PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function){
    this->bus=bus;
    this->device=device;
    this->function=function;
}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor(){

}


PeripheralComponentconnectController::PeripheralComponentconnectController()
: dataport(0xCFC),
commandport(0XCF8)
{

}

PeripheralComponentconnectController::~PeripheralComponentconnectController(){

}

uint32_t PeripheralComponentconnectController::Read(uint16_t bus,uint16_t device ,uint16_t function ,uint32_t registeroffset){
    uint32_t id=
    0x80000000
    |((bus & 0xFF) << 16)
    |((device & 0x1F) << 11)
    |((function & 0x07) << 8)
    |((registeroffset & 0xFC)); // the least two significant biy must be zero to make read and write aligned with 4 bytes bits(address must be multiable of 4)

    commandport.Write(id);
    uint32_t result=dataport.Read();
    return result >>(8*(registeroffset%4));
}

void PeripheralComponentconnectController::Write(uint16_t bus,uint16_t device ,uint16_t function ,uint32_t registeroffset, int32_t value){
    uint32_t id=
    0x80000000
    |((bus & 0xFF) << 16)
    |((device & 0x1F) << 11)
    |((function & 0x07) << 8)
    |((registeroffset & 0xFC));

    commandport.Write(id);
    dataport.Write(value);
}

bool PeripheralComponentconnectController::DeviceHasFunction(uint16_t bus, uint16_t device){
    // we read 0X0E address anf 7th bit determine if this address have multiable function or not
    return Read(bus,device,0,0x0E)&(1<<7);
}

void PeripheralComponentconnectController::SelectDriver(DriverManager* drivermanager, InterruptManager* interrupts){
    for(int bus=0; bus <8; bus++){
        for(int device=0; device <32; device++){
            int numfunctions=DeviceHasFunction(bus,device)?8:1;
            for(int function=0; function < numfunctions; function++){
                PeripheralComponentInterconnectDeviceDescriptor dev=GetDeviceDescriptor(bus,device,function);
                // if vendor_id=0x0000 or 0xFFFF that means there is no device on that function
                if(dev.vendor_id==0x0000 || dev.vendor_id==0xFFFF)
                    continue;

                for(int barNum=0; barNum <6 ; barNum++){
                    BaseAddressRegister bar=GetBaseAddressRegister(bus,device,function,barNum);
                    if(bar.address && (bar.type ==1))
                    dev.portbase=(uint32_t)bar.address;
                }

                Driver* driver=GetDriver(dev,interrupts);
                if(driver!=0)
                    drivermanager->AddDriver(driver);





                printf("PCI BUS");
                printfHex(bus &0xFF); 
                printf(", DEVICE");
                printfHex(device &0xFF); 

                printf(", FUNCTION");
                printfHex(function &0xFF);

                printf(" = VENDOR");
                printfHex((dev.vendor_id &0xFF00) >>8);
                printfHex(dev.vendor_id & 0xFF);
                printf(", DEVICE");
                printfHex((dev.device_id &0xFF00) >>8);
                printfHex(dev.device_id & 0xFF); 
                printf("\n");




            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentconnectController ::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function){
PeripheralComponentInterconnectDeviceDescriptor result(bus,device,function);

    result.vendor_id=Read(bus,device,function,0x00);
    result.device_id=Read(bus,device,function,0x02);

    result.class_id=Read(bus,device,function,0x0b);
    result.subclass_id=Read(bus,device,function,0x0a);
    result.interface_id=Read(bus,device,function,0x09);

    result.revision=Read(bus,device,function,0x08);
    result.interrupt=Read(bus,device,function,0x3c);

    return result;
}

BaseAddressRegister PeripheralComponentconnectController::GetBaseAddressRegister(uint16_t bus,uint16_t device,uint16_t function,uint16_t bar){
BaseAddressRegister result;

uint32_t headertype=Read(bus,device,function,0x0E)&(0x7F);
// Type0 header use 6 base address registers(BAR)
// Type1 header use 2 base address registers(BAR)
int maxBARs=6-(4*headertype);
if(bar >= maxBARs)
return result;

uint32_t bar_value=Read(bus,device,function,0x10+4*bar); 
result.type=(bar_value&0x1)?InputOutput:MemoryMapping;
uint32_t temp;

if(result.type==MemoryMapping){
switch((bar_value >>1) &0x3){
case 0:// 32 Bit Mode
case 1:// 20 Bit Mode
case 2:// 64 Bit Mode

break;
}
}else{
result.address=(uint8_t*)(bar_value & ~0x3); //the two least siginficant bits is cancelled so the address is multibule of 4
result.prefetchable=false;
}

return result;
}


Driver* PeripheralComponentconnectController:: GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager*interrupts){
    
    Driver* driver=0;
    switch(dev.vendor_id){
        case 0x1022: //AMD
        switch(dev.device_id){
            case 0x2000: //am79c973
                printf("AMD am79c973");
                driver=(amd_am79c973*)MemoryManager::activeMemoryManager->malloc(sizeof(amd_am79c973));
                if(driver!=0)
                    new(driver) amd_am79c973(&dev,interrupts);
                else
                    printf("instantiation failed");
                return driver; 
                break;
            }
            break;

            case 0x8086: //Intel
            break;
    }

    switch(dev.class_id){
        case 0x03: //graphics
            switch(dev.subclass_id){
                case 0x00: //VGA
                    printf("VGA");
                    break;
            }

            break;
    }
    
    return driver;
}