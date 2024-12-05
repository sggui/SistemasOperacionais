#include <stdio.h>
#include <stddef.h>

#define TAMANHO_POOL 1024
#define TAMANHO_BLOCO 16

static unsigned char pool[TAMANHO_POOL];

typedef struct BlocoDeMemoria {
    size_t tamanho_bloco;
    struct BlocoDeMemoria* proximo_bloco;
    int livre;
} BlocoDeMemoria;

static BlocoDeMemoria* blocos_livres = (BlocoDeMemoria*)pool;

void* alocar_memoria(size_t tamanho_requisitado) {
    BlocoDeMemoria* bloco_atual = blocos_livres;

    while (bloco_atual) {
        if (bloco_atual->livre && bloco_atual->tamanho_bloco >= tamanho_requisitado) {
            bloco_atual->livre = 0;

            if (bloco_atual->tamanho_bloco > tamanho_requisitado + sizeof(BlocoDeMemoria)) {
                BlocoDeMemoria* novo_bloco = (BlocoDeMemoria*)((unsigned char*)bloco_atual + sizeof(BlocoDeMemoria) + tamanho_requisitado);
                novo_bloco->tamanho_bloco = bloco_atual->tamanho_bloco - tamanho_requisitado - sizeof(BlocoDeMemoria);
                novo_bloco->proximo_bloco = bloco_atual->proximo_bloco;
                novo_bloco->livre = 1;
                bloco_atual->proximo_bloco = novo_bloco;
                bloco_atual->tamanho_bloco = tamanho_requisitado;
            }

            return (unsigned char*)bloco_atual + sizeof(BlocoDeMemoria);
        }
        bloco_atual = bloco_atual->proximo_bloco;
    }
    return NULL;
}

void liberar_memoria(void* ponteiro) {
    if (!ponteiro) return;

    BlocoDeMemoria* bloco_a_liberar = (BlocoDeMemoria*)((unsigned char*)ponteiro - sizeof(BlocoDeMemoria));
    bloco_a_liberar->livre = 1;

    BlocoDeMemoria* bloco_atual = blocos_livres;
    while (bloco_atual) {
        if (bloco_atual->livre && bloco_atual->proximo_bloco && bloco_atual->proximo_bloco->livre) {
            bloco_atual->tamanho_bloco += bloco_atual->proximo_bloco->tamanho_bloco + sizeof(BlocoDeMemoria);
            bloco_atual->proximo_bloco = bloco_atual->proximo_bloco->proximo_bloco;
        }
        bloco_atual = bloco_atual->proximo_bloco;
    }
}

void configurar_pool() {
    blocos_livres->tamanho_bloco = TAMANHO_POOL - sizeof(BlocoDeMemoria);
    blocos_livres->proximo_bloco = NULL;
    blocos_livres->livre = 1;
}

void exibir_dados_bloco(int* ptr, size_t quantidade) {
    for (size_t i = 0; i < quantidade; i++) {
        printf("ptr[%zu] = %d, endereço = %p\n", i, ptr[i], (void*)&ptr[i]);
    }
}

int main() {
    configurar_pool();

    int* primeiro_bloco = (int*)alocar_memoria(5 * sizeof(int));
    int* segundo_bloco = (int*)alocar_memoria(5 * sizeof(int));

    if (!primeiro_bloco || !segundo_bloco) {
        printf("Falha na alocação de memória.\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        primeiro_bloco[i] = i * 2;
        segundo_bloco[i] = i * 3;
    }

    exibir_dados_bloco(primeiro_bloco, 5);

    if (primeiro_bloco < segundo_bloco) {
        printf("O bloco de 'primeiro_bloco' começa antes de 'segundo_bloco' na memória.\n");
    } else {
        printf("O bloco de 'segundo_bloco' começa antes de 'primeiro_bloco' na memória.\n");
    }

    liberar_memoria(primeiro_bloco);
    liberar_memoria(segundo_bloco);

    return 0;
}