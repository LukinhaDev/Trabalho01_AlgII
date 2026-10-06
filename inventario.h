#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "item.h"

/* O inventário aguenta 50 espaços. Como todo item ocupa pelo menos 1,
 * no máximo cabem 50 itens, por isso o vetor tem 50 posições. */
#define INVENTARIO_CAPACIDADE 50

typedef struct {
    Item itens[INVENTARIO_CAPACIDADE];  /* os itens ficam sempre juntos no começo do vetor */
    int  quantidade;
} Inventario;

void inventario_inicializar(Inventario *inv);

/* soma os espaços de todos os itens guardados */
int inventario_ocupacao(const Inventario *inv);

int inventario_espacos_livres(const Inventario *inv);

int inventario_quantidade(const Inventario *inv);

int inventario_cabe(const Inventario *inv, int espacos);

/* Coloca o item no final. Se der qualquer erro o inventário fica como estava. */
Resultado inventario_adicionar(Inventario *inv, const Item *item);

/* devolve a posição do item no vetor ou -1 se não achar */
int inventario_buscar(const Inventario *inv, int idItem);

const Item *inventario_obter(const Inventario *inv, int indice);

/* Tira o item e puxa os outros pra esquerda pra não ficar buraco.
 * Se passar 'removido', recebe uma cópia do que saiu. */
Resultado inventario_remover(Inventario *inv, int idItem, Item *removido);

#endif
