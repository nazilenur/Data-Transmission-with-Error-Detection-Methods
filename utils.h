#ifndef UTILS_H
#define UTILS_H

// CRC-16 Hesaplama Fonksiyonu
unsigned short calculate_crc16(const char* data, int length);

// YENİ: Parity (Tek/Çift) Hesaplama Fonksiyonu
int calculate_parity(const char* data, int length);

#endif
