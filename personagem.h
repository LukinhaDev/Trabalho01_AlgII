
#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include "item.h"
#include "inventario.h"

#define CAPACIDADE_PERSONAGENS 20
#define PERSONAGEM_NOME_TAM    50   

/* limites dos atributos que o enunciado pediu */
#define NIVEL_MIN        1
#define NIVEL_MAX       20
#define PV_MAX_MIN       1
#define PV_MAX_MAX     999
#define ATAQUE_MIN       0
#define ATAQUE_MAX      30
#define DEFESA_MIN       1
#define DEFESA_MAX      30
#define INICIATIVA_MIN  -5
#define INICIATIVA_MAX  20
#define PODER_MIN        1
#define PODER_MAX      100

/* As 4 raças que o enunciado pede.
 * O TOTAL_ no final é só pra saber quantos tem (mesma ideia do item.h). */
typedef enum {
    RACA_HUMANO,
    RACA_ELFO,
    RACA_ANAO,
    RACA_HALFLING,
    TOTAL_RACAS
} Raca;

/* Mesma coisa pras classes, só as 4 do enunciado. */
typedef enum {
    CLASSE_GUERREIRO,
    CLASSE_LADINO,
    CLASSE_MAGO,
    CLASSE_CLERIGO,
    TOTAL_CLASSES
} Classe;

/* Os lugares onde dá pra equipar coisa. Também servem de índice
 * no vetor de equipamentos do personagem. */
typedef enum {
    POS_ELMO,
    POS_PEITORAL,
    POS_MANOPLAS,
    POS_CALCA,
    POS_BOTAS,
    POS_ANEL,
    POS_COLAR,
    POS_CINTO,
    POS_MAO_DIREITA,
    POS_MAO_ESQUERDA,
    TOTAL_POSICOES
} PosicaoEquipamento;

typedef struct {
    Item item;
    int  ocupada;   /* 0 = vazio, 1 = tem item */
} SlotEquipamento;

/* A arma de duas mãos a gente sempre guarda na mão direita, e a
 * esquerda fica bloqueada enquanto ela tiver lá. Assim o item não
 * aparece duplicado nas duas mãos. */
typedef struct {
    int    id;
    char   nome[PERSONAGEM_NOME_TAM];
    Raca   raca;
    Classe classe;
    int    nivel;
    int    pvMax;        /* valores base, sem contar os itens */
    int    pvAtual;
    int    ataque;
    int    defesa;
    int    iniciativa;
    int    poder;

    Inventario      inventario;
    SlotEquipamento equipamentos[TOTAL_POSICOES];
} Personagem;

/* Não tem variável global, o main cria o cadastro e vai passando
  pras funções. */
typedef struct {
    Personagem itens[CAPACIDADE_PERSONAGENS];
    int        quantidade;
} CadastroPersonagens;

/* Base + bônus dos itens equipados. Isso não fica salvo na ficha,
  é calculado na hora que pedir. */
typedef struct {
    int ataque;
    int defesa;
    int iniciativa;
    int pvMax;
    int poder;
} AtributosTotais;

int         raca_valida(int raca);
const char *raca_nome(Raca raca);
int         classe_valida(int classe);
const char *classe_nome(Classe classe);
int         posicao_valida(int posicao);
const char *posicao_nome(PosicaoEquipamento pos);

int item_compativel_com_posicao(TipoItem tipo, PosicaoEquipamento pos);

/* deixa a ficha zerada, com inventário e equipamentos vazios */
void personagem_preparar(Personagem *p);

Resultado personagem_validar(const Personagem *p);



void cadastro_inicializar(CadastroPersonagens *c);

/* o personagem sempre entra sem itens */
Resultado cadastro_inserir(CadastroPersonagens *c, const Personagem *ficha);

const Personagem *cadastro_buscar(const CadastroPersonagens *c, int id);

/* Troca os dados da ficha mas mantém inventário e equipamentos.
 * Dá pra mudar o ID também, desde que não seja de outro personagem. */
Resultado cadastro_alterar(CadastroPersonagens *c, int id, const Personagem *novos);

Resultado cadastro_remover(CadastroPersonagens *c, int id);

int cadastro_quantidade(const CadastroPersonagens *c);

/* usado pra listar sem precisar mexer no vetor direto */
const Personagem *cadastro_obter(const CadastroPersonagens *c, int indice);



const Inventario *personagem_inventario(const Personagem *p);

/* o ID do item não pode repetir nem com os itens que tão equipados */
Resultado personagem_adicionar_item(CadastroPersonagens *c, int idPersonagem,
                                    const Item *item);

Resultado personagem_buscar_item(const CadastroPersonagens *c, int idPersonagem,
                                 int idItem, Item *saida);

Resultado personagem_remover_item(CadastroPersonagens *c, int idPersonagem,
                                  int idItem);


                                  
Resultado personagem_equipar(CadastroPersonagens *c, int idPersonagem,
                             int idItem, PosicaoEquipamento pos);

/* se não tiver espaço no inventário, o item continua equipado */
Resultado personagem_desequipar(CadastroPersonagens *c, int idPersonagem,
                                PosicaoEquipamento pos);

const Item *personagem_item_equipado(const Personagem *p, PosicaoEquipamento pos);

int personagem_mao_esquerda_bloqueada(const Personagem *p);

/* só os itens equipados contam, o que tá no inventário não dá bônus */
void personagem_atributos_totais(const Personagem *p, AtributosTotais *totais);

#endif
