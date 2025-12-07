
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
    char message[500];
    char packet[1024];
    
    
    unsigned short crc_val;
    int parity_val;
    int choice;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket hatasi \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n Gecersiz Adres \n");
        return -1;
    }

    printf(" CLIENT 1 (GONDERICI) ---\n");
    printf("Sunucuya baglaniliyor.\n");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nBaglanti Hatasi! Once ./sunucu calismali.\n");
        return -1;
    }
    printf("-> Baglandi\n");

    while(1) {
        printf("\nMesaj girin,Cikis icin 'exit yazin'): ");
        if (fgets(message, sizeof(message), stdin) == NULL) break;
        message[strcspn(message, "\n")] = 0;

        if(strcmp(message, "exit") == 0) break;
        if(strlen(message) == 0) continue;

       
        printf("Yontem Secin: [1] CRC-16  [2] Parity: ");
        if (scanf("%d", &choice) != 1) choice = 1; 
        getchar();

        if (choice == 2) {
            
            parity_val = calculate_parity(message, strlen(message));
          
            sprintf(packet, "%s|PARITY|%d", message, parity_val);
        } else {
           
            crc_val = calculate_crc16(message, strlen(message));
            
            sprintf(packet, "%s|CRC16|%X", message, crc_val);
        }

        send(sock, packet, strlen(packet), 0);
        printf("-> Paket yollandi: %s\n", packet);
    }

    close(sock);
    return 0;
}