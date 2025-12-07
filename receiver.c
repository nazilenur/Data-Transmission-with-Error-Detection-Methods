
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "utils.h"

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int valread;
    
    char *data_part;
    char *method_part;
    char *checksum_part;
    
    
    unsigned short calc_crc, recv_crc;
    int calc_parity, recv_parity;
    int is_correct = 0; 

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) return -1;

    printf(" CLIENT 2\n");
    printf("Sunucuya baglaniliyor\n");

    while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        sleep(2);
    }
    printf("-> Baglandi, Veri bekleniyor.\n");

    while(1) {
        memset(buffer, 0, 1024);
        valread = read(sock, buffer, 1024);
        
        if(valread > 0) {
            char temp_buffer[1024];
            strcpy(temp_buffer, buffer);

            data_part = strtok(temp_buffer, "|");    
            method_part = strtok(NULL, "|");    
            checksum_part = strtok(NULL, "|");  

            if (data_part && method_part && checksum_part) {
                printf("\n SONUC RAPORU\n");
                printf("Gelen Veri: %s\n", data_part);
                printf("Yontem: %s\n", method_part);
                printf("Gelen Checksum: %s\n", checksum_part);

                
                if (strcmp(method_part, "PARITY") == 0) {
                   
                    calc_parity = calculate_parity(data_part, strlen(data_part));
                    recv_parity = atoi(checksum_part); 
                    
                    printf("Hesaplanan Parity: %d\n", calc_parity);
                    if (calc_parity == recv_parity) is_correct = 1;
                    else is_correct = 0;

                } else {
                    
                    calc_crc = calculate_crc16(data_part, strlen(data_part));
                    recv_crc = (unsigned short)strtol(checksum_part, NULL, 16);
                    
                    printf("Hesaplanan CRC (Hex): %X\n", calc_crc);
                    if (calc_crc == recv_crc) is_correct = 1;
                    else is_correct = 0;
                }

                
                if (is_correct) {
                    printf("DURUM: \033[0;32mDATA CORRECT (Veri Saglam)\033[0m\n");
                } else {
                    printf("DURUM: \033[0;31mDATA CORRUPTED (Veri Bozuk!)\033[0m\n");
                }
            }
        } else {
            printf("Baglanti koptu.\n");
            break;
        }
    }
    close(sock);
    return 0;
}