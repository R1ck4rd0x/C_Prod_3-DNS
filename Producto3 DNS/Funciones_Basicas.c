#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_LINE_LENGTH 100

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Enlazamos las librerias que lo necesitan 
#include "Cabeceras.h"
#include "Definiciones.h"

#pragma comment(lib, "Ws2_32.lib")


// -------------------------------------------------------------------------

// Función que lee una dirección IP desde una cadena de caracteres
int read_ip_address(char* str, IP_ADDRESS* ip) {
    int i, n;
    /* analizar la cadena de caracteres str y extraer los cuatro números enteros que forman la dirección IP, que se almacenan en los campos b1, b2, b3 y b4 de la estructura IP_ADDRESS.
    Si sscanf_s es capaz de leer los cuatro números correctamente, la función devuelve el valor 1 (ya que el [n]úmero de valores leídos es 4, que es el número de campos de la estructura IP_ADDRESS)
    */
    n = sscanf_s(str, "%hhu.%hhu.%hhu.%hhu", &ip->b1, &ip->b2, &ip->b3, &ip->b4); // El modificador hh indica que se debe interpretar como un unsigned char, que es un tipo de dato de 8 bits en C.
    return n == 4;
}

// -------------------------------------------------------------------------

// Función que convierte una dirección IP a una cadena de caracteres
char* ip_address_to_string(IP_ADDRESS* ip, char* buffer) {
    sprintf_s(buffer, 16, "%hhu.%hhu.%hhu.%hhu", ip->b1, ip->b2, ip->b3, ip->b4);
    return buffer;
}


// -------------------------------------------------------------------------

// Función que calcula el tiempo de respuesta de un servidor DNS
int measure_dns_server_response_time(IP_ADDRESS* ip) {
    SOCKET s;
    struct sockaddr_in addr;
    clock_t start, end;
    int elapsed_time;
    char ip_str[INET_ADDRSTRLEN]; // Variable de retorno de la IP a string, con el tamaño copnstante asignado por el sistema

    // Inicializamos la biblioteca Winsock antes de crear el socket.
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return -1;
    }

    // Aquí se crea un socket con el protocolo de Internet (AF_INET), el tipo de socket es datagrama (SOCK_DGRAM) y el protocolo es UDP (IPPROTO_UDP). Si el socket no se puede crear, se devuelve -1.
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = inet_addr(ip_address_to_string(ip, ip_str));

    start = clock();
    sendto(s, "ping", 4, 0, (struct sockaddr*)&addr, sizeof(addr));
    recvfrom(s, "pong", 4, 0, NULL, NULL);
    end = clock();

    
    elapsed_time = (int)((end - start) * 1000 / CLOCKS_PER_SEC);
    printf("Haciendo ping a %s recibiendo respuesta en %d milisegundos\n", ip_str, elapsed_time);

    closesocket(s);
    WSACleanup();

    //lapsed_time = (int)((end - start) * 1000 / CLOCKS_PER_SEC);
    return elapsed_time;
}


// -------------------------------------------------------------------------

// Función que calcula el DNS más rápido de los resultados obtenidos en un archivo

void DNS_MAS_RAPIDO(const char* file_name, DNS_ADAPT* faster_DNS) {

    FILE* file = fopen("DNS_Buenos.txt", "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo DNS_Buenos.txt\n");
        return 1;
    }
    //DNS_ADAPT faster_DNS;
    char ip_address[16];
    int hops, milliseconds;
    char min_ip[16] = { 0 };
    int min_hops = INT_MAX;
    int min_milliseconds = INT_MAX;

    while (fscanf(file, "%s %d %d", ip_address, &hops, &milliseconds) == 3) {
        if (milliseconds < min_milliseconds) {
            // Nuevo mínimo de milisegundos encontrado
            strcpy(min_ip, ip_address);
            min_hops = hops;
            min_milliseconds = milliseconds;
        }
        else if (milliseconds == min_milliseconds) {
            // Empate en milisegundos, comparar número de saltos
            if (hops < min_hops) {
                // Nuevo mínimo de saltos encontrado
                strcpy(min_ip, ip_address);
                min_hops = hops;
            }
        }
    }
    fclose(file);

    printf("\nLa IP con menor tiempo y menos saltos es: %s\n", min_ip);
    printf("\nNúmero de saltos: %d\n", min_hops);
    printf("\nMilisegundos: %d\n", min_milliseconds);
    strcpy(faster_DNS->ip, min_ip);
    faster_DNS->hops = min_hops;
    faster_DNS->response_time = min_milliseconds;
    
}