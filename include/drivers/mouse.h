#ifndef __MYOS__DRIVERS__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H


#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\drivers\driver.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>

namespace myos
{
    namespace drivers
    {
        class MouseEventHandler
        {

            public:
                MouseEventHandler();
                virtual void OnActivate();
                virtual void OnMouseDown(myos::common::uint8_t button);
                virtual void OnMouseUp(myos::common::uint8_t button);
                virtual void OnMouseMove(myos::common::int8_t x, myos::common::int8_t y);
       };

       class MouseDriver : public myos::hardwarecommunication::InterruptHandler, public Driver
            {

                MouseEventHandler *handler;
                myos::hardwarecommunication::Port8bit dataport;
                myos::hardwarecommunication::Port8bit commandport;
                myos::common::uint8_t buffer[3]; // Whenever a mouse move or any of its buttons is pressed a three bytes is send to proccessor buffer is used to store theses bits
                myos::common::uint8_t offset;
                myos::common::uint8_t buttons; // store last state of button

                public:
                    MouseDriver(myos::hardwarecommunication::InterruptManager *manager, MouseEventHandler *handler);
                    ~MouseDriver();
                    virtual myos::common::uint32_t HandleInterrupt( myos::common::uint32_t esp);
                    virtual void Activate();
        };
    }
}
#endif