
#include <stdio.h>
#include <string.h>   
#include "personagem.h"

#define TAM_LINHA 100    


int ler_linha(char texto[], int tamanho)
{
    int len;
    int c;

    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return -1;
    }

    len = strlen(texto);
    if (len > 0 && texto[len - 1] == '\n') {
        texto[len - 1] = '\0';        /* tira o '\n' do final */
        return 1;
    }

    /* se não veio o '\n', ou encheu o vetor ou acabou a entrada,
     * aí olha o próximo caractere pra saber qual foi */
    c = getchar();
    if (c == '\n' || c == EOF) {
        return 1;
    }
    /* sobrou texto, então joga fora o resto da linha senão
     * ele aparece na próxima leitura */
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
    return 0;
}


int texto_para_inteiro(const char texto[], int *valor)
{
    int i = 0;
    int negativo = 0;
    int digitos = 0;
    int resultado = 0;

    while (texto[i] == ' ') {      
        i++;
    }
    if (texto[i] == '-') {
        negativo = 1;
        i++;
    } else if (texto[i] == '+') {
        i++;
    }
    while (texto[i] >= '0' && texto[i] <= '9') {
        if (digitos == 9) {
            return 0;             
        }
        resultado = resultado * 10 + (texto[i] - '0');
        digitos++;
        i++;
    }
    while (texto[i] == ' ') {      
        i++;
    }
    if (digitos == 0 || texto[i] != '\0') {
        return 0;                  
    }
    if (negativo) {
        resultado = -resultado;
    }
    *valor = resultado;
    return 1;
}

/* Fica perguntando até a pessoa digitar um número.
 * Aqui só vê se é número mesmo, se tá no intervalo certo
 * quem confere é o módulo. */
int ler_inteiro(const char mensagem[])
{
    char texto[TAM_LINHA];
    int valor = 0;
    int status;

    while (1) {
        printf("%s", mensagem);
        status = ler_linha(texto, TAM_LINHA);
        if (status == -1) {
            /* acabou a entrada, devolve 0 pra os menus fecharem */
            printf("\nFim da entrada.\n");
            return 0;
        }
        if (status == 1 && texto_para_inteiro(texto, &valor)) {
            return valor;
        }
        printf("Entrada invalida: digite um numero inteiro.\n");
    }
}

/* Se o nome não couber, deixa ele vazio que a validação
 * do módulo recusa depois. */
int ler_nome(const char mensagem[], char destino[], int capacidade)
{
    printf("%s", mensagem);
    if (ler_linha(destino, capacidade) != 1) {
        printf("Nome longo demais (maximo %d caracteres).\n", capacidade - 1);
        destino[0] = '\0';
        return 0;
    }
    return 1;
}

void pausar(void)
{
    char lixo[TAM_LINHA];
    printf("\nPressione ENTER para continuar...");
    ler_linha(lixo, TAM_LINHA);
}

void mostrar_resultado(Resultado r)
{
    printf(">> %s\n", resultado_mensagem(r));
}

/* funções que só imprimem coisa na tela */

void listar_opcoes_racas(void)
{
    int i;
    printf("Racas disponiveis:\n");
    for (i = 0; i < TOTAL_RACAS; i++) {
        printf("  %d - %s\n", i, raca_nome(i));
    }
}

void listar_opcoes_classes(void)
{
    int i;
    printf("Classes disponiveis:\n");
    for (i = 0; i < TOTAL_CLASSES; i++) {
        printf("  %d - %s\n", i, classe_nome(i));
    }
}

void listar_opcoes_tipos(void)
{
    int i;
    printf("Tipos de item:\n");
    for (i = 0; i < TOTAL_TIPOS_ITEM; i++) {
        printf("  %d - %s\n", i, tipo_item_nome(i));
    }
}

void listar_opcoes_posicoes(void)
{
    int i;
    printf("Posicoes:\n");
    for (i = 0; i < TOTAL_POSICOES; i++) {
        printf("  %d - %s\n", i, posicao_nome(i));
    }
}

void imprimir_ficha(const Personagem *p)
{
    const Inventario *inv = personagem_inventario(p);

    printf("----------------------------------------\n");
    printf("ID: %d | Nome: %s\n", p->id, p->nome);
    printf("Raca: %s | Classe: %s | Nivel: %d\n",
           raca_nome(p->raca), classe_nome(p->classe), p->nivel);
    printf("PV: %d/%d | Ataque: %d | Defesa: %d | Iniciativa: %d | Poder: %d\n",
           p->pvAtual, p->pvMax, p->ataque, p->defesa, p->iniciativa, p->poder);
    printf("Inventario: %d item(ns), %d/%d espacos\n",
           inventario_quantidade(inv), inventario_ocupacao(inv), INVENTARIO_CAPACIDADE);
}

void imprimir_item(const Item *it)
{
    printf("  [%d] %s | %s | espacos: %d | atq %d, def %d, vida %d, ini %d | poder %d\n",
           it->id, it->nome, tipo_item_nome(it->tipo), it->espacos,
           it->bonusAtaque, it->bonusDefesa, it->bonusVida,
           it->bonusIniciativa, it->poder);
}

void imprimir_inventario(const Personagem *p)
{
    const Inventario *inv = personagem_inventario(p);
    int i;

    printf("Inventario de %s:\n", p->nome);
    if (inventario_quantidade(inv) == 0) {
        printf("  (vazio)\n");
    }
    /* usa as funções do módulo em vez de acessar o vetor direto */
    for (i = 0; i < inventario_quantidade(inv); i++) {
        imprimir_item(inventario_obter(inv, i));
    }
    printf("Ocupados: %d/%d | Livres: %d\n", inventario_ocupacao(inv),
           INVENTARIO_CAPACIDADE, inventario_espacos_livres(inv));
}

void imprimir_equipamentos(const Personagem *p)
{
    int i;
    const Item *it;

    printf("Equipamentos de %s:\n", p->nome);
    for (i = 0; i < TOTAL_POSICOES; i++) {
        it = personagem_item_equipado(p, i);

        printf("  %s: ", posicao_nome(i));
        if (i == POS_MAO_ESQUERDA && personagem_mao_esquerda_bloqueada(p)) {
            printf("(bloqueada pela arma de duas maos)\n");
        } else if (it == NULL) {
            printf("(vazio)\n");
        } else {
            printf("[%d] %s\n", it->id, it->nome);
        }
    }
}

/* devolve 0 se o nome não coube */
int ler_ficha(Personagem *p)
{
    int valor;

    personagem_preparar(p);

    p->id = ler_inteiro("ID (inteiro positivo): ");
    if (!ler_nome("Nome (1 a 49 caracteres): ", p->nome, PERSONAGEM_NOME_TAM)) {
        return 0;
    }

    listar_opcoes_racas();
    valor = ler_inteiro("Raca: ");
    /* se digitar uma raça que não existe, coloca TOTAL_RACAS que é
     * inválido, aí o módulo recusa a ficha (mesma coisa na classe) */
    if (raca_valida(valor)) {
        p->raca = valor;
    } else {
        p->raca = TOTAL_RACAS;
    }

    listar_opcoes_classes();
    valor = ler_inteiro("Classe: ");
    if (classe_valida(valor)) {
        p->classe = valor;
    } else {
        p->classe = TOTAL_CLASSES;
    }

    p->nivel      = ler_inteiro("Nivel (1 a 20): ");
    p->pvMax      = ler_inteiro("PV maximos (1 a 999): ");
    p->pvAtual    = ler_inteiro("PV atuais (0 ate os PV maximos): ");
    p->ataque     = ler_inteiro("Ataque (0 a 30): ");
    p->defesa     = ler_inteiro("Defesa (1 a 30): ");
    p->iniciativa = ler_inteiro("Iniciativa (-5 a 20): ");
    p->poder      = ler_inteiro("Poder (1 a 100): ");
    return 1;
}

/* mesma ideia do ler_ficha, só que pro item */
int ler_item(Item *it)
{
    int valor;

    it->id = ler_inteiro("ID do item (inteiro positivo): ");
    if (!ler_nome("Nome do item (1 a 49 caracteres): ", it->nome, ITEM_NOME_TAM)) {
        return 0;
    }
    listar_opcoes_tipos();
    valor = ler_inteiro("Tipo: ");
    if (tipo_item_valido(valor)) {
        it->tipo = valor;
    } else {
        it->tipo = TOTAL_TIPOS_ITEM;
    }

    it->espacos         = ler_inteiro("Espacos ocupados (1 a 50): ");
    it->bonusAtaque     = ler_inteiro("Bonus de ataque: ");
    it->bonusDefesa     = ler_inteiro("Bonus de defesa: ");
    it->bonusVida       = ler_inteiro("Bonus de vida: ");
    it->bonusIniciativa = ler_inteiro("Bonus de iniciativa: ");
    it->poder           = ler_inteiro("Poder (0 ou mais): ");
    return 1;
}

/* pede o ID e já avisa se não achar o personagem */
const Personagem *pedir_personagem(const CadastroPersonagens *c)
{
    int id = ler_inteiro("ID do personagem: ");
    const Personagem *p = cadastro_buscar(c, id);
    if (p == NULL) {
        mostrar_resultado(RES_NAO_ENCONTRADO);
    }
    return p;
}

/* cada opção do menu principal tem sua função acao_ */

void acao_cadastrar(CadastroPersonagens *c)
{
    Personagem ficha;

    printf("\n=== Cadastrar personagem ===\n");
    if (!ler_ficha(&ficha)) {
        mostrar_resultado(RES_DADOS_INVALIDOS);
        return;
    }
    mostrar_resultado(cadastro_inserir(c, &ficha));
    printf("Personagens cadastrados: %d/%d\n",
           cadastro_quantidade(c), CAPACIDADE_PERSONAGENS);
}

void acao_consultar(const CadastroPersonagens *c)
{
    const Personagem *p;

    printf("\n=== Consultar personagem ===\n");
    p = pedir_personagem(c);
    if (p != NULL) {
        imprimir_ficha(p);
    }
}

void acao_alterar(CadastroPersonagens *c)
{
    Personagem novos;
    const Personagem *p;
    int idAtual;

    printf("\n=== Alterar personagem ===\n");
    p = pedir_personagem(c);
    if (p == NULL) {
        return;
    }
    idAtual = p->id;
    printf("Ficha atual:\n");
    imprimir_ficha(p);
    printf("Digite os NOVOS dados (inventario e equipamentos sao mantidos):\n");
    if (!ler_ficha(&novos)) {
        mostrar_resultado(RES_DADOS_INVALIDOS);
        return;
    }
    mostrar_resultado(cadastro_alterar(c, idAtual, &novos));
}

void acao_remover(CadastroPersonagens *c)
{
    int id;

    printf("\n=== Remover personagem ===\n");
    id = ler_inteiro("ID do personagem: ");
    mostrar_resultado(cadastro_remover(c, id));
}

void acao_listar(const CadastroPersonagens *c)
{
    int i;

    printf("\n=== Personagens (%d/%d) ===\n",
           cadastro_quantidade(c), CAPACIDADE_PERSONAGENS);
    if (cadastro_quantidade(c) == 0) {
        printf("Nenhum personagem cadastrado.\n");
    }
    for (i = 0; i < cadastro_quantidade(c); i++) {
        imprimir_ficha(cadastro_obter(c, i));
    }
}

/* submenu do inventário (opção 6) */
void acao_inventario(CadastroPersonagens *c)
{
    const Personagem *p;
    const Inventario *inv;
    Item item;
    Resultado r;
    int idPers;
    int opcao;

    printf("\n=== Administrar inventario ===\n");
    p = pedir_personagem(c);
    if (p == NULL) {
        return;
    }
    idPers = p->id;
    inv = personagem_inventario(p);

    do {
        printf("\n--- Inventario de %s ---\n", p->nome);
        printf("1 - Adicionar item\n");
        printf("2 - Buscar item por ID\n");
        printf("3 - Remover item\n");
        printf("4 - Listar itens\n");
        printf("5 - Ver ocupacao\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
        case 1:
            if (!ler_item(&item)) {
                mostrar_resultado(RES_DADOS_INVALIDOS);
            } else {
                mostrar_resultado(personagem_adicionar_item(c, idPers, &item));
            }
            break;
        case 2:
            r = personagem_buscar_item(c, idPers, ler_inteiro("ID do item: "), &item);
            if (r == RES_OK) {
                imprimir_item(&item);
            } else {
                mostrar_resultado(r);
            }
            break;
        case 3:
            mostrar_resultado(personagem_remover_item(c, idPers,
                                                      ler_inteiro("ID do item: ")));
            break;
        case 4:
            imprimir_inventario(p);
            break;
        case 5:
            printf("Ocupacao: %d/%d espacos (%d livres)\n",
                   inventario_ocupacao(inv), INVENTARIO_CAPACIDADE,
                   inventario_espacos_livres(inv));
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

void acao_consultar_equipamentos(const CadastroPersonagens *c)
{
    const Personagem *p;

    printf("\n=== Equipamentos ===\n");
    p = pedir_personagem(c);
    if (p != NULL) {
        imprimir_equipamentos(p);
    }
}

void acao_equipar(CadastroPersonagens *c)
{
    const Personagem *p;
    int idItem;
    int pos;

    printf("\n=== Equipar item ===\n");
    p = pedir_personagem(c);
    if (p == NULL) {
        return;
    }
    imprimir_inventario(p);
    idItem = ler_inteiro("ID do item a equipar: ");
    listar_opcoes_posicoes();
    pos = ler_inteiro("Posicao: ");
    if (!posicao_valida(pos)) {
        mostrar_resultado(RES_DADOS_INVALIDOS);
        return;
    }
    mostrar_resultado(personagem_equipar(c, p->id, idItem, pos));
}

void acao_desequipar(CadastroPersonagens *c)
{
    const Personagem *p;
    int pos;

    printf("\n=== Desequipar item ===\n");
    p = pedir_personagem(c);
    if (p == NULL) {
        return;
    }
    imprimir_equipamentos(p);
    listar_opcoes_posicoes();
    pos = ler_inteiro("Posicao a desequipar: ");
    if (!posicao_valida(pos)) {
        mostrar_resultado(RES_DADOS_INVALIDOS);
        return;
    }
    mostrar_resultado(personagem_desequipar(c, p->id, pos));
}

void acao_atributos(const CadastroPersonagens *c)
{
    const Personagem *p;
    AtributosTotais t;

    printf("\n=== Atributos totais ===\n");
    p = pedir_personagem(c);
    if (p == NULL) {
        return;
    }
    personagem_atributos_totais(p, &t);
    printf("Atributo      Base   Total\n");
    printf("Ataque       %5d   %5d\n", p->ataque, t.ataque);
    printf("Defesa       %5d   %5d\n", p->defesa, t.defesa);
    printf("Iniciativa   %5d   %5d\n", p->iniciativa, t.iniciativa);
    printf("PV maximos   %5d   %5d\n", p->pvMax, t.pvMax);
    printf("Poder        %5d   %5d\n", p->poder, t.poder);
    printf("(Somente itens equipados contam; itens do inventario nao dao bonus.)\n");
}

void mostrar_menu(void)
{
    printf("\n========== SISTEMA AUXILIAR DE MESTRE ==========\n");
    printf(" 1 - Cadastrar personagem\n");
    printf(" 2 - Consultar personagem por ID\n");
    printf(" 3 - Alterar personagem\n");
    printf(" 4 - Remover personagem\n");
    printf(" 5 - Listar personagens\n");
    printf(" 6 - Administrar inventario\n");
    printf(" 7 - Consultar equipamentos\n");
    printf(" 8 - Equipar item\n");
    printf(" 9 - Desequipar item\n");
    printf("10 - Exibir atributos totais\n");
    printf(" 0 - Encerrar\n");
}

int main(void)
{
    /* o cadastro fica aqui no main e vai sendo passado por endereço */
    CadastroPersonagens cadastro;
    int opcao;

    cadastro_inicializar(&cadastro);

    do {
        mostrar_menu();
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
        case 1:  acao_cadastrar(&cadastro);              break;
        case 2:  acao_consultar(&cadastro);              break;
        case 3:  acao_alterar(&cadastro);                break;
        case 4:  acao_remover(&cadastro);                break;
        case 5:  acao_listar(&cadastro);                 break;
        case 6:  acao_inventario(&cadastro);             break;
        case 7:  acao_consultar_equipamentos(&cadastro); break;
        case 8:  acao_equipar(&cadastro);                break;
        case 9:  acao_desequipar(&cadastro);             break;
        case 10: acao_atributos(&cadastro);              break;
        case 0:  printf("Encerrando. Ate a proxima aventura!\n"); break;
        default: printf("Opcao invalida.\n");
        }
        /* o inventário tem menu próprio, então não precisa pausar */
        if (opcao != 0 && opcao != 6) {
            pausar();
        }
    } while (opcao != 0);

    return 0;
}

/*
    gcc -std=c11 -Wall -Wextra -pedantic main.c personagem.c inventario.c item.c -o personagens
  ./personagens no Linux     .\personagens.exe no Windows
 */
