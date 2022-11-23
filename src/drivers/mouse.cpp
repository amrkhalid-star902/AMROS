#include <G:\MyOS\include\drivers\mouse.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


void printf(char* str);

MouseEventHandler::MouseEventHandler(){

}

void MouseEventHandler::OnActivate(){

}

void MouseEventHandler::OnMouseDown(uint8_t button){

}

void MouseEventHandler::OnMouseUp(uint8_t button){

}

void MouseEventHandler::OnMouseMove(int8_t x,int8_t y){

}

MouseDriver::MouseDriver(InterruptManager* manager,MouseEventHandler* handler)
:InterruptHandler(0x2C,manager),
dataport(0x60),
commandport(0x64)
{
    this->handler=handler;

}



void MouseDriver::Activate(){
    offset=0;
    buttons=0;


    commandport.Write(0xA8); // Enable Keyboard
    commandport.Write(0x20); //get current state 
    uint8_t status =(dataport.Read()|2);
    commandport.Write(0x60); 
    dataport.Write(status);
    commandport.Write(0xD4);
    dataport.Write(0xF4); //The mouse start sending automatic packets when it is clicked or moved
    dataport.Read();
}


MouseDriver::~MouseDriver(){

}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){

    uint8_t status=commandport.Read();
    if(!(status & 0x20) ) // if only the 6th bit in status equal to 1 then there is a data to read
        return esp;
    buffer[offset++]=dataport.Read();

    if(handler==0)
        return esp;

    offset=offset%3;

    //buffer[1] contain movement in x-axis
    //buffer[2] contain movement in y-axis but in opposite direction
    if(offset == 0) //statrt of new movement
    {

        if(buffer[1] != 0 || buffer[2] !=0){
        handler->OnMouseMove((int8_t)buffer[1],-((int8_t)buffer[2]));
    }

    //check the state of button is it changed or not by comparing it to last reading 0 position for left pin ,(1) for right pin ,(2) for middle pin 
    for(uint8_t i=0 ;i<3;i++){
        if(buffer[0] &(0x01 <<i) !=(buttons & (0x01 << i))){
            if(buttons &(0x1<<i))
                handler->OnMouseUp(i+1); //Event happens when Mouse button is released
            else
                handler->OnMouseDown(i+1); // Event happens when Mouse button is pressed 
        }
    } 

    buttons=buffer[0]; 

    }


    return esp;
}