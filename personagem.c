/* Aqui fica o grosso do trabalho: cadastro, inventário do personagem,
 * equipar/desequipar e o cálculo dos atributos. */
#include <stdio.h>   /* só pro NULL */
#include "personagem.h"

/* Os vetores de nomes ficam na mesma ordem dos enums, aí é só usar
 * o valor do enum como índice. Deixamos eles dentro das funções pra
 * não precisar de variável global. */

int raca_valida(int raca)
{
    if (raca >= 0 && raca < TOTAL_RACAS) {
        return 1;
    }
    return 0;
}

int classe_valida(int classe)
{
    if (classe >= 0 && classe < TOTAL_CLASSES) {
        return 1;
    }
    return 0;
}

int posicao_valida(int posicao)
{
    if (posicao >= 0 && posicao < TOTAL_POSICOES) {
        return 1;
    }
    return 0;
}

const char *raca_nome(Raca raca)
{
    const char *nomes[TOTAL_RACAS] = {
        "Humano", "Elfo", "Anao", "Halfling"
    };
    if (!raca_valida(raca)) {
        return "Desconhecida";
    }
    return nomes[raca];
}

const char *classe_nome(Classe classe)
{
    const char *nomes[TOTAL_CLASSES] = {
        "Guerreiro", "Ladino", "Mago", "Clerigo"
    };
    if (!classe_valida(classe)) {
        return "Desconhecida";
    }
    return nomes[classe];
}

const char *posicao_nome(PosicaoEquipamento pos)
{
    const char *nomes[TOTAL_POSICOES] = {
        "Elmo", "Peitoral", "Manoplas", "Calca", "Botas",
        "Anel", "Colar", "Cinto", "Mao direita", "Mao esquerda"
    };
    if (!posicao_valida(pos)) {
        return "Desconhecida";
    }
    return nomes[pos];
}

int item_compativel_com_posicao(TipoItem tipo, PosicaoEquipamento pos)
{
    /* armadura e acessório só vão no lugar deles, arma pode ir em qualquer mão */
    switch (tipo) {
    case TIPO_ELMO:      return pos == POS_ELMO;
    case TIPO_PEITORAL:  return pos == POS_PEITORAL;
    case TIPO_MANOPLAS:  return pos == POS_MANOPLAS;
    case TIPO_CALCA:     return pos == POS_CALCA;
    case TIPO_BOTAS:     return pos == POS_BOTAS;
    case TIPO_ANEL:      return pos == POS_ANEL;
    case TIPO_COLAR:     return pos == POS_COLAR;
    case TIPO_CINTO:     return pos == POS_CINTO;
    case TIPO_ARMA_UMA_MAO:
    case TIPO_ARMA_DUAS_MAOS:
        return pos == POS_MAO_DIREITA || pos == POS_MAO_ESQUERDA;
    case TOTAL_TIPOS_ITEM:
        break;
    }
    return 0;
}

/* Daqui pra baixo são funções de apoio que só esse arquivo usa,
 * por isso não colocamos no personagem.h. */

/* devolve a posição do personagem no vetor ou -1 */
int buscar_indice(const CadastroPersonagens *c, int id)
{
    int i;
    for (i = 0; i < c->quantidade; i++) {
        if (c->itens[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Parecida com a cadastro_buscar, só que essa deixa alterar a ficha.
 * Não tá no .h de propósito, pra só esse arquivo conseguir mexer nas fichas. */
Personagem *buscar_para_alterar(CadastroPersonagens *c, int id)
{
    int i = buscar_indice(c, id);
    if (i < 0) {
        return NULL;
    }
    return &c->itens[i];
}

/* copia só os dados da ficha, inventário e equipamentos ficam como estão */
void copiar_dados_basicos(Personagem *destino, const Personagem *origem)
{
    int i;

    destino->id = origem->id;
    /* copia o nome letra por letra e fecha com '\0' */
    i = 0;
    while (origem->nome[i] != '\0') {
        destino->nome[i] = origem->nome[i];
        i++;
    }
    destino->nome[i] = '\0';

    destino->raca       = origem->raca;
    destino->classe     = origem->classe;
    destino->nivel      = origem->nivel;
    destino->pvMax      = origem->pvMax;
    destino->pvAtual    = origem->pvAtual;
    destino->ataque     = origem->ataque;
    destino->defesa     = origem->defesa;
    destino->iniciativa = origem->iniciativa;
    destino->poder      = origem->poder;
}

/* a arma de duas mãos sempre fica na direita, então é só olhar lá */
int tem_arma_duas_maos(const Personagem *p)
{
    if (p->equipamentos[POS_MAO_DIREITA].ocupada &&
        p->equipamentos[POS_MAO_DIREITA].item.tipo == TIPO_ARMA_DUAS_MAOS) {
        return 1;
    }
    return 0;
}

/* vê se o ID já existe no inventário ou em algum item equipado */
int id_item_em_uso(const Personagem *p, int idItem)
{
    int i;
    if (inventario_buscar(&p->inventario, idItem) >= 0) {
        return 1;
    }
    for (i = 0; i < TOTAL_POSICOES; i++) {
        if (p->equipamentos[i].ocupada && p->equipamentos[i].item.id == idItem) {
            return 1;
        }
    }
    return 0;
}

/* só pra validação ficar mais fácil de ler */
int entre(int valor, int minimo, int maximo)
{
    if (valor >= minimo && valor <= maximo) {
        return 1;
    }
    return 0;
}

void personagem_preparar(Personagem *p)
{
    int i;

    if (p == NULL) {
        return;
    }
    p->id = 0;
    p->nome[0] = '\0';
    p->raca = RACA_HUMANO;
    p->classe = CLASSE_GUERREIRO;
    p->nivel = 0;
    p->pvMax = 0;
    p->pvAtual = 0;
    p->ataque = 0;
    p->defesa = 0;
    p->iniciativa = 0;
    p->poder = 0;

    inventario_inicializar(&p->inventario);
    for (i = 0; i < TOTAL_POSICOES; i++) {
        p->equipamentos[i].ocupada = 0;
    }
}

Resultado personagem_validar(const Personagem *p)
{
    if (p == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    if (p->id <= 0 ||
        !nome_valido(p->nome, PERSONAGEM_NOME_TAM) ||
        !raca_valida(p->raca) ||
        !classe_valida(p->classe) ||
        !entre(p->nivel, NIVEL_MIN, NIVEL_MAX) ||
        !entre(p->pvMax, PV_MAX_MIN, PV_MAX_MAX) ||
        !entre(p->pvAtual, 0, p->pvMax) ||          /* PV atual não pode passar do máximo */
        !entre(p->ataque, ATAQUE_MIN, ATAQUE_MAX) ||
        !entre(p->defesa, DEFESA_MIN, DEFESA_MAX) ||
        !entre(p->iniciativa, INICIATIVA_MIN, INICIATIVA_MAX) ||
        !entre(p->poder, PODER_MIN, PODER_MAX)) {
        return RES_DADOS_INVALIDOS;
    }
    return RES_OK;
}

/* ----- cadastro ----- */

void cadastro_inicializar(CadastroPersonagens *c)
{
    if (c != NULL) {
        c->quantidade = 0;
    }
}

Resultado cadastro_inserir(CadastroPersonagens *c, const Personagem *ficha)
{
    Personagem *novo;
    Resultado r;

    if (c == NULL) {
        return RES_DADOS_INVALIDOS;
    }

    /* confere os dados, depois o ID, depois se tem vaga.
     * Só mexe no cadastro se passar em tudo */
    r = personagem_validar(ficha);
    if (r != RES_OK) {
        return r;
    }
    if (buscar_indice(c, ficha->id) >= 0) {
        return RES_ID_DUPLICADO;
    }
    if (c->quantidade >= CAPACIDADE_PERSONAGENS) {
        return RES_CADASTRO_CHEIO;
    }

    /* entra no final da lista */
    novo = &c->itens[c->quantidade];
    personagem_preparar(novo);
    copiar_dados_basicos(novo, ficha);
    c->quantidade++;
    return RES_OK;
}

const Personagem *cadastro_buscar(const CadastroPersonagens *c, int id)
{
    int i;
    if (c == NULL) {
        return NULL;
    }
    i = buscar_indice(c, id);
    if (i < 0) {
        return NULL;
    }
    return &c->itens[i];
}

Resultado cadastro_alterar(CadastroPersonagens *c, int id, const Personagem *novos)
{
    Personagem *atual;
    Resultado r;

    if (c == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    atual = buscar_para_alterar(c, id);
    if (atual == NULL) {
        return RES_NAO_ENCONTRADO;
    }

    /* não deixa trocar uma ficha boa por uma inválida */
    r = personagem_validar(novos);
    if (r != RES_OK) {
        return r;
    }
    /* se mudou o ID, ele não pode ser de outro personagem */
    if (novos->id != id && buscar_indice(c, novos->id) >= 0) {
        return RES_ID_DUPLICADO;
    }

    copiar_dados_basicos(atual, novos);
    return RES_OK;
}

Resultado cadastro_remover(CadastroPersonagens *c, int id)
{
    int i;
    int pos;

    if (c == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    pos = buscar_indice(c, id);
    if (pos < 0) {
        return RES_NAO_ENCONTRADO;
    }
    /* puxa os próximos pra esquerda pra não ficar buraco, igual no inventário */
    for (i = pos; i < c->quantidade - 1; i++) {
        c->itens[i] = c->itens[i + 1];
    }
    c->quantidade--;
    return RES_OK;
}

int cadastro_quantidade(const CadastroPersonagens *c)
{
    if (c == NULL) {
        return 0;
    }
    return c->quantidade;
}

const Personagem *cadastro_obter(const CadastroPersonagens *c, int indice)
{
    if (c == NULL || indice < 0 || indice >= c->quantidade) {
        return NULL;
    }
    return &c->itens[indice];
}

/* ----- inventário do personagem ----- */

const Inventario *personagem_inventario(const Personagem *p)
{
    if (p == NULL) {
        return NULL;
    }
    return &p->inventario;
}

Resultado personagem_adicionar_item(CadastroPersonagens *c, int idPersonagem,
                                    const Item *item)
{
    Personagem *p;
    Resultado r;

    if (c == NULL || item == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    p = buscar_para_alterar(c, idPersonagem);
    if (p == NULL) {
        return RES_NAO_ENCONTRADO;
    }
    r = item_validar(item);
    if (r != RES_OK) {
        return r;
    }
    /* tem que checar os equipados também, porque o inventario_adicionar
     * sozinho só olha o que tá guardado */
    if (id_item_em_uso(p, item->id)) {
        return RES_ID_DUPLICADO;
    }
    return inventario_adicionar(&p->inventario, item);
}

Resultado personagem_buscar_item(const CadastroPersonagens *c, int idPersonagem,
                                 int idItem, Item *saida)
{
    const Personagem *p = cadastro_buscar(c, idPersonagem);
    int indice;

    if (p == NULL) {
        return RES_NAO_ENCONTRADO;
    }
    indice = inventario_buscar(&p->inventario, idItem);
    if (indice < 0) {
        return RES_ITEM_NAO_ENCONTRADO;
    }
    if (saida != NULL) {
        *saida = *inventario_obter(&p->inventario, indice);
    }
    return RES_OK;
}

Resultado personagem_remover_item(CadastroPersonagens *c, int idPersonagem,
                                  int idItem)
{
    Personagem *p;

    if (c == NULL) {
        return RES_DADOS_INVALIDOS;
    }
    p = buscar_para_alterar(c, idPersonagem);
    if (p == NULL) {
        return RES_NAO_ENCONTRADO;
    }
    return inventario_remover(&p->inventario, idItem, NULL);
}

/* ----- equipamentos ----- */

Resultado personagem_equipar(CadastroPersonagens *c, int idPersonagem,
                             int idItem, PosicaoEquipamento pos)
{
    Personagem *p;
    SlotEquipamento *destino;
    Item novo;
    int indice;
    int ehMao;
    int ocupacaoFinal;

    if (c == NULL || !posicao_valida(pos)) {
        return RES_DADOS_INVALIDOS;
    }
    p = buscar_para_alterar(c, idPersonagem);
    if (p == NULL) {
        return RES_NAO_ENCONTRADO;
    }

    /* primeiro só confere as coisas, não muda nada ainda */

    indice = inventario_buscar(&p->inventario, idItem);
    if (indice < 0) {
        return RES_ITEM_NAO_ENCONTRADO;
    }
    novo = *inventario_obter(&p->inventario, indice);

    if (!item_compativel_com_posicao(novo.tipo, pos)) {
        return RES_ITEM_INCOMPATIVEL;
    }

    ehMao = 0;
    if (pos == POS_MAO_DIREITA || pos == POS_MAO_ESQUERDA) {
        ehMao = 1;
    }

    if (novo.tipo == TIPO_ARMA_DUAS_MAOS) {
        pos = POS_MAO_DIREITA;
        /* A gente decidiu que pra pegar arma de duas mãos as mãos têm que
         * tá livres. Só deixa trocar se já for outra de duas mãos. */
        if (p->equipamentos[POS_MAO_ESQUERDA].ocupada) {
            return RES_CONFLITO_DUAS_MAOS;
        }
        if (p->equipamentos[POS_MAO_DIREITA].ocupada && !tem_arma_duas_maos(p)) {
            return RES_CONFLITO_DUAS_MAOS;
        }
    } else if (ehMao && tem_arma_duas_maos(p)) {
        /* com arma de duas mãos não entra mais nenhuma outra */
        return RES_CONFLITO_DUAS_MAOS;
    }

    destino = &p->equipamentos[pos];

    /* Se for troca, vê antes se o item antigo vai caber de volta.
     * Na conta já tira o espaço do item novo, que vai sair do inventário. */
    if (destino->ocupada) {
        ocupacaoFinal = inventario_ocupacao(&p->inventario)
                        - novo.espacos + destino->item.espacos;
        if (ocupacaoFinal > INVENTARIO_CAPACIDADE) {
            return RES_INVENTARIO_SEM_ESPACO;
        }
    }

    /* Se chegou aqui passou em tudo, então agora pode mexer sem medo,
     * nada disso vai falhar no meio do caminho. */
    inventario_remover(&p->inventario, idItem, NULL);
    if (destino->ocupada) {
        inventario_adicionar(&p->inventario, &destino->item);  /* o antigo volta pro inventário */
    }
    destino->item = novo;
    destino->ocupada = 1;
    return RES_OK;
}

Resultado personagem_desequipar(CadastroPersonagens *c, int idPersonagem,
                                PosicaoEquipamento pos)
{
    Personagem *p;
    SlotEquipamento *slot;
    Resultado r;

    if (c == NULL || !posicao_valida(pos)) {
        return RES_DADOS_INVALIDOS;
    }
    p = buscar_para_alterar(c, idPersonagem);
    if (p == NULL) {
        return RES_NAO_ENCONTRADO;
    }

    /* se pedir pra tirar da mão esquerda e ela tiver bloqueada,
     * na real é pra tirar a arma de duas mãos que tá na direita */
    if (pos == POS_MAO_ESQUERDA && tem_arma_duas_maos(p)) {
        pos = POS_MAO_DIREITA;
    }

    slot = &p->equipamentos[pos];
    if (!slot->ocupada) {
        return RES_POSICAO_VAZIA;
    }

    /* se não couber no inventário dá erro e o item fica equipado mesmo */
    r = inventario_adicionar(&p->inventario, &slot->item);
    if (r != RES_OK) {
        return r;
    }
    slot->ocupada = 0;
    return RES_OK;
}

const Item *personagem_item_equipado(const Personagem *p, PosicaoEquipamento pos)
{
    if (p == NULL || !posicao_valida(pos) || !p->equipamentos[pos].ocupada) {
        return NULL;
    }
    return &p->equipamentos[pos].item;
}

int personagem_mao_esquerda_bloqueada(const Personagem *p)
{
    if (p == NULL) {
        return 0;
    }
    return tem_arma_duas_maos(p);
}

/* ----- atributos ----- */

void personagem_atributos_totais(const Personagem *p, AtributosTotais *totais)
{
    int i;

    if (p == NULL || totais == NULL) {
        return;
    }

    /* começa com os valores da ficha */
    totais->ataque     = p->ataque;
    totais->defesa     = p->defesa;
    totais->iniciativa = p->iniciativa;
    totais->pvMax      = p->pvMax;
    totais->poder      = p->poder;

    /* e soma o bônus de tudo que tá equipado (inventário não conta) */
    for (i = 0; i < TOTAL_POSICOES; i++) {
        if (p->equipamentos[i].ocupada) {
            totais->ataque     += p->equipamentos[i].item.bonusAtaque;
            totais->defesa     += p->equipamentos[i].item.bonusDefesa;
            totais->iniciativa += p->equipamentos[i].item.bonusIniciativa;
            totais->pvMax      += p->equipamentos[i].item.bonusVida;
            totais->poder      += p->equipamentos[i].item.poder;
        }
    }
}
