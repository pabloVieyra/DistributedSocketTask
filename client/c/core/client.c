#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MIN_USERNAME_LENGTH 5
#define MAX_USERNAME_LENGTH 15
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 50

void initialize_winsock();
SOCKET create_client_socket();
void connect_to_server(SOCKET client_socket);
void show_menu(SOCKET client_socket);
void send_request(SOCKET client_socket, const char *request);
void receive_response(SOCKET client_socket);
void cleanup(SOCKET client_socket);

void initialize_winsock()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Error initializing Winsock. Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

SOCKET create_client_socket()
{
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to create socket. Code: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

void connect_to_server(SOCKET client_socket)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "Connection to server failed(initializer server pls). Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s on port %d.\n", SERVER_IP, SERVER_PORT);
}

void show_menu(SOCKET client_socket)
{
    int choice;
    char buffer[BUFFER_SIZE];
    int length;

    while (1)
    {
        printf("\n--- Menu ---\n");
        printf("1. Generate Username\n");
        printf("2. Generate Password\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter desired username length (%d-%d): ", MIN_USERNAME_LENGTH, MAX_USERNAME_LENGTH);
            scanf("%d", &length);
            snprintf(buffer, BUFFER_SIZE, "username:%d", length);
            send_request(client_socket, buffer);
            receive_response(client_socket);
            break;
        case 2:
            printf("Enter desired password length (%d-%d): ", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
            scanf("%d", &length);
            snprintf(buffer, BUFFER_SIZE, "password:%d", length);
            send_request(client_socket, buffer);
            receive_response(client_socket);
            break;
        case 3:
            printf("Exiting...\n");
            cleanup(client_socket);
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }
}

void send_request(SOCKET client_socket, const char *request)
{
    if (send(client_socket, request, strlen(request), 0) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to send request. Code: %d\n", WSAGetLastError());
    }
}

void receive_response(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE];
    int length = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (length > 0)
    {
        buffer[length] = '\0';
        printf("Server response: %s\n", buffer);
    }
    else
    {
        fprintf(stderr, "Failed to receive response or connection closed.\n");
    }
}

void cleanup(SOCKET client_socket)
{
    closesocket(client_socket);
    WSACleanup();
}

int main()
{
    SOCKET client_socket;

    initialize_winsock();
    client_socket = create_client_socket();
    connect_to_server(client_socket);

    show_menu(client_socket);

    cleanup(client_socket);
    return 0;
}
