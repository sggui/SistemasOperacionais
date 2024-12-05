#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t status;
    uint8_t chs_inicio[3];
    uint8_t tipo;
    uint8_t chs_fim[3];
    uint32_t lba_inicio;
    uint32_t setores;
} ParticaoInfo;

void criar_mbr(const char *arquivo_saida) {
    // Criar o MBR, que é um arquivo de 512 bytes
    uint8_t mbr[512] = {0};

    // Definir a tabela de partições (a partir do byte 446 até 462)
    ParticaoInfo particoes[4] = {
        {0x80, {0, 0, 0}, 0x07, {0, 0, 0}, 2048, 204800},  // Partição 1 (NTFS)
        {0x00, {0, 0, 0}, 0x0B, {0, 0, 0}, 4096, 102400},  // Partição 2 (FAT32)
        {0x00, {0, 0, 0}, 0x83, {0, 0, 0}, 8192, 102400},  // Partição 3 (Linux)
        {0x00, {0, 0, 0}, 0x00, {0, 0, 0}, 0, 0}            // Partição 4 (não usada)
    };

    // Copiar as partições para a tabela de partições do MBR
    memcpy(mbr + 446, particoes, sizeof(particoes));

    // Definir a assinatura do MBR (últimos 2 bytes: 0xAA55)
    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    // Criar o arquivo de saída e escrever o MBR
    FILE *arquivo = fopen(arquivo_saida, "wb");
    if (arquivo == NULL) {
        perror("Erro ao criar o arquivo");
        exit(1);
    }

    fwrite(mbr, sizeof(mbr), 1, arquivo);
    fclose(arquivo);

    printf("Arquivo MBR criado com sucesso: %s\n", arquivo_saida);
}

int main() {
    const char *nome_arquivo = "mbr.bin";
    criar_mbr(nome_arquivo);
    return 0;
}

