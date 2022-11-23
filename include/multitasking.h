#ifndef __MYOS_MULTITASKING_H
#define __MYOS_MULTITASKING_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\gdt.h>

namespace myos{
    struct CPUState
    {
        //These values are pushed by interruptstubs.s
        myos::common::uint32_t eax; //accumulator register
        myos::common::uint32_t ebx; //base register
        myos::common::uint32_t ecx; //counting register
        myos::common::uint32_t edx; //data register

        myos::common::uint32_t esi; //stack index
        myos::common::uint32_t edi; //data index
        myos::common::uint32_t ebp; //base pointer

        myos::common::uint32_t error;

        //These values are pushed by processor
        myos::common::uint32_t eip; //instruction pointer
        myos::common::uint32_t cs; //code segment
        myos::common::uint32_t eflags;
        myos::common::uint32_t esp; //stack pointer
        myos::common::uint32_t ss; //stack segment


    }__attribute__((packed));

        class Task{

        friend class TaskManager;
        private:
            myos::common::uint8_t stack[4096];
            CPUState* cpustate;

            public:
            Task(GlobalDescriptorTable* gdt , void entrypoint());
            ~Task(); 
    };

    class TaskManager{
        private:
            Task* tasks[256];
            int numTasks;
            int currentTask;
        public:
            TaskManager();
            ~TaskManager();
            bool AddTask(Task* task);
            CPUState* Schedule(CPUState* cpustate);
    };
}


#endif