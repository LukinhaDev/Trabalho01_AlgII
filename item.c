#include <stdio.h>   
#include "item.h"


const char *resultado_mensagem(Resultado r)
{
    switch (r) {
    case RES_OK:                    return "Operacao realizada com sucesso.";
    case RES_CADASTRO_CHEIO:        return "Cadastro cheio: nao ha posicao disponivel.";
    case RES_ID_DUPLICADO:          return "ID duplicado: ja existe um registro com esse identificador.";
    case RES_NAO_ENCONTRADO:        return "Personagem nao encontrado.";
    case RES_DADOS_INVALIDOS:       return "Dados invalidos: verifique os intervalos permitidos.";
    case RES_INVENTARIO_SEM_ESPACO: return "Inventario sem espaco suficiente.";
    case RES_ITEM_INCOMPATIVEL:     return "Item incompativel com a posicao escolhida.";
    case RES_ITEM_NAO_ENCONTRADO:   return "Item nao encontrado no inventario.";
    case RES_CONFLITO_DUAS_MAOS:    return "Conflito com arma de duas maos: libere as maos primeiro.";
    case RES_POSICAO_VAZIA:         return "A posicao escolhida esta vazia.";
    }
    return "Resultado desconhecido.";
}

int tipo_item_valido(int tipo)
{
    if (tipo >= 0 && tipo < TOTAL_TIPOS_ITEM) {
        return 1;
    }
    return 0;
}

const char *tipo_item_nome(TipoItem tipo)
{
    /* tem que ficar na mesma ordem do enum */
    const char *nomes[TOTAL_TIPOS_ITEM] = {
        "Elmo", "Peitoral", "Manoplas", "Calca", "Botas",
        "Anel", "Colar", "Cinto", "Arma de uma mao", "Arma de duas maos"
    };

    if (!tipo_item_valido(tipo)) {
        return "Desconhecido";
    }
    return nomes[tipo];
}

int nome_valido(const char nome[], int capacidade)
{
    int i = 0;
    int temLetra = 0;

    if (nome == NULL) {
        return 0;
    }

    /* vai até o '\0' contando o tamanho, mas sem passar da capacidade
     * pra não ler fora do vetor. Aproveita e vê se tem alguma letra */
    while (i < capacidade && nome[i] != '\0') {
        if (nome[i] != ' ' && nome[i] != '\t') {
            temLetra = 1;
        }
        i++;
    }

    if (i == capacidade) {
        return 0;   /* nem achou o '\0', então o nome é grande demais */
    }
    if (i == 0 || !temLetra) {
        return 0;
    }
    return 1;
}

Resultado item_validar(const Item *item)
{
    if (item == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    if (item->id <= 0) {
        return RES_DADOS_INVALIDOS;
    }
    if (!nome_valido(item->nome, ITEM_NOME_TAM)) {
        return RES_DADOS_INVALIDOS;
    }
    if (!tipo_item_valido(item->tipo)) {
        return RES_DADOS_INVALIDOS;
    }
    if (item->espacos < ITEM_ESPACOS_MIN || item->espacos > ITEM_ESPACOS_MAX) {
        return RES_DADOS_INVALIDOS;
    }
    if (item->poder < 0) {
        return RES_DADOS_INVALIDOS;
    }
    return RES_OK;
}
