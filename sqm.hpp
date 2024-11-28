#pragma once

#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <WinSock2.h>
	#include <WS2tcpip.h>

	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <unistd.h>

	#define SOCKET int
#endif

#include "wrappers.hpp"

class c_sqm
{
private:
	SOCKET _socket;
	sockaddr _address;
	const char *_ip;
	u_short _port;

public:
	static c_sqm *singleton()
	{
		static c_sqm instance;
		return &instance;
	}

	/*
	*	Initialize connection socket
	*/
	void initialize(const char *ip, u_short port)
	{
		int result;
#ifdef _WIN32	
		WSADATA wsa_data;
		result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (result != 0)
		{
#ifdef __cpp_exceptions
			throw std::runtime_error("Failed to initiates WinSock.");
#else
			return;
#endif
		}

#endif
		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(ip);
		address.sin_port = htons(port);

		_socket = socket(address.sin_family, SOCK_DGRAM, IPPROTO_UDP);
		if (_socket == -1)
		{
#ifdef __cpp_exceptions
			throw std::runtime_error("Failed to create socket.");
#else
			return;
#endif
		}

		u_long param = 1;
#ifdef _WIN32
		result = ioctlsocket(_socket, FIONBIO, &param);
#else
		result = ioctl(_socket, FIONBIO, &param);
#endif
		if (result < 0)
		{
#ifdef __cpp_exceptions
			throw std::runtime_error("Failed to set socket non-blocking.");
#else
			return;
#endif
		}

		_address = reinterpret_cast<sockaddr&>(address);
		_ip = ip;
		_port = port;
	}
	
    void shutdown()
    {
#ifdef _WIN32
		closesocket(_socket);
		WSACleanup();
#else
		close(_socket);
#endif 
    }

	/*
	*	Translate IP to bytes (for the Query)
	*/
	std::string translate_ip(std::string ip)
	{
		std::string result, part;
		std::stringstream it(ip);
		while (std::getline(it, part, '.'))
		{
			result += atoi(part.c_str());
		}
		return result;
	}

	/*
	*	Send query and receive response
	*/
	std::stringstream query(e_query_type type, int timeout, const char* rconcmd = NULL, const char* rconpass = NULL)
	{
		if(type >= QUERYTYPES_COUNT)
		{
#ifdef __cpp_exceptions
			throw std::runtime_error("Invalid query type.");
#else
			return std::stringstream("");
#endif
		}

		std::string packet {"SAMP"};

		packet += translate_ip(_ip);
		packet += (char)(_port & 0xFFu);
		packet += (char)((_port >> 8) & 0xFFu);
		packet += e_type_to_rpc[type];

		if(type == QUERYTYPE_RCON)
		{
			if(rconcmd && rconpass && rconcmd[0] && rconpass[0])
			{
				// TODO: needs to be re-tested
				int16_t len = strlen(rconpass);
				packet += (char)(len & 0xFFu);
				packet += (char)((len >> 8) & 0xFFu);
				packet += rconpass;

				len = strlen(rconcmd);
				packet += (char)(len & 0xFFu);
				packet += (char)((len >> 8) & 0xFFu);
				packet += rconcmd;

				sendto(_socket, packet.c_str(), packet.size(), 0, &_address, sizeof(_address));
			}
			return std::stringstream("");
		}

		sendto(_socket, packet.c_str(), packet.size(), 0, &_address, sizeof(_address));
		return receive(timeout);
	}

	/*
	*	Waiting for response from server (1s timeout should be enough for ping < 500)
	*/
	std::stringstream receive(int timeout)
	{
		char buffer[2048];
		sockaddr from;
		socklen_t from_length = sizeof(_address);

		std::stringstream packet {};
		time_t start_time = time(0);
		while (time(0) - start_time < timeout)
		{
#ifdef _WIN32
			Sleep(1); // 1ms
#else
			usleep(1000); // 1ms
#endif
			int received_bytes = recvfrom(_socket, buffer, sizeof(buffer), 0, &from, &from_length);
			if (received_bytes > 11)
			{
				packet = std::stringstream(std::string(buffer, received_bytes));
				start_time = time(0);
			}
		}
		return packet;
	}

	template<typename T> 
	T handle(std::stringstream ss)
	{
		if(std::is_pointer<T>::value)
		{
			return ( new decltype(std::remove_pointer<T>::type)( ss ) );
		}
		else
		{
			return T {ss};
		}
	}
};
