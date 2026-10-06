#ifndef ITEM_H
#define ITEM_H

#define ITEM_NOME_TAM     50
#define ITEM_ESPACOS_MIN   1
#define ITEM_ESPACOS_MAX  50


typedef enum {
    RES_OK,
    RES_CADASTRO_CHEIO,
    RES_ID_DUPLICADO,
    RES_NAO_ENCONTRADO,
    RES_DADOS_INVALIDOS,
    RES_INVENTARIO_SEM_ESPACO,
    RES_ITEM_INCOMPATIVEL,
    RES_ITEM_NAO_ENCONTRADO,
    RES_CONFLITO_DUAS_MAOS,
    RES_POSICAO_VAZIA           /* quando tenta desequipar um lugar que já tá vazio */
} Resultado;

/* O tipo do item é o que decide onde ele pode ser equipado.
 * O TOTAL_TIPOS_ITEM no final não é um tipo de verdade, a gente usou
 * ele só pra saber quantos tipos existem e facilitar a validação. */
typedef enum {
    TIPO_ELMO,
    TIPO_PEITORAL,
    TIPO_MANOPLAS,
    TIPO_CALCA,
    TIPO_BOTAS,
    TIPO_ANEL,
    TIPO_COLAR,
    TIPO_CINTO,
    TIPO_ARMA_UMA_MAO,
    TIPO_ARMA_DUAS_MAOS,
    TOTAL_TIPOS_ITEM
} TipoItem;

typedef struct {
    int      id;
    char     nome[ITEM_NOME_TAM];
    TipoItem tipo;
    int      espacos;           
    int      bonusAtaque;           
    int      bonusDefesa;
    int      bonusVida;
    int      bonusIniciativa;
    int      poder;            
} Item;

const char *resultado_mensagem(Resultado r);

int tipo_item_valido(int tipo);

const char *tipo_item_nome(TipoItem tipo);

/* Usada tanto pro nome do item quanto pro nome do personagem.
 * Recusa nome vazio, só com espaços ou que não cabe no vetor. */
int nome_valido(const char nome[], int capacidade);

Resultado item_validar(const Item *item);

#endif
