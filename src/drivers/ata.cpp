#include <G:\MyOS\include\drivers\ata.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;


void printf(char* str);
void printfHex(uint8_t);

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(bool master , uint16_t portbase)
:dataPort(portbase),
errorPort(portbase + 0x1),
sectorCountPort(portbase + 0x2),
lbaLowPort(portbase + 0x3),
lbaMidPort(portbase + 0x4),
lbaHighPort(portbase + 0x5),
devicePort(portbase + 0x6),
commandPort(portbase + 0x7),
controlPort(portbase + 0x206)
{
    this->master=master;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment(){

}

void AdvancedTechnologyAttachment::Identify(){
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0);

    //status always be read from master 
    devicePort.Write(0xA0);
    uint8_t status = commandPort.Read();

    if(status == 0xFF);
        //there is no device connected to the bus
        return;

    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHighPort.Write(0);
    commandPort.Write(0xEC); // identify command

    status = commandPort.Read();
    if(status == 0x00)
        return;

    //There is some delay before hard drive is ready
    while(((status & 0x80) == 0x80)
    &&((status & 0x01) != 0x01)) 
        status =commandPort.Read();

        if(status & 0x01){
        printf("ERROR");
        return;
    } 

    for(int i=0 ; i < 256 ; i++){
        uint16_t data = dataPort.Read();
        char* text=" \0";
        text[0]=(data>>8) & 0xFF;
        text[1]=(data) & 0xFF;
        printf(text);
    }

    printf("\n");
}


void AdvancedTechnologyAttachment::Read28(uint32_t sectorNum , int count){
    if(sectorNum > 0x0FFFFFFF)
        return; //the sector number is 28 bits so the maximum sector number is 0x0FFFFFFF

    // E0 is used to write to master
    //F0 is used to write to slave
    //E0 or F0 is written to device port with
    //4 rightmost significant bits of sectorNUm
    devicePort.Write(master ? 0xE0 : 0xF0 | ((sectorNum & 0x0F000000) >> 24)); 
    errorPort.Write(0);
    sectorCountPort.Write(1);
    lbaLowPort.Write(sectorNum & 0x000000FF); 
    lbaMidPort.Write((sectorNum & 0x0000FF00) >> 8);
    lbaHighPort.Write((sectorNum & 0x00FF0000) >> 16);
    commandPort.Write(0x20);

    uint8_t status = commandPort.Read();
    //There is some delay before hard drive is ready
    while(((status & 0x80) == 0x80)
    &&((status & 0x01) != 0x01)) 
        status =commandPort.Read(); 

    if(status & 0x01){
        printf("ERROR");
        return;
    } 

    printf("Reading ATA Drive: ");

    for(int i=0; i < count ; i+=2){
        uint16_t wdata = dataPort.Read();

        char* text = " \0";
        text[0]=wdata & 0xFF;

        if(i+1 < count)
        text[1]=(wdata >> 8) & 0xFF;
        else
        text[1]='\0';

        printf(text); 
    } 

    //The sector must be read internally so it not throw an error
    // if the count is odd it will be read arleady so we will escape it to the next one

    for(int i = count + (count%2) ; i < 512; i+=2)
        dataPort.Read(); 

}


void AdvancedTechnologyAttachment::Write28(uint32_t sectorNum , uint8_t* data , uint32_t count){
    if(sectorNum > 0x0FFFFFFF)
        return;

    if(count > 512)
        return; 

    devicePort.Write(master ? 0xE0 : 0xF0 | ((sectorNum & 0x0F000000) >> 24)); 
    errorPort.Write(0);
    sectorCountPort.Write(1);
    lbaLowPort.Write(sectorNum & 0x000000FF); 
    lbaMidPort.Write((sectorNum & 0x0000FF00) >> 8);
    lbaHighPort.Write((sectorNum & 0x00FF0000) >> 16);
    commandPort.Write(0x30); 

    printf("Writing to ATA Drive: ");

    for(int i=0; i < count; i+=2){
        uint16_t wdata = data[i];
        if(i+1 < count)
        wdata |= ((uint16_t)data[i+1]) << 8;
        dataPort.Write(wdata);

        char* text = " \0";
        text[0]=(wdata >> 8) & 0xFF;
        text[1]=wdata & 0xFF;
        printf(text);

    }

    for(int i = count +(count%2); i < 512 ; i+=2)
        dataPort.Write(0x0000);


}

void AdvancedTechnologyAttachment::Flush(){
    devicePort.Write(master ? 0xE0 : 0xF0);
    commandPort.Write(0xE7); //command for flush

    uint8_t status = commandPort.Read();
    if(status == 0x00)
        return;

    //There is some delay before hard drive is flushing
    while(((status & 0x80) == 0x80)
    &&((status & 0x01) != 0x01)) 
        status =commandPort.Read();

    if(status & 0x01){
        printf("ERROR");
        return;
    } 



}