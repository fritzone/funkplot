#ifndef NETHELPER_H
#define NETHELPER_H

#include <string>

namespace nethelper
{

/**
 * @brief The socket_wrapper class is a simple RAII wrapper for a socket
 */
class socket_wrapper
{
public:

    /**
     * @brief socket_wrapper creates a new socket wrapper object, by ensuring a call to ::socket
     *        with the given parameters
     *
     * @param domain the domain of the socket (AF_INET, ...)
     * @param type the type of the socket (SOCK_STREAM, etc...)
     * @param protocol the protocol of the socket
     */
    socket_wrapper(int domain, int type, int protocol);

    /**
     * @brief ~socket_wrapper closes the RAII socket upon destruction
     */
    virtual ~socket_wrapper();

    /**
     * @brief operator < just to make the API compatible with data returned by ::socket, ie. comparing
     *        the return value with the given number to check for errors
     * @param v the value to compare it
     * @return true/false
     */
    bool operator < (int v);

    /**
     * @brief operator int just to make the API compatible with data returned by ::socket, ie. handline
     *        this object as an integer value
     */
    operator int ();

    /**
     * @brief invalidate invalidates the socket
     */
    void invalidate();

    /**
     * @brief is_valid returns the state of the socket
     * @return true/false
     */
    bool is_valid() const;

private:

    bool m_valid = true;
    int m_sd = -1;
};

/**
 * @brief pingChecksum Calculates the checksum of the ICMP packet (RFC 1071)
 * @param buf - the buffer to calculate
 * @param sz - the size of it
 * @return the ICMP checksum of the packet
 */
int pingChecksum(unsigned short *buf, int sz);

/**
 * @brief ping pings the given host, returns true if it succeeded, false otherwise
 * @param host the host to ping
 * @return true if the host is pingable, false otherwise
 */
bool ping(const std::string &host);

}

#endif // NETHELPER_H
