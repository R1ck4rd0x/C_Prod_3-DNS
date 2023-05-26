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
#pragma comment(lib, "ws2_32.lib")


int tracert(char* ip_address, int* response_times, int* num_hops) {
    char command[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    int hop = 0;
    int tmax = 0;
    int total_time = 0;

    // Construimos nuestro comando tracert con los la IP que corresponda
    sprintf_s(command, MAX_LINE_LENGTH, "tracert -d -h %d %s", MAX_HOPS, ip_address);

    // Abrimos la tubería para ejecutar el comando construido
    FILE* pipe = _popen(command, "r");
    if (!pipe) {
        printf("Error: No se pudo abrir el comando 'tracert'\n");
        return -1;
    }
    printf("\nAnalizando el DNS ..... %s \n", ip_address);
        
    // Leemos la salida línea por línea
    while (fgets(line, MAX_LINE_LENGTH, pipe)) {


        // Buscamos el siguiente salto si el puntero de la linea encuentra " " quiere decir que recibimos 
       // respueta y posiciona el puntero despues del espacio que se encuentra el número de salto, lo extraemos con "atoi"
        char* ptr_hop = strstr(line, " ");
        if (ptr_hop != NULL && strstr(line, "Traza completa.") == NULL) {
            hop = atoi(ptr_hop);
        }

        if (hop > 0 && hop <= MAX_HOPS) {
            // Buscamos el tiempo de respuesta para el hop actual
            char* ptr = strstr(line, "ms");
            if (ptr != NULL) {
                ptr = ptr - 4;
                int time = atoi(ptr);
                response_times[hop - 1] = time;
                total_time += time;
                printf("\n salto actual %d tiempo de respuesta %d milisengundos\n", hop, time);
            }
            else if (strstr(line, "Tiempo de espera agotado") != NULL) {
                response_times[hop - 1] = 10000;
                total_time += 10000;
            }
        }

        

        // Si la traza tiene éxito, guardamos los dNS en un archivo, despues cerramos el pipe y salimos del bucle
        if (strstr(line, "Traza completa.") != NULL) {

            // Verificar si el archivo existe
            if (_access("DNS_Buenos.txt", 0) == -1) {
                // Si no existe, crear un archivo nuevo y escribir la dirección IP
                FILE* fp = fopen("DNS_Buenos.txt", "w");
                if (fp == NULL) {
                    printf("\nError: No se pudo crear el archivo DNS_Buenos.txt\n");
                    // return -1;
                }
                fprintf(fp, "%s %d %d\n", ip_address, hop, total_time);
                fclose(fp);
                printf("\n --------> Completado!! La dirección IP %s se ha guardado en el archivo DNS_Buenos.txt\n", ip_address);
            }
            else {
                // Si el archivo ya existe, abrirlo en modo "append" y escribir la dirección IP al final
                FILE* fp = fopen("DNS_Buenos.txt", "a");
                if (fp == NULL) {
                    printf("\nError: No se pudo abrir el archivo DNS_Buenos.txt\n");
                    // return -1;
                }
                fprintf(fp, "%s %d %d\n", ip_address, hop, total_time);
                fclose(fp);
                printf("\n --------> Completado!!La dirección %s se ha agregado al archivo DNS_Buenos.txt\n", ip_address);
            }
           
            _pclose(pipe);
            break;
        }
        if (strstr(line, "Tiempo de espera agotado") != NULL) {
            printf("\n Tiempo de espera agotado\n");
            tmax++;
            if (tmax == 4) { // He puesto este límite, porque normalmente con estos límites de tiempo agotados no resultan completados
                printf("\n ---------> Demasiados saltos AGOTADOS el destino será inalcanzable.... abortando. No se escribirá en DNS_Buenos.txt\n");
                break;
            }
        }
        // Si hemos llegado al final del archivo, salimos del bucle
        if (feof(pipe)) {
            break;
        }
     
    }
        
    *num_hops = hop;
       
    return 0;
       
}
