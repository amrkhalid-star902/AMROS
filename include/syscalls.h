#ifndef __MYOS__SYSCALLS_H
#define __MYOS__SYSCALLS_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\multitasking.h>

namespace myos{
    class SyscallHandler : public hardwarecommunication::InterruptHandler{
        public:
            SyscallHandler(hardwarecommunication::InterruptManager* interruptManager , myos::common::uint8_t InterruptNumber);
            ~SyscallHandler();

            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
    };
}


#endif