#ifndef __MYOS__DRIVERS_ATA_H
#define __MYOS__DRIVERS_ATA_H


#include <G:\MyOS\include\common\types.h>
#include <G:\MyOS\include\hardwarecommunication\interrupts.h>
#include <G:\MyOS\include\hardwarecommunication\port.h>

namespace myos{
    namespace drivers{

        class AdvancedTechnologyAttachment{
            protected:
                bool master;
                hardwarecommunication::Port16bit dataPort;
                hardwarecommunication::Port8bit errorPort;
                hardwarecommunication::Port8bit sectorCountPort;
                //Three ports used to translate Logic Block Address for sector we want to read
                //int cylindical head address(CHA)
                hardwarecommunication::Port8bit lbaLowPort;
                hardwarecommunication::Port8bit lbaMidPort;
                hardwarecommunication::Port8bit lbaHighPort;
                hardwarecommunication::Port8bit devicePort;
                hardwarecommunication::Port8bit commandPort;
                hardwarecommunication::Port8bit controlPort;
            public:
                AdvancedTechnologyAttachment(bool master , myos::common::uint16_t portbase );
                ~AdvancedTechnologyAttachment();

                void Identify();
                void Read28(myos::common::uint32_t sectorNUm , int count = 512);
                void Write28(myos::common::uint32_t sectorNUm , myos::common::uint8_t* data , myos::common::uint32_t count);
                //When the data is written to harddrive it stored first in cache
                //In order to write the data in specific sector the cache must be
                //FLushed or the sector will be invisable to write or read 
                void Flush();

        };
    }
}

#endif