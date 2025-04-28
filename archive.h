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

struct archive
{
    char *directory;         // diretorio com as informacoes dos membros em formato de string
    struct member **members; // vetor com as informacoes dos membros como structs
    unsigned long size;      // tamanho do vetor de membros
};

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
struct archive *create_Archive(FILE *archive);

// cria um struct membro baseado nas informacoes de um arquivo pre-existente
struct member *create_Member(char *directory, char *name);

// faz movimentacoes de dados dentro de um arquivo, max representa o tamanho maximo do buffer
int move(FILE *archive, unsigned long start, unsigned long tam, unsigned long target, unsigned long max);

// insere um struct membro no struct archive
int insert_member(struct archive *a, struct member *m);

// remove um struct membro de um struct archive
struct member *remove_Member(struct archive *a, char *name, unsigned long max);

// imprime as informacoes de um membro
int print_Member(struct member *m);

// retorna ponteiro para o struct membro procurado, ou NULL se o mesmo nao existir
struct member *search_Archive(struct archive *a, char *name);

// sobreescreve um arquivo com a struct archive
int overwrite(FILE *archive, struct archive *a);

// libera o espaco de um struct archive
int destroy_Archive(struct archive *a);

// extrai o membro indicado do arquive e o transforma em um arquivo
int extract_Member(struct archive *a, char *name);

#endif