#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;
void printf(char *str);
void printfHex(uint8_t key);

InterruptHandler::InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager){
    this->interruptNumber=interruptNumber;
    this->interruptManager=interruptManager;
    interruptManager->handlers[interruptNumber]=this;
}

InterruptHandler::~InterruptHandler(){
    if(interruptManager->handlers[interruptNumber] == this)
    interruptManager->handlers[interruptNumber]=0;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp){
    return esp;
}

InterruptManager :: GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager ::ActiveInterruptManager=0;

void InterruptManager::SetInterruptDescriptorTableEntry(
uint8_t interruptNumber,
uint16_t codeSegmentSelectorOffset,
void (*handler)(),
uint8_t DescriptorPrivilegelevel,
uint8_t DescriptorType
)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowbits=((uint32_t) handler)&0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighbits=((uint32_t) handler >> 16)&0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_CodeSegmentSelector=codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access=IDT_DESC_PRESENT|DescriptorType|((DescriptorPrivilegelevel&3) <<5);
    interruptDescriptorTable[interruptNumber].Reserved=0;

}


InterruptManager::InterruptManager(GlobalDescriptorTable *gdt , TaskManager* taskmanager)
:picMasterCommand(0x20),
picMasterData(0x21),
picSlaveCommand(0xA0),
picSlaveData(0xA1)
{
    this->taskmanager=taskmanager;
    uint16_t CodeSegment=gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE=0xE;
    for(uint16_t i=0;i <256;i++){
        handlers[i]=0;
        SetInterruptDescriptorTableEntry(i,CodeSegment,&InterruptIgnore,0,IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20,CodeSegment,&HandleInterruptRequest0x00,0,IDT_INTERRUPT_GATE); // the interrupt nymber is 20 because interrupt base in interruptstubs.s is 0x20 so interrupt number equal 0x20+0 =0x20
    SetInterruptDescriptorTableEntry(0x21,CodeSegment,&HandleInterruptRequest0x01,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x2C,CodeSegment,&HandleInterruptRequest0x0C,0,IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(0x20); //add 0x20 to interrupt numbers coming from master
    picSlaveData.Write(0x28); //add 0x28 to interrupt numbers coming from slave
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02); 

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);
    InterruptDescriptorTablePointer idt;
    idt.size=256*sizeof(GateDescriptor)-1;
    idt.base=(uint32_t) interruptDescriptorTable;
    asm volatile("lidt %0": :"m"(idt));

}

InterruptManager::~InterruptManager()
{

}

void InterruptManager::Activate()
{
    if(ActiveInterruptManager !=0)
    ActiveInterruptManager->Deactivate();
    ActiveInterruptManager=this;
    asm ("sti"); // The instruction to start interrupt is in seprate function so in kernel.cpp the hardware is initialized first then interrupts is activated
}

void InterruptManager::Deactivate()
{
    if(ActiveInterruptManager == this){
        ActiveInterruptManager=0;
        asm("cli");
    }
}


uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber ,uint32_t esp){
    if(ActiveInterruptManager !=0)
        return ActiveInterruptManager->DohandleInterrupt(interruptNumber,esp);
    return esp; 
}

uint32_t InterruptManager::DohandleInterrupt(uint8_t interruptNumber,uint32_t esp){
    if(ActiveInterruptManager !=0){
        esp =handlers[interruptNumber]->HandleInterrupt(esp);
    }

    else if(interruptNumber !=0x20){
        printf("UNHANDLED INTERRUPT 0X");
        printfHex(interruptNumber);

    }

    if(interruptNumber == 0x20){
        esp=(uint32_t)taskmanager->Schedule((CPUState*)esp);
    }


    if(interruptNumber >=0x20 && interruptNumber <0x30) //Hardware interruptts are mapped between 20 and 30
    {
        picMasterCommand.Write(0x20); 
        if(interruptNumber >= 0x28) // slave interrupt located between 28 and 30
        picSlaveCommand.Write(0x20);
    }
    return esp;
}