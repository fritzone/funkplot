#include "nethelper.h"

#include <iostream>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ostream>
#include <unistd.h>

using namespace nethelper;

socket_wrapper::socket_wrapper(int domain, int type, int protocol)
{
    m_sd = ::socket(domain, type, protocol);
    if(m_sd == -1)
    {
        invalidate();
    }
}

socket_wrapper::~socket_wrapper()
{
    if(m_sd != -1)
    {
        close(m_sd);
    }
}

bool socket_wrapper::operator <(int v)
{
    return m_sd < v;
}

socket_wrapper::operator int()
{
    return m_sd;
}

void socket_wrapper::invalidate()
{
    m_valid = false;
}

bool socket_wrapper::is_valid() const
{
    return m_valid;
}

int nethelper::pingChecksum(unsigned short *buf, int sz)
{
    int nleft = sz;
    int sum = 0;
    unsigned short *w = buf;
    unsigned short ans = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *reinterpret_cast<unsigned char *> (&ans) = *reinterpret_cast<unsigned char *>(w);
        sum += ans;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    ans = ~sum;
    return (ans);
}

bool nethelper::ping(const std::string &host)
{
    static const int DEFDATALEN = 56;
    static const int MAXIPLEN = 60;
    static const int MAXICMPLEN = 76;

    sockaddr_in pingaddr = {0, 0, {0}, {0} };
    icmp *pkt = 0;
    int c = 0;
    char packet[DEFDATALEN + MAXIPLEN + MAXICMPLEN] = {0};

    socket_wrapper sock(AF_INET, SOCK_RAW, 1);

    if (sock < 0 || !sock.is_valid())
    {
        std::cerr << "Invalid socket: " << sock << std::endl;
        return false;
    }

    inet_aton(host.c_str(), &pingaddr.sin_addr);
    pingaddr.sin_family = AF_INET;

    pkt = reinterpret_cast<icmp*>(packet);
    pkt->icmp_type = ICMP_ECHO;
    pkt->icmp_cksum = pingChecksum(reinterpret_cast<unsigned short*>(pkt), sizeof(packet));

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int serr = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (serr < 0)
    {
        std::cerr << "serr < 0: sock" << sock << std::endl;

        return false;
    }

    c = sendto(sock, packet, sizeof(packet), 0, reinterpret_cast<sockaddr*>(&pingaddr), sizeof(sockaddr_in));

    if (c < 0 || c != sizeof(packet))
    {
        std::cerr << "c < 0: sock" << sock << std::endl;

        return false;
    }

    sockaddr_in from;
    socklen_t fromlen = static_cast<socklen_t>( sizeof(from) );

    if ((c = recvfrom(sock, packet, sizeof(packet), 0, reinterpret_cast<sockaddr*>(&from), &fromlen)) < 0)
    {
        std::cerr << "recvrfrom failed < 0: c=" << c << " sock="  << sock << std::endl;

        return false;
    }
    if (c >= MAXICMPLEN)
    {
        iphdr *ip_header = reinterpret_cast<iphdr*>(packet);
        pkt = reinterpret_cast<icmp*>(packet + (ip_header->ihl << 2));
        if (pkt->icmp_type == ICMP_ECHOREPLY)
        {
            return true;
        }
    }

    std::cerr << "c>MAXIPLEN c=" << c << " sock="  << sock << std::endl;

    return false;
}
