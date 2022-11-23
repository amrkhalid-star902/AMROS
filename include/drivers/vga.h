#ifndef __MYOS__DRIVERS__VGA_H
#define __MYOS__DRIVERS__VGA_H

#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\drivers\driver.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>

namespace myos{
    namespace drivers{
        class VideoGraphicsArray{
            protected:
                hardwarecommunication::Port8bit miscPort;
                hardwarecommunication::Port8bit crtcIndexPort;
                hardwarecommunication::Port8bit crtcDataPort;
                hardwarecommunication::Port8bit sequencerIndexPort;
                hardwarecommunication::Port8bit sequencerDataPort;
                hardwarecommunication::Port8bit graphicsControllerIndexPort;
                hardwarecommunication::Port8bit graphicsControllerDataPort;
                hardwarecommunication::Port8bit attributeControllerIndexPort;
                hardwarecommunication::Port8bit attributeControllerReadPort;
                hardwarecommunication::Port8bit attributeControllerWritePort;
                hardwarecommunication::Port8bit attributeCOntrollerResetPort;
                void WriteRegisters(myos::common::uint8_t* registers);
                myos::common::uint8_t* GetFrameBufferSegment(); //gives us correct offset for segment we want to use
                virtual myos::common::uint8_t GetColorIndex( myos::common::uint8_t r ,myos::common::uint8_t g , myos::common::uint8_t b);

                public:
                    VideoGraphicsArray();
                    ~VideoGraphicsArray();

                    virtual bool SupportsMode(myos::common::uint32_t width , myos::common::uint32_t height , myos::common::uint32_t colordepth);
                    virtual bool SetMode(myos::common::uint32_t width , myos::common::uint32_t height , myos::common::uint32_t colordepth);
                    virtual void PutPixel(myos::common::int32_t x , myos::common::int32_t y, myos::common::uint8_t r ,myos::common::uint8_t g , myos::common::uint8_t b);
                    virtual void PutPixel(myos::common::int32_t x , myos::common::int32_t y, myos::common::uint8_t colorIndex);
                    virtual void FillRectangle(myos::common::uint32_t x , myos::common::uint32_t y ,myos::common::uint32_t w ,myos::common::uint32_t h ,myos::common::uint8_t r ,myos::common::uint8_t g , myos::common::uint8_t b);
            };
    }
} 




#endif