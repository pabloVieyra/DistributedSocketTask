#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <ctype.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024
#define MIN_USERNAME_LENGTH 5
#define MAX_USERNAME_LENGTH 15
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 50
#define SERVER_PORT 8080
#define MAX_PENDING_CONNECTIONS 3

void initialize_winsock();
SOCKET create_server_socket();
void bind_and_listen(SOCKET server_socket);
void handle_client_connection(SOCKET client_socket);
char *generate_username(int length);
char *generate_password(int length);
void send_response(SOCKET client_socket, const char *response);
void log_message(const char *message, int error_code);
void cleanup(SOCKET server_socket);
void validate_length(int length, int min, int max, const char *error_message, SOCKET client_socket);

void initialize_winsock()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        log_message("Error initializing Winsock.", 500);
        exit(EXIT_FAILURE);
    }
}

SOCKET create_server_socket()
{
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        log_message("Failed to create socket.", 500);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return server_socket;
}

void bind_and_listen(SOCKET server_socket)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        log_message("Bind failed.", 500);
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == SOCKET_ERROR)
    {
        log_message("Listen failed.", 500);
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    log_message("Server listening on port.", 200);
}

void handle_client_connection(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE];
    int length;

    while (1)
    {

        length = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (length <= 0)
        {
            if (length == 0)
            {
                log_message("Client disconnected.", 200);
            }
            else
            {
                log_message("Failed to receive data from client.", 400);
            }
            closesocket(client_socket);
            return;
        }

        buffer[length] = '\0';

        if (strcmp(buffer, "exit") == 0)
        {
            log_message("Client requested to exit.", 200);
            closesocket(client_socket);
            return;
        }

        char operation[BUFFER_SIZE];
        int input_length;
        sscanf(buffer, "%[^:]:%d", operation, &input_length);

        if (strcmp(operation, "username") == 0)
        {
            if (input_length >= MIN_USERNAME_LENGTH && input_length <= MAX_USERNAME_LENGTH)
            {
                char *username = generate_username(input_length);
                log_message("Generated username.", 200);
                send_response(client_socket, username);
                free(username);
            }
            else
            {
                send_response(client_socket, "Invalid username length. Please try again.");
                log_message("Invalid username length.", 400);
            }
        }
        else if (strcmp(operation, "password") == 0)
        {
            if (input_length >= MIN_PASSWORD_LENGTH && input_length <= MAX_PASSWORD_LENGTH)
            {
                char *password = generate_password(input_length);
                log_message("Generated password.", 200);
                send_response(client_socket, password);
                free(password);
            }
            else
            {
                send_response(client_socket, "Invalid password length. Please try again.");
                log_message("Invalid password length.", 400);
            }
        }
        else
        {
            send_response(client_socket, "Unknown operation. Please try again.");
            log_message("Unknown operation.", 400);
        }
    }
}

char *generate_username(int length)
{
    static const char *vowels = "aeiou";
    static const char *consonants = "bcdfghjklmnpqrstvwxyz";

    char *username = (char *)malloc((length + 1) * sizeof(char));
    if (!username)
    {
        log_message("Failed to allocate memory for username.", 500);
        exit(EXIT_FAILURE);
    }

    int is_vowel = rand() % 2;

    for (int i = 0; i < length; i++)
    {
        username[i] = is_vowel ? vowels[rand() % strlen(vowels)] : consonants[rand() % strlen(consonants)];
        is_vowel = !is_vowel;
    }

    username[length] = '\0';
    return username;
}

char *generate_password(int length)
{
    static const char *alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    char *password = (char *)malloc((length + 1) * sizeof(char));
    if (!password)
    {
        log_message("Failed to allocate memory for password.", 500);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++)
    {
        password[i] = alphanumeric[rand() % strlen(alphanumeric)];
    }

    password[length] = '\0';
    return password;
}

void send_response(SOCKET client_socket, const char *response)
{
    if (send(client_socket, response, strlen(response), 0) == SOCKET_ERROR)
    {
        log_message("Failed to send response.", 500);
    }
}

void log_message(const char *message, int error_code)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (error_code == 0 || error_code == 200)
    {
        printf("[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
               t->tm_hour, t->tm_min, t->tm_sec, message);
    }
    else
    {
        const char *error_message;
        switch (error_code)
        {
        case 400:
            error_message = "400 Bad Request: ";
            break;
        case 500:
            error_message = "500 Internal Server Error: ";
            break;
        default:
            error_message = "500 Internal Server Error: ";
            break;
        }
        printf("[%04d-%02d-%02d %02d:%02d:%02d] %s%s\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
               t->tm_hour, t->tm_min, t->tm_sec, error_message, message);
    }
}

void cleanup(SOCKET server_socket)
{
    closesocket(server_socket);
    WSACleanup();
}

void validate_length(int length, int min, int max, const char *error_message, SOCKET client_socket)
{
    if (length < min || length > max)
    {
        send_response(client_socket, error_message);
        log_message(error_message, 400);
        return;
    }
}

int main()
{
    SOCKET server_socket, client_socket;
    struct sockaddr_in client;
    int client_len = sizeof(struct sockaddr_in);

    srand((unsigned int)time(NULL));

    initialize_winsock();
    server_socket = create_server_socket();
    bind_and_listen(server_socket);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len)) != INVALID_SOCKET)
    {
        log_message("Client connected.", 200);
        handle_client_connection(client_socket);
    }

    if (client_socket == INVALID_SOCKET)
    {
        log_message("Failed to accept connection.", 500);
    }

    cleanup(server_socket);
    return 0;
}
