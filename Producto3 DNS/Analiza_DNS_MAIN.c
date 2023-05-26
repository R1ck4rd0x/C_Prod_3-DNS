/* Producto 3. Automatización básica de la administración de un sistema operativo
*  Ricardo Mesa
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_DNS 2

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include "Cabeceras.h"
#include "Definiciones.h"

#pragma comment(lib, "Ws2_32.lib")


int main() {

    char filename[256], buffer[256];
    FILE* fp;
    IP_ADDRESS ip;
    DNS_SERVER* servers;
    int num_servers = 0;
    int i, j, k, result;
    char ip_DNS1[INET_ADDRSTRLEN];
    char ip_DNS2[INET_ADDRSTRLEN];
    char ip_dns_servers[MAX_DNS_SERVERS][INET_ADDRSTRLEN];
    char min_ip[16];

    DNS_ADAPT ip_dns_adapt[MAX_DNS];
    DNS_ADAPT fasterR_DNS;

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    ULONG family = AF_INET;
    
    int response_times[MAX_HOPS];
    int num_hops;
    int total_response[10];
    int total_response_adapt[4];
    char ip_addr_char[INET_ADDRSTRLEN]; // Variable de retorno de la IP a string, tamaño copnstante asignado por el sistema;

    // ----------------- Paso 1: Leer el nombre y la ruta del archivo dnsips.txt y mostrar su contenido por pantalla

    printf("Introduce el nombre y la ruta del archivo dnsips.txt: ");
    scanf_s("%255s", filename, 256);         // se leerá una cadena de caracteres de longitud máxima %255s y el 256 impide que se desborde el buffer y pete el programa

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\nError: no se pudo abrir el archivo %s.\n", filename);
        //return 1;
    }

    printf("\nEl contenido del archivo %s:\n", filename);
    while (fgets(buffer, 256, fp) != NULL) 
    {
        printf("%s", buffer);
        if (read_ip_address(buffer, &ip)) {
            num_servers++;
        }
    }
    fclose(fp);

    //------------------ Paso 2: Mostrar los adaptadores del equipo por pantalla, de los que se le pide al usuario que elija uno.

    // Llamamos a la función que leerá los adaptadores
    
    int adapter_index = read_Adaptors();

    // Llamamos a la función que extrae los DNS de los adaptadores


    read_DNS_servers(adapter_index, &ip_dns_servers);

    printf("Los DNS del adaptador seleccionado son: %s y %s\n", ip_dns_servers[0], ip_dns_servers[1]);


    strcpy(ip_dns_adapt[0].ip, ip_dns_servers[0]);

    strcpy(ip_dns_adapt[1].ip, ip_dns_servers[1]);



    // Comparamos los tiempos y los saltos de los DNS del adaptador seleccionado.
    i = 0;
    k = 0;
    total_response_adapt[i] = 0;

    for (k = 0; k < MAX_DNS; k++) {
        result = tracert(ip_dns_adapt[k].ip, response_times, &num_hops);

        if (result == 0) {
            printf("\nTiempo de respuesta y número de saltos para %s:\n", ip_dns_adapt[k].ip);
            for (int j = 0; j < num_hops; j++) {
                printf("\nSalto %d: %d ms\n", j + 1, response_times[j]);
                total_response_adapt[i] += response_times[j];
            }
            printf("\nNúmero de saltos: %d\n", num_hops);
            printf("Tiempo de respuesta total: %d ms\n", total_response_adapt[i]);
        }
        ip_dns_adapt[k].hops = num_hops;
        ip_dns_adapt[k].response_time = total_response_adapt[i];
    }


    // ---------------------   Paso 3: Leer el archivo dnsips.txt y comprobar la accesibilidad de los servidores DNS

    
    printf("\nPulse una tecla para analizar los DNS del archivo...%s\n", filename);
    system("pause");
                    
    servers = calloc(num_servers, sizeof(DNS_SERVER));
    if (servers == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        //return 1;
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: no se pudo abrir el archivo %s.\n", filename);
        free(servers);
        //return 1;
    }
    
    i = 0;
    while (fgets(buffer, 256, fp) != NULL) {
        if (read_ip_address(buffer, &ip)) {

            ip_address_to_string(&ip, ip_addr_char);
            servers[i].ip = ip;
            servers[i].hops = -1;
            servers[i].response_time = -1;
            total_response[i] = 0;

            // llamamos a la función que nos calculará el tiempo de respuesta y el número  de saltos 

            result = tracert(ip_addr_char, response_times, &num_hops);

            if (result == 0) {
                printf("\nTiempo de respuesta y número de saltos para %s:\n", ip_addr_char);
                for (int j = 0; j < num_hops; j++) {
                    printf("\nSalto %d: %d ms\n", j + 1, response_times[j]);
                    total_response[i] += response_times[j];
                }
                printf("\nNúmero de saltos: %d\n", num_hops);
                printf("Tiempo de respuesta total: %d ms\n", total_response[i]);
            }
            servers[i].hops = num_hops;
            servers[i].response_time = total_response[i];

            
            i++;
        }
    }

    num_servers = i;

    fclose(fp);

    // ----------------- Paso 4: Llamamos a la funcióin para leer el archivo DNS_Buenos.txt y comprobar el menor tiempo o saltos de los servidores DNS
    
    DNS_MAS_RAPIDO("DNS_Buenos.txt", &fasterR_DNS);

    printf("\nEl DNS con mejor tiempo de respuesta y menos saltos del archivo es: %s\n", fasterR_DNS.ip);

    // Verificar si la IP ya está configurada en ip_dns_adapt[0] o ip_dns_adapt[1]

    bool already_configured = false;
    for (int i = 0; i < MAX_DNS; i++) {
        if (strcmp(fasterR_DNS.ip, ip_dns_adapt[i].ip) == 0) {
            already_configured = true;
            break;
        }
    }
    if (already_configured) {
        printf("Ya tienes el mejor DNS configurado.\n");
        return;
    }
    // Obtener el índice del DNS con el response_time más alto de los que están configurados en el adaptador

    int peor_DNS = 0;
    for (int i = 1; i < MAX_DNS; i++) {
        if (ip_dns_adapt[i].response_time > ip_dns_adapt[peor_DNS].response_time) {
            peor_DNS = i;
        }
       
    }

    // Comparar el DNS más rápido con el DNS seleccionado en ip_dns_adapt
    if (fasterR_DNS.response_time < ip_dns_adapt[peor_DNS].response_time ||
        (fasterR_DNS.response_time == ip_dns_adapt[peor_DNS].response_time &&
            fasterR_DNS.hops < ip_dns_adapt[peor_DNS].hops)) {
        // Actualizar los campos de ip_dns_adapt con los valores del DNS más rápido
        printf("\n Se cambiara el DNS %s, con %d ms y %d saltos\n", ip_dns_adapt[peor_DNS].ip, ip_dns_adapt[peor_DNS].response_time, ip_dns_adapt[peor_DNS].hops);
        strcpy(ip_dns_adapt[peor_DNS].ip, fasterR_DNS.ip);
        ip_dns_adapt[peor_DNS].hops = fasterR_DNS.hops;
        ip_dns_adapt[peor_DNS].response_time = fasterR_DNS.response_time;

        printf("\n Por el DNS %s, con %d ms y %d saltos\n", ip_dns_adapt[peor_DNS].ip, ip_dns_adapt[peor_DNS].response_time, ip_dns_adapt[peor_DNS].hops);

        system("pause");
    }
 

    return 0;
}

    