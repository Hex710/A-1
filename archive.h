#ifndef archive_h
#define archive_h

#include <stdio.h>

#define NAME 0
#define UID 1
#define OG 2
#define COMP 3
#define DATE 4
#define ORDER 5
#define OFFSET 6

// contem todos os membros do arquivo e o tamanho do mesmo
struct directory
{
    struct member **members; // vetor com as informacoes dos membros como structs
    unsigned long size;      // tamanho do vetor de membros
};

// contem os dados de um membro do archive
struct member
{
    char name[1025];         // nome do membro
    unsigned long uid;       // id unica do membro
    unsigned long og_Size;   // tamanho do membro original
    unsigned long comp_Size; // tamanho do membro apos compressao
    unsigned long mod_Date;  // data de modificacao do membro
    unsigned long order;     // ordem do membro dentro do archive
    unsigned long offset;    // offset do inicio do archive ate o inicio do membro
};

// cria o struct arquivo baseado no arquivo ja existente
struct directory *create_Directory(FILE *archive);

// cria um struct membro baseado nas informacoes de um arquivo pre-existente
struct member *create_Member(char *name, unsigned long uid, unsigned long off);

// faz movimentacoes de dados dentro de um arquivo, max representa o tamanho maximo do buffer
int move(FILE *archive, unsigned long start, unsigned long tam, unsigned long target, unsigned long max);

// insere um struct membro no struct directory
int insert_member(struct directory *d, struct member *m);

// remove um struct membro de um struct directory
struct member *remove_Member(struct directory *d, char *name);

// imprime as informacoes de um membro
int print_Member(struct member *m);

// retorna ponteiro para o struct membro procurado, ou NULL se o mesmo nao existir
struct member *search_Directory(struct directory *d, char *name);

// sobrescreve um arquivo com a struct directory
int overwrite(FILE *archive, struct directory *d, unsigned long max);

// libera o espaco de um struct directory
int destroy_Directory(struct directory *d);

// extrai o membro indicado do arquive e o transforma em um arquivo
int extract_Member(FILE *arc, struct directory *d, char *name, unsigned long max);

#endif