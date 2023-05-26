#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include "Cabeceras.h"
#include "Definiciones.h"

// Enlazamos las librerias que lo necesitan 
#pragma comment(lib, "iphlpapi.lib") 
#pragma comment(lib, "Ws2_32.lib")

PIP_ADAPTER_INFO pAdapterInfo = NULL;
PIP_ADAPTER_INFO pAdapter = NULL;
ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
int adapter_index = -1;


// Paso 2: Función que muestra la lista de adaptadores de red y preguntar al usuario por el que quiere modificar
int read_Adaptors() {
    printf("\n Pulse una tecla para analizar los adaptadores de su euipo...\n");
    system("pause");

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return 1;
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error: no se pudo reservar memoria.\n");
            return 1;
        }

        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != NO_ERROR) {
            printf("Error: no se pudo obtener la información de los adaptadores de red.\n");
            free(pAdapterInfo);
            return 1;
        }
    }

    printf("\nLista de adaptadores de red de su equipo:\n");
    for (pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next) {
        printf("%d. %s\n", pAdapter->Index, pAdapter->Description);
    }

    free(pAdapterInfo);

    printf("\nIntroduce el índice del adaptador de red que quieres modificar: \n");
    scanf_s("%d", &adapter_index);

    // Obtener los DNS configurados para el adaptador seleccionado
    
    return adapter_index;
}