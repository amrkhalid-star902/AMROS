#include <G:\MyOS\include\net\icmp.h>

using namespace myos;
using namespace myos::common;
using namespace myos::net;


InernetControlMessageProtocol::InernetControlMessageProtocol(InternetProtocolProvider* backend)
: InternetProtocolHandler(backend , 0x01)
{

}

InernetControlMessageProtocol::~InernetControlMessageProtocol(){

}

void printf(char*);
void printfHex(uint8_t);

bool InernetControlMessageProtocol::OnInternetProtocolReceived(uint32_t srcIP_BE , uint32_t dstIP_BE ,
uint8_t* internetprotocolPayload , uint32_t size){

    if(size < sizeof(InternetControlMessageProtocolMessage))
        return false;

    InternetControlMessageProtocolMessage* msg = (InternetControlMessageProtocolMessage*)internetprotocolPayload;

    switch(msg->type){
        case 0:
            printf("Ping response from"); printfHex(srcIP_BE & 0xFF);
            printf("."); printfHex((srcIP_BE >> 8) & 0xFF);
            printf("."); printfHex((srcIP_BE >> 16) & 0xFF);
            printf("."); printfHex((srcIP_BE >> 24) & 0xFF);
            printf("\n");
            break;

        case 8:
            msg->type=0;
            msg->checksum=0;
            msg->checksum=InternetProtocolProvider::Checksum((uint16_t*)msg,
            sizeof(InternetControlMessageProtocolMessage));
            return true; 
    }

    return false;
}

void InernetControlMessageProtocol::RequestEchoReply(uint32_t ip_be){
    InternetControlMessageProtocolMessage icmp;
    icmp.type=8; //ping
    icmp.code=0;
    icmp.data = 0x1212;
    icmp.checksum = 0;
    icmp.checksum = InternetProtocolProvider::Checksum((uint16_t*)&icmp , 
    sizeof(InernetControlMessageProtocol));

    InternetProtocolHandler::Send(ip_be , (uint8_t*)&icmp , sizeof(InternetControlMessageProtocolMessage)); 
}