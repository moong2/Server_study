#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create server socket");
        return 1;
    }
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Failed to bind server socket");
        close(serverSocket);
        return 1;
    }
    
    if (listen(serverSocket, 5) == -1) {
        perror("Failed to listen on server socket");
        close(serverSocket);
        return 1;
    }
    
    printf("Server is running and waiting for connections.\n");
    
    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            perror("Failed to accept client connection");
            close(serverSocket);
            return 1;
        }
        
        printf("Accepted a new client connection. Client socket: %d\n", clientSocket);
        
        close(clientSocket);
    }
    
    close(serverSocket);
    return 0;
}
