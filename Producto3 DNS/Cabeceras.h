#pragma once
#include "Definiciones.h"
#include <stdbool.h>
#ifndef Cabeceras_h
#define Cabeceras_h

// DECLARACIÓN DE FUNCIONES

int read_ip_address(char* str, IP_ADDRESS* ip);
int read_Adaptors();
int read_DNS_servers(int adapter_index, char ip_dns_servers[MAX_DNS_SERVERS][INET_ADDRSTRLEN]);
char* ip_address_to_string(IP_ADDRESS* ip, char* buffer);
int tracert(char* ip_address, int* response_times, int* num_hops);
int measure_dns_server_response_time(IP_ADDRESS* ip);
void DNS_MAS_RAPIDO(const char* file_name, DNS_ADAPT* faster_DNS);
bool ChangeAdapterDNS(int adapterIndex, const char* dnsIP);

#endif