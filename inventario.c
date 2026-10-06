#include <stdio.h>  
#include "inventario.h"

void inventario_inicializar(Inventario *inv)
{
    if (inv != NULL) {
        /* só zerar a quantidade já resolve, o resto do vetor nunca é lido */
        inv->quantidade = 0;
    }
}

int inventario_ocupacao(const Inventario *inv)
{
    int i;
    int soma = 0;

    if (inv == NULL) {
        return 0;
    }
    for (i = 0; i < inv->quantidade; i++) {
        soma = soma + inv->itens[i].espacos;
    }
    return soma;
}

int inventario_espacos_livres(const Inventario *inv)
{
    return INVENTARIO_CAPACIDADE - inventario_ocupacao(inv);
}

int inventario_quantidade(const Inventario *inv)
{
    if (inv == NULL) {
        return 0;
    }
    return inv->quantidade;
}

int inventario_cabe(const Inventario *inv, int espacos)
{
    /* é <= mesmo, ficar com 50/50 pode */
    if (inventario_ocupacao(inv) + espacos <= INVENTARIO_CAPACIDADE) {
        return 1;
    }
    return 0;
}

int inventario_buscar(const Inventario *inv, int idItem)
{
    int i;

    if (inv == NULL) {
        return -1;
    }
    for (i = 0; i < inv->quantidade; i++) {
        if (inv->itens[i].id == idItem) {
            return i;
        }
    }
    return -1;
}

const Item *inventario_obter(const Inventario *inv, int indice)
{
    if (inv == NULL || indice < 0 || indice >= inv->quantidade) {
        return NULL;
    }
    return &inv->itens[indice];
}

Resultado inventario_adicionar(Inventario *inv, const Item *item)
{
    Resultado r;

    if (inv == NULL) {
        return RES_DADOS_INVALIDOS;
    }

    /* primeiro confere tudo, só depois mexe no inventário,
     * assim se der erro nada muda */
    r = item_validar(item);
    if (r != RES_OK) {
        return r;
    }
    if (inventario_buscar(inv, item->id) >= 0) {
        return RES_ID_DUPLICADO;
    }
    if (!inventario_cabe(inv, item->espacos) ||
        inv->quantidade >= INVENTARIO_CAPACIDADE) {
        return RES_INVENTARIO_SEM_ESPACO;
    }

    inv->itens[inv->quantidade] = *item;
    inv->quantidade++;
    return RES_OK;
}

Resultado inventario_remover(Inventario *inv, int idItem, Item *removido)
{
    int i;
    int pos = inventario_buscar(inv, idItem);

    if (pos < 0) {
        return RES_ITEM_NAO_ENCONTRADO;
    }
    if (removido != NULL) {
        *removido = inv->itens[pos];
    }

    /* puxa todo mundo que tá depois uma casa pra esquerda
     * ex: [A][B][C][D] tirando o B fica [A][C][D] */
    for (i = pos; i < inv->quantidade - 1; i++) {
        inv->itens[i] = inv->itens[i + 1];
    }
    inv->quantidade--;
    return RES_OK;
}
