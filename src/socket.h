#pragma once

#include <string>
#include <functional>
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define INVALID_SOCKET_VALUE INVALID_SOCKET
    #define SOCKET_ERROR_VALUE SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCKET_VALUE -1
    #define SOCKET_ERROR_VALUE -1
    #define closesocket close
#endif

namespace librats {

/**
 * UDP peer information
 */
struct Peer {
    std::string ip;
    uint16_t port;
    
    Peer() : port(0) {}
    Peer(const std::string& ip, uint16_t port) : ip(ip), port(port) {}
    
    bool operator==(const Peer& other) const {
        return ip == other.ip && port == other.port;
    }
    
    bool operator!=(const Peer& other) const {
        return !(*this == other);
    }
};

// Socket Library Initialization
/**
 * Initialize the socket library
 * @return true if successful, false otherwise
 */
bool init_socket_library();

/**
 * Cleanup the socket library
 */
void cleanup_socket_library();

// TCP Socket Functions
/**
 * Create a TCP client socket and connect to a server using dual stack (IPv6 with IPv4 fallback)
 * @param host The hostname or IP address to connect to
 * @param port The port number to connect to
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_client(const std::string& host, int port);

/**
 * Create a TCP client socket and connect to a server using IPv4 only
 * @param host The hostname or IP address to connect to
 * @param port The port number to connect to
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_client_v4(const std::string& host, int port);

/**
 * Create a TCP client socket and connect to a server using IPv6 only
 * @param host The hostname or IPv6 address to connect to
 * @param port The port number to connect to
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_client_v6(const std::string& host, int port);

/**
 * Create a TCP server socket and bind to a port using dual stack (IPv6 with IPv4 support)
 * @param port The port number to bind to
 * @param backlog The maximum number of pending connections
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_server(int port, int backlog = 5);

/**
 * Create a TCP server socket and bind to a port using IPv4 only
 * @param port The port number to bind to
 * @param backlog The maximum number of pending connections
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_server_v4(int port, int backlog = 5);

/**
 * Create a TCP server socket and bind to a port using IPv6 only
 * @param port The port number to bind to
 * @param backlog The maximum number of pending connections
 * @return Socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_tcp_server_v6(int port, int backlog = 5);

/**
 * Accept a client connection on a server socket
 * @param server_socket The server socket handle
 * @return Client socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t accept_client(socket_t server_socket);

/**
 * Get the peer address (IP:port) from a connected socket
 * @param socket The connected socket handle
 * @return Peer address string in format "IP:port", or empty string on error
 */
std::string get_peer_address(socket_t socket);

/**
 * Send data through a TCP socket
 * @param socket The socket handle
 * @param data The data to send
 * @return Number of bytes sent, or -1 on error
 */
int send_tcp_data(socket_t socket, const std::string& data);

/**
 * Receive data from a TCP socket
 * @param socket The socket handle
 * @param buffer_size Maximum number of bytes to receive
 * @return Received data as string, empty string on error
 */
std::string receive_tcp_data(socket_t socket, size_t buffer_size = 1024);

/**
 * Send a framed message with length prefix through a TCP socket
 * @param socket The socket handle
 * @param message The message to send
 * @return Total bytes sent (including length prefix), or -1 on error
 */
int send_tcp_message_framed(socket_t socket, const std::string& message);

/**
 * Receive exact number of bytes from a TCP socket (blocking until complete)
 * @param socket The socket handle
 * @param num_bytes Number of bytes to receive
 * @return Received data as string, empty string on error or connection close
 */
std::string receive_exact_bytes(socket_t socket, size_t num_bytes);

/**
 * Receive a complete framed message from a TCP socket
 * @param socket The socket handle
 * @return Complete message as string, empty string on error or connection close
 */
std::string receive_tcp_message_framed(socket_t socket);

// UDP Socket Functions
/**
 * Create a UDP socket with dual stack support (IPv6 with IPv4 support)
 * @param port The port to bind to (0 for any available port)
 * @return UDP socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_udp_socket(int port = 0);

/**
 * Create a UDP socket with IPv4 support only
 * @param port The port to bind to (0 for any available port)
 * @return UDP socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_udp_socket_v4(int port = 0);

/**
 * Create a UDP socket with IPv6 support only
 * @param port The port to bind to (0 for any available port)
 * @return UDP socket handle, or INVALID_SOCKET_VALUE on error
 */
socket_t create_udp_socket_v6(int port = 0);

/**
 * Send UDP data to a peer
 * @param socket The UDP socket handle
 * @param data The data to send
 * @param peer The destination peer
 * @return Number of bytes sent, or -1 on error
 */
int send_udp_data(socket_t socket, const std::vector<uint8_t>& data, const Peer& peer);

/**
 * Send UDP data to a hostname and port directly
 * @param socket The UDP socket handle
 * @param data The data to send
 * @param hostname The destination hostname or IP address
 * @param port The destination port
 * @return Number of bytes sent, or -1 on error
 */
int send_udp_data_to(socket_t socket, const std::vector<uint8_t>& data, const std::string& hostname, int port);

/**
 * Receive UDP data from a peer
 * @param socket The UDP socket handle
 * @param buffer_size Maximum number of bytes to receive
 * @param sender_peer Output parameter for the sender's peer info
 * @return Received data, empty vector on error
 */
std::vector<uint8_t> receive_udp_data(socket_t socket, size_t buffer_size, Peer& sender_peer);

/**
 * Receive UDP data with timeout support
 * @param socket The UDP socket handle
 * @param buffer_size Maximum number of bytes to receive
 * @param timeout_ms Timeout in milliseconds (0 for non-blocking, -1 for blocking)
 * @param sender_ip Optional output parameter for sender IP address
 * @param sender_port Optional output parameter for sender port
 * @return Received data, empty vector on timeout or error
 */
std::vector<uint8_t> receive_udp_data_with_timeout(socket_t socket, size_t buffer_size, int timeout_ms, 
                                                   std::string* sender_ip = nullptr, int* sender_port = nullptr);

// Common Socket Functions
/**
 * Close a socket
 * @param socket The socket handle to close
 */
void close_socket(socket_t socket, bool force = false);

/**
 * Check if a socket is valid
 * @param socket The socket handle to check
 * @return true if valid, false otherwise
 */
bool is_valid_socket(socket_t socket);

/**
 * Set socket to non-blocking mode
 * @param socket The socket handle
 * @return true if successful, false otherwise
 */
bool set_socket_nonblocking(socket_t socket);

/**
 * Check if a socket is a TCP socket
 * @param socket The socket handle to check
 * @return true if TCP socket, false otherwise
 */
bool is_tcp_socket(socket_t socket);

} // namespace librats 