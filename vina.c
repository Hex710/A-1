#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz.h"

#define UID 1
#define OG 2
#define COMP 3
#define DATE 4
#define ORDER 5
#define OFFSET 6

struct info
{
    char name[1025]; // nome do membro
    int uid;         // id unica do membro
    int og_Size;     // tamanho do membro original
    int comp_Size;   // tamanho do membro apos compressao
    int mod_Date;    // data de modificacao do membro
    int order;       // ordem do membro dentro do archive
    int offset;      // offset do inicio do archive ate o inicio do membro
};

char *search(FILE *archive, char *name, int s)
{
    FILE *temp = strtok(archive, " ");

    while (temp != name)
        temp = strtok(NULL, " ");

    for (int i = 0; i < s; i++)
        temp = strtok(NULL, " ");

    return temp;
}

int main()
{
    FILE *archive;
    char *buffer, *op, *token, *temp;
    fscanf(stdin, buffer);

    op = strtok(buffer, " ");

    temp = strtok(NULL, " ");

    if (op == "m")
    {
        token = temp;
        temp = strtok(NULL, " ");
    }

    archive = fopen(temp, "+a");

    if (op == "m")
    {
        temp = strtok(NULL, " ");
        search(archive, temp, OFFSET);
    }
    else if (op == "x")
    {
    }
    else if (op == "r")
    {
    }
    else if (op == "c")
    {
    }
    else
    {

        if (op == "ip")
        {
        }
        else if (op == "ic")
        {
        }
    }

    fclose(archive);
}