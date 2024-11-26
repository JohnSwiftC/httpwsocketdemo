#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>

#define MAXBUFLEN 100

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {
	WSADATA WsaData;

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) {
		printf("Could not start WinSock\n");
		return(1);
	}

	// Check to see if args are correct.

	if (argc != 3) {
		printf("usage: sendgetreq <URL> <FILE>");
		return(1);
	}

	struct addrinfo* p, * servinfo, hints;

	memset(&hints, 0, sizeof hints);

	// Fill hints, we want a TCP with IPv4 for HTTP!!!

	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//Fill linked list.

	getaddrinfo(argv[1], "80", &hints, &servinfo);

	// Check for bad address

	if (servinfo == NULL) {
		printf("Bad domain!!!\n");
		return(1);
	}

	// Now move through linkedlist and try to get a socket and connection to server.

	int sockfd;

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			printf("Failed to get socket... Trying again...\n");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			printf("Failed to connect to socket... Trying again...\n");
			continue;
		}

		break;
	}

	// Ensure we do have a connection.

	if (p == NULL) {
		printf("Failed to get connection.\n");
		return(1);
	}

	// We no longer need servinfo

	freeaddrinfo(servinfo);

	// Create our message.

	char message[1024];

	memset(message, 0, sizeof message);

	strcat(message, "GET ");
	strcat(message, argv[2]);
	strcat(message, " HTTP/1.1\r\n");
	strcat(message, "Host: ");
	strcat(message, argv[1]);
	strcat(message, "\r\n\r\n");

	// Time to send and recv

	char buf[4096];

	memset(buf, 0, sizeof buf);

	int numbytes;

	do {
		numbytes = send(sockfd, message, strlen(message), 0);
	} while (numbytes == -1);

	// Now recieve server response!

	do {
		numbytes = recv(sockfd, buf, sizeof buf, 0);
	} while (numbytes == -1);

	printf("Message from server: %s", buf);

	// Clean up after ourselves.

	freeaddrinfo(p);
	closesocket(sockfd);
	WSACleanup();
	return(0);
}