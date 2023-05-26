#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#ifndef Definiciones_H
#define Definiciones_H
#define MAX_LINE_LENGTH 100
#define MAX_DNS 2
#define MAX_HOPS 30
#define MAX_LINE_LENGTH 256
#define MAX_DNS_SERVERS 10

// Estructura que representa una dirección IP
typedef struct {
    unsigned char b1, b2, b3, b4;
} IP_ADDRESS;

// Estructura que representa un servidor DNS
typedef struct {
    IP_ADDRESS ip;
    int hops;
    int response_time;
    FILE* fp;
} DNS_SERVER;

// Estructura para los servidores DNS de un adaptador
typedef struct {
    char ip[16];
    int hops;
    int response_time;
} DNS_ADAPT;


#endif // Definiciones_H