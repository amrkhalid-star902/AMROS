#include </home/amrkhalid/MyOS/include/common/types.h>
#include </home/amrkhalid/MyOS/include/hardwarecommunication/interrupts.h>
#include </home/amrkhalid/MyOS/include/memorymanagement.h>
#include </home/amrkhalid/MyOS/include/drivers/driver.h>
#include </home/amrkhalid/MyOS/include/hardwarecommunication/port.h>
#include </home/amrkhalid/MyOS/include/hardwarecommunication/pci.h>
#include </home/amrkhalid/MyOS/include/drivers/keyboard.h>
#include </home/amrkhalid/MyOS/include/drivers/mouse.h>
#include </home/amrkhalid/MyOS/include/drivers/vga.h>
#include </home/amrkhalid/MyOS/include/drivers/ata.h>
#include </home/amrkhalid/MyOS/include/gui/desktop.h>
#include </home/amrkhalid/MyOS/include/gui/widget.h>
#include </home/amrkhalid/MyOS/include/gui/window.h>
#include </home/amrkhalid/MyOS/include/multitasking.h>
#include </home/amrkhalid/MyOS/include/drivers/amd_am79c973.h>
#include </home/amrkhalid/MyOS/include/syscalls.h>
#include </home/amrkhalid/MyOS/include/net/arp.h>
#include </home/amrkhalid/MyOS/include/net/etherframe.h>
#include </home/amrkhalid/MyOS/include/net/ipv4.h>
#include </home/amrkhalid/MyOS/include/net/icmp.h>
#include </home/amrkhalid/MyOS/include/net/udp.h>
#include </home/amrkhalid/MyOS/include/net/tcp.h>

//#define GRAPHICSMODE

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;
using namespace myos::net;

void printf(char *str){
static uint16_t* videomemory =(uint16_t*) 0xb8000;
static uint8_t x=0; //X responsiable for column index
static uint8_t y=0; // Y is responisable for row index
// The screen is 80 charachters wide and 25 character deep
for(int i=0; str[i]!='\0';i++){
switch (str[i])
{
case '\n':
x=0;
y++;
break;
default:
videomemory[80*y+x]=(videomemory[80*y+x] & 0xFF00 )|str[i];
x++;
break;
}
if(x >=80){
x=0;
y++;
}

if(y >=25){
for(y=0; y <25 ;y++)
for(x=0; x <80 ;x++)
videomemory[80*y+x]=(videomemory[80*y+x] & 0xFF00 )|' ';
x=0;
y=0;
}
}
}

void printfHex(uint8_t key){
char* foo="00";
char* hex="0123456789ABCDEF";
foo[0]=hex[( key>> 4) & 0x0F];
foo[1]=hex[key & 0xF];
printf(foo);
}

class PrintKeyboardEventHandler: public KeyboardEventHandler{
public:
void OnKeyDown(char c){
char* foo=" ";
foo[0]=c;
printf(foo);
}
};


class MouseToConsole: public MouseEventHandler{
int8_t x,y;
public:

MouseToConsole(){
static uint16_t* videomemory =(uint16_t*) 0xb8000;
x=40;
y=12;
//Cursor is initialized to center
videomemory[80*y+x]=((videomemory[80*y+x]&0xF000) >> 4)
| ((videomemory[80*y+x]&0x0F00) << 4)
| ((videomemory[80*y+x]&0x00F0));
}

virtual void OnMouseMove(int Xoffset,int yoffset){
static uint16_t* videomemory =(uint16_t*) 0xb8000;
videomemory[80*y+x]=((videomemory[80*y+x]&0xF000) >> 4)
| ((videomemory[80*y+x]&0x0F00) << 4)
| ((videomemory[80*y+x]&0x00F0));
x+=Xoffset;
if(x <0) x=0;
if(x>=80) x=79;
y+=yoffset;

if(y <0) y=0;
if(y>=25) y=24;
// the first byte contain information about color of the charachet and if the first half byte is flipped with second half byte the color of charachter will be changed this is usefull in blinking the cursor 
videomemory[80*y+x]=((videomemory[80*y+x]&0xF000) >> 4)
| ((videomemory[80*y+x]&0x0F00) << 4)
| ((videomemory[80*y+x]&0x00F0));
}

};

class PrintfUDPHandler : public UserDatagramProtocolHandler{
public:
void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket , uint8_t* data , uint16_t size){
char* foo = " ";
for(int i = 0 ; i < size ; i++){
foo[0] = data[i];
printf(foo);
}
}
};


class PrintfTCPHandler : public TransmissionControlProtocolHandler{
public:
bool HandleTransmissionControlProtocolMessage(UserDatagramProtocolSocket* socket , uint8_t* data , uint16_t size){
char* foo = " ";
for(int i = 0 ; i < size ; i++){
foo[0] = data[i];
printf(foo);
}

if(size > 9
&& data[0] == 'G'
&& data[0] == 'E'
&& data[0] == 'T'
&& data[0] == ' '
&& data[0] == '/'
&& data[0] == ' '
&& data[0] == 'H'
&& data[0] == 'T'
&& data[0] == 'T'
&& data[0] == 'P'){
socket->Send((uint8_t*)"HTTP/1.1 200 OK\r\nServer: MYOS\r\nContent-Type: text/html\r\n\r\n<html><head><title>My Operating System<title></head><body><b>My Operating System</b> http://www.MYOS.com</body></html>\r\n" , 200);
socket->Disconnect();
}

return true;
}
};


void sysprintf(char* str){
asm("int $0x80" : : "a" (4), "b"(str));
}

void taskA(){
while (true)
{
sysprintf("A");
}
}

void taskB(){
while (true)
{
sysprintf("B");
}
}







typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors(){
for(constructor *i=&start_ctors; i!=&end_ctors;i++)
(*i)();
}




extern "C" void kernelmain(void * multiboot_structure,uint32_t magicnumber )
{
printf("Hello world!");
GlobalDescriptorTable gdt;

uint32_t* memupper=(uint32_t*)(((size_t)multiboot_structure)+8);
size_t heap=10*1024*1024;
MemoryManager memorymanager(heap ,(*memupper)*1024-heap-10*1024);
printf("heap: 0x");
printfHex((heap >> 24) & 0xFF);
printfHex((heap >> 16) & 0xFF);
printfHex((heap >> 8) & 0xFF);
printfHex((heap) & 0xFF);

void* allocated=memorymanager.malloc(1024);
printf("\nallocated: 0x");
printfHex(((size_t)allocated >> 24) & 0xFF);
printfHex(((size_t)allocated >> 16) & 0xFF);
printfHex(((size_t)allocated >> 8) & 0xFF);
printfHex(((size_t)allocated) & 0xFF);
printf("\n");

TaskManager taskManager;
/*
Task task1(&gdt , taskA);
Task task2(&gdt , taskB);
taskManager.AddTask(&task1);
taskManager.AddTask(&task2);
*/
InterruptManager interrupts(&gdt , &taskManager);
SyscallHandler syscalls(&interrupts, 0x80);
printf("Intitializing hardware stage 1\n");
#ifdef GRAPHICSMODE
Desktop desktop(320,200,0x00,0x00,0xA8);
#endif
DriverManager drvManager;

#ifdef GRAPHICSMODE
KeyboardDriver keyboard(&interrupts,&desktop);
#else
PrintKeyboardEventHandler kbhandler;
KeyboardDriver keyboard(&interrupts,&kbhandler); 
#endif
drvManager.AddDriver(&keyboard);

#ifdef GRAPHICSMODE
MouseDriver mouse(&interrupts,&desktop);
#else
MouseToConsole mousehandler;
MouseDriver mouse(&interrupts,&mousehandler);
#endif
drvManager.AddDriver(&mouse);

PeripheralComponentconnectController PCIController;
PCIController.SelectDriver(&drvManager,&interrupts);

VideoGraphicsArray vga;

printf("Intitializing hardware stage 2\n");
drvManager.ActivateAll();
printf("Intitializing hardware stage 3\n");
#ifdef GRAPHICSMODE
vga.SetMode(320,200,8);
Window win1(&desktop,10,10,20,20,0xA8,0x00,0x00);
desktop.AddChild(&win1);
Window win2(&desktop,40,15,30,30,0x00,0xA8,0x00);
desktop.AddChild(&win2);
#endif 
/*
// interrupt 14
AdvancedTechnologyAttachment ata0m(0x1F0 , true ); //Primary master located at port 0x1F0
printf("ATA Primary Master: ");
ata0m.Identify();

AdvancedTechnologyAttachment ata0s(0x1F0 , false ); //Primary Slave
printf("ATA Primary Slave: ");
ata0s.Identify();
// interrupt 15
AdvancedTechnologyAttachment ata1m(0x170 , true ); //Secondary master located at port 0x170
printf("ATA Secondary Master: ");
ata1m.Identify();

AdvancedTechnologyAttachment ata1s(0x170 , false ); //SecondarySlave
printf("ATA Primary Slave: ");
ata1s.Identify(); 
*/

amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);

// IP Address
uint8_t ip1=10 , ip2=0 , ip3=2 , ip4=15;
uint32_t ib_be = ((uint32_t)ip4 << 24)
| ((uint32_t)ip3 << 16) 
| ((uint32_t)ip2 << 8) 
| ((uint32_t)ip1);

eth0->SetIPAddress(ib_be);
EtherFrameProvider etherframe(eth0);
AddressResolutionProtocol arp(&etherframe);

// IP Address of default gateway
uint8_t gip1=10 , gip2=0 , gip3=2 , gip4=15;
uint32_t gib_be = ((uint32_t)gip4 << 24)
| ((uint32_t)gip3 << 16) 
| ((uint32_t)gip2 << 8) 
| ((uint32_t)gip1);


uint8_t subnet1=255 , subnet2=255 , subnet3=255 , subnet4=0;
uint32_t subnet_be = ((uint32_t)subnet4 << 24)
| ((uint32_t)subnet3 << 16)
| ((uint32_t)subnet2 << 8)
| ((uint32_t)subnet1); 

InternetProtocolProvider ipv4(&etherframe , &arp , gib_be , subnet_be); 

InernetControlMessageProtocol icmp(&ipv4); 
UserDatagramProtocolProvider udp(&ipv4);
TransmissionControlProtocolProvider tcp(&ipv4);

interrupts.Activate();
printf("\n\n\n\n");
//To make gateway know our Ip address
arp.BroadcastMACAddress(gib_be);

/*icmp.RequestEchoReply(gib_be);

PrintfUDPHandler udphandler;
UserDatagramProtocolSocket* udpsocket = udp.Connect(gib_be , 1234);
udp.Bind(udpsocket , &udphandler);
udpsocket->Send((uint8_t*)"Hello UDP!" , 10);*/

PrintfTCPHandler tcphandler;
TransmissionControlProtocolSocket* tcpsocket = tcp.Listen(1234);
tcp.Bind(tcpsocket , &tcphandler);




while(1){
#ifdef GRAPHICSMODE
desktop.Draw(&vga);
#endif
}
}