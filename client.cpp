#include <iostream>
#include <cstring>  // for memset
#include <arpa/inet.h>  // for inet_pton()
#include <unistd.h>  // for close()
#include <sys/socket.h> // for socket()

const int PORT = 4277;
const char SERVER_IP[] = "127.0.0.1";  // La IP del servidor (localhost)

int main() {
    // Crear el socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error al crear el socket\n";
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  // Limpiar la estructura
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Convertir el puerto a formato de red
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Error al convertir la IP\n";
        close(sockfd);
        return 1;
    }

    // Conectarse al servidor
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error al conectar al servidor\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Conectado al servidor\n";

    // Enviar un mensaje al servidor

    // MID Hard coded
    const char *message = "0JUANVINDAS*********************kyAksk22589Z$*******************aOjskasndp1ands";
    if (send(sockfd, message, strlen(message), 0) < 0) {
        std::cerr << "Error al enviar el mensaje\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Mensaje enviado: " << message << "\n";

    // Cerrar el socket
    close(sockfd);
    std::cout << "Conexión cerrada\n";

    return 0;
}
