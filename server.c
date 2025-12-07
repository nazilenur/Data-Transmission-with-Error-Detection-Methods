
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h> 

#define PORT 8080

/* Veriyi bozan fonksiyon (Bit Flip Yöntemi) */
void corrupt_data(char* buffer, int len) {
    if (len <= 0) return;
    
    // Packet yapısı: DATA|METHOD|HASH
    // HASH kısmını bozarsak checksum tutmaz, DATA kısmını bozarsak yine tutmaz.
    // Biz %100 ihtimalle rastgele bir yeri bozalım.
    
    // Pipe (|) karakterlerini korumaya çalışabiliriz ama 
    // Bit flip rastgele olduğu için direkt veriye dalalım.

    int random_index = rand() % len; /* Rastgele karakter seç */
    int random_bit = rand() % 8;     /* Rastgele bit (0-7) seç */
    
    printf("[SERVER] SALDIRI! %d. indisteki karakterin %d. biti degistiriliyor...\n", random_index, random_bit);
    
    /* Bitwise XOR ile biti ters çevir (1 ise 0, 0 ise 1 yapar) */
    buffer[random_index] ^= (1 << random_bit);
}

int main() {
    int server_fd, socket_receiver, socket_sender;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int valread;

    srand(time(0)); /* Rastgelelik için seed */

    /* Socket Kurulumu */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* Portu tekrar kullanılabilir yap */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
      if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("--- SERVER (ARACI + BOZUCU) ---\n");
    
    /* 1. Receiver Bekle (CLIENT 2) */
    printf("1. Receiver (Client 2) bekleniyor...\n");
    if ((socket_receiver = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept receiver");
        exit(EXIT_FAILURE);
    }
    printf("-> Receiver baglandi.\n");

    /* 2. Sender Bekle (CLIENT 1) */
    printf("2. Sender (Client 1) bekleniyor...\n");
    if ((socket_sender = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept sender");
        exit(EXIT_FAILURE);
    }
    printf("-> Sender baglandi.\n");

    /* SÜREKLİ İLETİŞİM DÖNGÜSÜ */
    while(1) {
        memset(buffer, 0, 1024);
        
        // Sender'dan oku
        valread = read(socket_sender, buffer, 1024);
        if (valread <= 0) {
            printf("Sender baglantiyi kesti.\n");
            break;
        }

        printf("\n[Gelen Paket]: %s\n", buffer);
        
        // Veriyi BOZ (Kesinlikle bozacak şekilde ayarladık)
        corrupt_data(buffer, valread);

        // Receiver'a yolla
        send(socket_receiver, buffer, valread, 0);
        printf("[Iletilen Paket]: %s\n", buffer);
    }

    /* Temizlik */
    close(socket_sender);
    close(socket_receiver);
    close(server_fd);
    return 0;
}


