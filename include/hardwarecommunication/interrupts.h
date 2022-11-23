#ifndef __MYOS__HARDWARECOMMUNICATION__INTERRUPTS_H
#define __MYOS__HARDWARECOMMUNICATION__INTERRUPTS_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\gdt.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>
#include <G:\MyOS\include\multitasking.h>

namespace myos{
    namespace hardwarecommunication{

        class InterruptManager;
        class InterruptHandler{
            protected:
                myos::common:: uint8_t interruptNumber;
                InterruptManager* interruptManager;
                InterruptHandler(myos::common:: uint8_t interruptNumber ,InterruptManager* interruptManager);
                ~InterruptHandler();
                public:
                virtual myos::common:: uint32_t HandleInterrupt(myos::common:: uint32_t esp);

            };
            class InterruptManager{
                friend class InterruptHandler;
                protected:
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];
                TaskManager* taskmanager;
                struct GateDescriptor
                {
                    myos::common:: uint16_t handlerAddressLowbits;
                    myos::common:: uint16_t gdt_CodeSegmentSelector ;
                    myos::common:: uint8_t Reserved;
                    myos::common:: uint8_t access;
                    myos::common:: uint16_t handlerAddressHighbits;


                }__attribute__((packed));


                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer
                {
                    myos::common:: uint16_t size;
                    myos::common:: uint32_t base;
                }__attribute__((packed));

                myos::common::uint16_t hardwareInterruptOffset;
                static void SetInterruptDescriptorTableEntry(
                myos::common:: uint8_t interruptNumber,
                myos::common:: uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                myos::common:: uint8_t DescriptorPrivilegelevel,
                myos::common:: uint8_t DescriptorType
                );

                static void InterruptIgnore(); // Every interrupt that not be set explicity must be ignored so we donot get global protection fault when interrupt has no handler
                static void HandleInterruptRequest0x00(); //Timer interrupt
                static void HandleInterruptRequest0x01();// Keyboard interrupt
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C(); // Mouse Interrupt
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();
                static void HandleInterruptRequest0x80();

                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();

                static myos::common:: uint32_t handleInterrupt(myos::common:: uint8_t interruptNumber ,myos::common:: uint32_t esp); //esp is current stack pointer that assembler gives us
                myos::common:: uint32_t DohandleInterrupt(myos::common:: uint8_t interruptNumber ,myos::common:: uint32_t esp);

                Port8bitslow picMasterCommand;
                Port8bitslow picMasterData;
                Port8bitslow picSlaveCommand;
                Port8bitslow picSlaveData;


            public:
                InterruptManager(GlobalDescriptorTable* gdt,myos::TaskManager* taskmanager);
                ~InterruptManager();
                myos::common::uint16_t HardwareInterruptOffset();
                void Activate();
                void Deactivate();




        };

    }
}

#endif