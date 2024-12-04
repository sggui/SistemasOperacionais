#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t status;
    uint8_t chs_inicio[3];
    uint8_t tipo;
    uint8_t chs_fim[3];
    uint32_t lba_inicio;
    uint32_t setores;
} ParticaoInfo;

void exibir_particao(ParticaoInfo *particao, int index) {
    const char *status_particao = (particao->status == 0x80) ? "Ativa" : "Inativa";
    double tamanho_mb = (particao->setores * 512.0) / (1024 * 1024);

    printf("Detalhes da Partição %d:\n", index + 1);
    printf("- Status: %s\n", status_particao);
    printf("- Tipo: 0x%02X\n", particao->tipo);
    printf("- LBA Inicial: %u\n", particao->lba_inicio);
    printf("- Quantidade de Setores: %u\n", particao->setores);
    printf("- Tamanho Aproximado: %.2f MB\n", tamanho_mb);
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Modo de uso: %s <caminho_do_arquivo_mbr>\n", argv[0]);
        return 1;
    }

    FILE *mbr_file = fopen(argv[1], "rb");
    if (!mbr_file) {
        perror("Falha ao abrir o arquivo");
        return 1;
    }

    uint8_t mbr[512];
    if (fread(mbr, 1, 512, mbr_file) != 512) {
        perror("Falha ao ler o arquivo");
        fclose(mbr_file);
        return 1;
    }
    fclose(mbr_file);

    if (mbr[510] != 0x55 || mbr[511] != 0xAA) {
        fprintf(stderr, "Assinatura de inicialização inválida. Não é um MBR válido.\n");
        return 1;
    }

    ParticaoInfo *particoes = (ParticaoInfo *)(mbr + 446);

    for (int i = 0; i < 4; i++) {
        if (particoes[i].tipo != 0) {
            exibir_particao(&particoes[i], i);
        }
    }

    return 0;
}