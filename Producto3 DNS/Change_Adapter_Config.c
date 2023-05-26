#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windns.h>
#include "Cabeceras.h"
#include "Definiciones.h"

// Enlazamos las librerias que lo necesitan 
#pragma comment(lib, "iphlpapi.lib") 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Dnsapi.lib")

// Función para cambiar el DNS de un adaptador en Windows * 

bool ChangeAdapterDNS(int adapterIndex, const char* dnsIP) {
    /* Obtener el nombre de adaptador correspondiente al adapterIndex
    char adapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    if (GetAdapterNameFromIndex(adapterIndex, adapterName) != ERROR_SUCCESS) {
        // Error al obtener el nombre del adaptador
        return false;
    }*/

    // Construir el comando netsh para cambiar el DNS del adaptador usando el propio adapterIndex
    char command[100];
    snprintf(command, sizeof(command), "netsh interface ip set dns name=\"%d\" static %s index=%d", adapterIndex, dnsIP);

    // Ejecutar el comando netsh
    int result = system(command);
    if (result != 0) {
        // Error al ejecutar el comando netsh
        return false;
    }

    // Éxito en la configuración del DNS
    return true;
}



