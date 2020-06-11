#include <iostream>
#include <sys/socket.h>
#include <linux/can.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <vector>
#include <unistd.h>

#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <linux/can/raw.h>

#define FULL_PAYLOAD 9

void consume_arguments(int argc, char *argv[]);
void set_up();
void client();
void server();

// aux
int mode;
int Client = 0;
int Server = 1;
int ret;


// socket
bool is_socket_available;
int32_t socket_descriptor;

// can bus
struct ifreq ifr;
std::string can_bus;

// can channel
int Rx, Tx;
struct sockaddr_can can_addr;

// can frame
struct can_frame frame;
int socket_length = sizeof(can_addr);


int main(int argc, char *argv[])
{
    consume_arguments(argc, argv);

    set_up();

    if(is_socket_available)
    {
        if(mode == Client)
        {
            client();
        }

        if(mode == Server)
        {
            server();            
        }
    }
}


void consume_arguments(int argc, char *argv[])
{
    if(argc >= 2)
    {
        argc >= 2 ? mode = atoi(argv[1]) : mode = 0;
        argc >= 3 ? can_bus = argv[2] : can_bus = "vcan0";
        if(argc == 5 )
        {
            Rx = atoi(argv[3]);
            Tx = atoi(argv[4]);
        }else
        {
            Rx = 100;
            Tx = 101;
        }
        
    } else
    {
        printf("Usage:\n");
        printf("  %s <mode>\n", argv[0]);
        printf("  %s <mode> <canBus>\n", argv[0]);
        printf("  %s <mode> <canBus> <Rx> <Tx>\n\n", argv[0]);
        printf("  mode   : 0: client 1: Server\n");
        printf("  canBus : can0, can1, vcan0, vcan1, ...\n");
        printf("  Rx     : 100\n");
        printf("  Tx     : 101\n");
        exit(0);
    }
}

void set_up()
{
    std::cout << "  Setting up socket: ";
    ret = (socket_descriptor = socket(PF_CAN, SOCK_DGRAM, CAN_ISOTP));
    is_socket_available = ret >= 0;
    std::cout << ret << "\n";

    if(is_socket_available)
    {
        // can bus
        strcpy(ifr.ifr_name, can_bus.data());

        ioctl(socket_descriptor, SIOCGIFINDEX, &ifr);

        // can channel
        can_addr.can_family  = AF_CAN;
        can_addr.can_ifindex = ifr.ifr_ifindex;
        can_addr.can_addr.tp.rx_id = Rx;
        can_addr.can_addr.tp.tx_id = Tx;

        ioctl(socket_descriptor, SIOCGIFINDEX, &ifr);

        std::cout << "  Binding to socket: ";
        ret = bind(socket_descriptor, (struct sockaddr *)&can_addr, sizeof(can_addr));
        is_socket_available = ret >= 0;
        std::cout << ret << "\n";
    }

    std::cout << "  Availability     : " << is_socket_available << "\n\n";
}

void client()
{
    std::cout << "Starting can TP Client.\n";
    std::vector<uint8_t> data = //"This is a big udp payload message just to test can ISO tp protocol ISO-15765-2."
    {
    0x41, 0x20, 0x73, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x73, 0x74, 0x72, 0x69, 
    0x6e, 0x67, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x49, 0x53,
    0x4f, 0x20, 0x31, 0x35, 0x37, 0x36, 0x35, 0x2d, 0x32, 0x20, 0x63, 0x61, 0x6e, 
    0x20, 0x74, 0x70, 0x20, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x63, 0x6f, 0x6c, 0x2e
    };
    
    struct can_frame frame;
    std::copy(data.begin(), data.end(), std::begin(frame.data));
    ret = write(socket_descriptor, &frame.data, data.size());
}

void server()
{
    std::cout << "Starting can TP Server.\n";
    struct can_frame frame;
    int socket_length = sizeof(can_addr);


    // ret = recvfrom
    ret = recvfrom(socket_descriptor, &frame, sizeof(frame), 0, (sockaddr*)&can_addr, (socklen_t*)&socket_length);
    
    std::cout << "Received Can Data id: " + std::to_string(frame.can_id);
    std::vector<uint8_t> data(FULL_PAYLOAD);
                    
    auto it = data.begin();
    *it = frame.can_id;
    ++it;
    std::copy(std::begin(frame.data), std::end(frame.data), it);

    
    // print ret
    std::cout << ret;
    

    // print da mensagem
    std::cout << "receive: " + std::to_string(frame.can_id);
}
