#include <G:\MyOS\include\drivers\amd_am79c973.h>
using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


void printf(char*);
void printfHex(uint8_t);


RawDataHandler::RawDataHandler(amd_am79c973* backend){
    this->backend=backend;
    backend->SetHandler(this);
}

RawDataHandler::~RawDataHandler(){
    backend->SetHandler(0);
}

bool RawDataHandler::OnRawDataReceived(uint8_t* buffer , uint32_t size){
    return false;
}

void RawDataHandler::Send(uint8_t* buffer , uint32_t size){
    backend->Send(buffer,size);
}


amd_am79c973::amd_am79c973(PeripheralComponentInterconnectDeviceDescriptor* dev , InterruptManager* interrupts)
:Driver(),
InterruptHandler( dev->interrupt +0x20, interrupts ),
MACAdress0Port(dev->portbase),
MACAdress2Port(dev->portbase + 0x02),
MACAdress4Port(dev->portbase + 0x04),
registerDataPort(dev->portbase + 0x10),
registerAddressPort(dev->portbase + 0x12),
resetPort(dev->portbase + 0x14),
busControlRegisterDataPort(dev->portbase + 0x16)
{

    currentSendBuffer=0;
    currentrecvBuffer=0;

    uint64_t MAC0 = MACAdress0Port.Read()%256;
    uint64_t MAC1 = MACAdress0Port.Read()/256;
    uint64_t MAC2 = MACAdress2Port.Read()%256;
    uint64_t MAC3 = MACAdress2Port.Read()/256;
    uint64_t MAC4 = MACAdress4Port.Read()%256;
    uint64_t MAC5 = MACAdress4Port.Read()/256;

    uint64_t MAC = MAC5 << 40
    | MAC4 << 32
    | MAC3 << 24
    | MAC2 << 16
    | MAC1 << 8
    | MAC0;



    //32 bit mode
    registerAddressPort.Write(20);
    busControlRegisterDataPort.Write(0x102);

    //Stop reset
    registerAddressPort.Write(0);
    registerAddressPort.Write(0x04);

    //initBLock

    initBlock.mode=0x0000; // promiscuous mode = false 
    initBlock.reserved1=0;
    initBlock.numSendBuffers=3; // number of buffers equal 2^3 = 8
    initBlock.reserved2=0;
    initBlock.numRecvBuffers=3;
    initBlock.physicalAddress=MAC;
    initBlock.reserved3=0;
    initBlock.logicalAddress=0;

    sendBufferDescr=(BufferDescriptor*)((((uint32_t)&sendBufferDescrMemory[0])+15) & ~((uint32_t)0xF)); //15 bytes is added to make address 16 byte aligned 
    initBlock.sendBufferDescrAddress=(uint32_t)sendBufferDescr;
    recvBufferDescr=(BufferDescriptor*)((((uint32_t)&recvBufferDescrMemory[0])+15) & ~((uint32_t)0xF)); //15 bytes is added to make address 16 byte aligned 
    initBlock.recvBufferDescrAddress=(uint32_t)recvBufferDescr;

    for(uint8_t i = 0 ; i<8 ; i++){
        sendBufferDescr[i].address=((((uint32_t)&sendBuffers[i])+15) & ~((uint32_t)0xF));
        sendBufferDescr[i].flags=0x7FF|0xF000;

        sendBufferDescr[i].flags2=0;
        sendBufferDescr[i].avail=0;


        recvBufferDescr[i].address=((((uint32_t)&recvBuffers[i])+15) & ~((uint32_t)0xF));
        recvBufferDescr[i].flags=0xF7FF|0x80000000;

        recvBufferDescr[i].flags2=0;
        recvBufferDescr[i].avail=0;

    }

    registerAddressPort.Write(1);
    registerDataPort.Write((uint32_t)(&initBlock)&0xFFFF);
    registerAddressPort.Write(2);
    registerDataPort.Write(((uint32_t)(&initBlock)>>16)&0xFFFF);

} 



amd_am79c973::~amd_am79c973(){

}

void amd_am79c973::Activate(){

    registerAddressPort.Write(0);
    registerDataPort.Write(0x41);

    registerAddressPort.Write(4);
    uint32_t temp=registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp|0xC00);

    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);

}


int amd_am79c973::Reset(){
    resetPort.Read();
    resetPort.Write(0);
    return 10;
}

uint32_t amd_am79c973::HandleInterrupt(uint32_t esp){
    registerAddressPort.Write(0);
    uint32_t temp=registerDataPort.Read();

    if((temp & 0x8000) == 0x8000) printf("AMD am79c973 ERROR\n");
    if((temp & 0x2000) == 0x2000) printf("AMD am79c973 COLLISION ERROR\n");
    if((temp & 0x1000) == 0x1000) printf("AMD am79c973 MISSED FRAME\n");
    if((temp & 0x0800) == 0x0800) printf("AMD am79c973 MEMORY ERROR\n");
    if((temp & 0x0400) == 0x0400) Receive();
    if((temp & 0x0200) == 0x0200) printf("SENT");

    // The CSRO register content should be written back
    //to indicate to card that the interrupt has been processed
    registerAddressPort.Write(0);
    registerDataPort.Write(temp);

    if((temp & 0x0100) == 0x0100) printf("AMD am79c973 INIT DONE\n");

    return esp;
}


void amd_am79c973::Send(uint8_t* buffer , int size){
    int sendDescriptor=currentSendBuffer;
    currentSendBuffer=(currentSendBuffer+1)%8;

    if(size > 1518) //The MAX Size of Ethernet Backet is 1518
        size=1518;

    for(uint8_t *src = buffer + size -1,
    *dst = (uint8_t*)(sendBufferDescr[sendDescriptor].address + size -1);
    src >= buffer; src-- , dst--)
        *dst=*src; 

    printf("\nSEND: "); 
    for(int i=0; i < (size>64?64:size) ; i++){
        printfHex(buffer[i]);
        printf(" ");
    } 

    sendBufferDescr[sendDescriptor].avail=0; // The send buffer is not available when it have thing to send
    sendBufferDescr[sendDescriptor].flags2=0; //The flags2 is set only when error occur so it must be set to zero by driver 
    sendBufferDescr[sendDescriptor].flags=0x8300F000
    |((uint16_t)((-size) & 0xFFF));

    registerAddressPort.Write(0);
    registerDataPort.Write(0x48); //Command to send data 

}

void amd_am79c973::Receive(){
    printf("\nRECV: ");
    //When card proccess any data in descriptors
    //it sets the OWN bit in the flag into zero
    //OWN bit is the most significant bit in the flags
    //so if a packet is received the own bit of the flag
    //will be equal to 1
    //We loop until we hit descriptor with clear OWN bit
    for(;(recvBufferDescr[currentrecvBuffer].flags & 0x80000000) ==0;
    currentrecvBuffer=(currentrecvBuffer+1)%8){
        //The data should only be read if ERROR bit 0x40000000 in the 
        //flags is not set In addition the bits STP and ENP
        //(0x02000000) and (0x01000000) should not be set
        if(!(recvBufferDescr[currentrecvBuffer].flags & 0x40000000)
        && (recvBufferDescr[currentrecvBuffer].flags & 0x03000000) == 0x03000000){
            uint32_t size = recvBufferDescr[currentrecvBuffer].flags & 0xFFF;
            if(size > 64)   //remove checksum
                size -= 4;  //The size of Ethernet2 frame is 64
                            //If the size is greater than 64 we delete last 4 bits
                            //Because we donot interessted in checksum bits 

            uint8_t* buffer = (uint8_t*)(recvBufferDescr[currentrecvBuffer].address);

            for(int i=0; i < (size>64?64:size) ; i++){
                printfHex(buffer[i]);
                printf(" ");
            }

            if(handler != 0)
                if(handler->OnRawDataReceived(buffer , size))
                    Send(buffer , size);

        }

        recvBufferDescr[currentrecvBuffer].flags2=0;
        recvBufferDescr[currentrecvBuffer].flags=0x8000F7FF;
    }

}

void amd_am79c973::SetHandler(RawDataHandler* handler){
    this->handler;
}


uint64_t amd_am79c973::GetMACAddress(){
    return initBlock.physicalAddress;
}


void amd_am79c973::SetIPAddress(uint32_t ip){
    initBlock.logicalAddress=ip;
}

uint32_t amd_am79c973::GetIPAddress(){
    return initBlock.logicalAddress;
}