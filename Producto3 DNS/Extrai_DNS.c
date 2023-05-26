#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <time.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include "Cabeceras.h"
#include "Definiciones.h"

// Enlazamos las librerias que lo necesitan 
#pragma comment(lib, "iphlpapi.lib") 
#pragma comment(lib, "Ws2_32.lib")



int read_DNS_servers(int adapter_index, char ip_dns_servers[MAX_DNS_SERVERS][INET_ADDRSTRLEN]) {
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    DWORD dwRetVal = 0;
    char ip_DNS_USER[INET_ADDRSTRLEN]; // Variable de retorno de la IP a string, con el tamaño copnstante asignado por el sistema;
    char ip_DNS1[INET_ADDRSTRLEN];
    char ip_DNS2[INET_ADDRSTRLEN];

    // Paso 1: Obtener la lista de direcciones de adaptadores de red
    dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
    }

    // Paso 2: Buscar el adaptador seleccionado por el usuario
    PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
    while (pCurrAddresses) {
        if (pCurrAddresses->IfIndex == adapter_index) { // adapter_index es la variable que contiene el índice del adaptador seleccionado
            // Paso 3: Mostrar la lista de servidores DNS configurados
            PIP_ADAPTER_DNS_SERVER_ADDRESS pDnsServer = NULL;
            pDnsServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnsServer) {
                printf("\n...Lista de servidores DNS configurados para el adaptador seleccionado:\n");
                int d = 0;
                do { // Además de mostrarlos, los escribimos en el array para  pasarlos devueltos al main.
                    printf("%s\n", inet_ntoa(((struct sockaddr_in*)pDnsServer->Address.lpSockaddr)->sin_addr));
                    snprintf(ip_dns_servers[d],INET_ADDRSTRLEN, "%s", inet_ntoa(((struct sockaddr_in*)pDnsServer->Address.lpSockaddr)->sin_addr));
                    d++;
                    pDnsServer = pDnsServer->Next;
                } while (pDnsServer);
            }
            else {
                printf("\nEl adaptador seleccionado no tiene servidores DNS configurados.\n");
            }

            break;
        }
        pCurrAddresses = pCurrAddresses->Next;
    }

    if (pAddresses) {
        free(pAddresses);
    }
    return 0;
}
