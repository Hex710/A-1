#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lz.h"

#define NAME 0
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

char *inttoa(int i)
{
    int c, j = i;
    c = 0;

    while (j)
    {
        j /= 10;
        c++;
    }

    j = c;

    char *str[c + 1];
    if (i < 0)
    {
        str[0] = "-";
        i = -i;
    }

    while (i > 0)
    {
        str[j--] = ((i % 10) + "0");
        i /= 10;
    }

    str[j] = "\0";

    return str;
}

char *search(FILE *archive, char *target, int base, int s)
{
    FILE *temp = strtok(archive, " ");

    char *aux[base - s];

    if (s > base)
    {
        while ((temp != target) && (temp != NULL))
            temp = strtok(NULL, " ");
    }
    else
    {
        aux[0] = temp;
        int i = 1;
        while ((temp != target) && (temp != NULL))
        {
            for (i; i < base; i++)
                aux[i] = strtok(NULL, " ");
            i = 0;
            temp = strtok(NULL, " ");
        }
        return aux[s - base];
    }

    if (temp == NULL)
    {
        for (int i = base; i < s; i++)
            temp = strtok(NULL, " ");
    }

    return temp;
}

int lenght_Til(FILE *archive, char *target)
{
    char *buffer;
    int sz;
}

int move(FILE *archive, char *antecessor, int espaco, int tam, int atual)
{
    // create two files to store the info from the members
    FILE *list, *target;
    char *buffer;
    int pulo, sz, ant, num = atual;
    sz = 0;

    list = fopen("list", "w+");
    target = fopen("target", "w+");

    // one by one copy the members, that appear after the one you want to move, into a file then trunk them out
    do
    {
        ant = num;
        num = search(archive, inttoa((num + 1)), ORDER, ORDER);
    } while (num);

    do
    {
        pulo = atoi(search(archive, inttoa(ant), ORDER, OFFSET));
        fseek(archive, pulo, SEEK_SET);
        fgets(buffer, atoi(search(archive, inttoa(ant), ORDER, COMP)), archive);
        fputs(buffer, list);
        ftruncate(archive, atoi(search(archive, ant, ORDER, OFFSET)));
        sz += atoi(search(archive, ant, ORDER, COMP));

        ant--;
    } while (ant > atual);

    // copy the member you wish to move into the secondfile then cut it
    fseek(archive, espaco, SEEK_SET);
    fgets(buffer, tam, archive);
    fputs(buffer, target);
    ftruncate(archive, espaco);

    // one by one copy the members, that appear before the one you want to move, but after the one you want to move to, into a file then trunk them out
    num = search(archive, antecessor, NAME, ORDER);
    ant = (atual - 1);

    do
    {
        pulo = atoi(search(archive, inttoa(ant), ORDER, OFFSET));
        fseek(archive, pulo, SEEK_SET);
        fgets(buffer, atoi(search(archive, inttoa(ant), ORDER, COMP)), archive);
        fputs(buffer, list);
        ftruncate(archive, atoi(search(archive, ant, ORDER, OFFSET)));
        sz += atoi(search(archive, ant, ORDER, COMP));

        ant = atoi(search(archive, inttoa(ant - 1), ORDER, ORDER));
    } while (ant > num);

    // write the member you wanted to move into the archive, then write the others in the opposite order you took them
    fgets(buffer, tam, target);
    fputs(buffer, archive);

    do
    {
        pulo = (-1 * atoi(search(archive, inttoa(ant), ORDER, COMP)));
        fseek(archive, pulo, SEEK_END);
        pulo = -pulo;
        fgets(buffer, pulo, list);
        fputs(buffer, archive);
        ftruncate(list, sz);
        sz -= atoi(search(archive, ant, ORDER, COMP));

        ant = atoi(search(archive, inttoa(ant + 1), ORDER, ORDER));
    } while (ant);

    // fix the positions in the directory

    fclose(list);
    fclose(target);
}

int main(int argc, char **argv)
{
    FILE *archive, *membro;
    char *buffer;
    int i;

    buffer = argv[2];

    if (argv[1] == "m")
        buffer = argv[3];

    archive = fopen(buffer, "+a");

    if (argv[1] == "m")
    {
        i = 4;
        do
        {
            buffer = argv[i];
            int espaco, compresso, pos;
            espaco = atoi(search(archive, buffer, NAME, OFFSET));
            compresso = atoi(search(archive, buffer, NAME, COMP));
            pos = atoi(search(archive, buffer, NAME, ORDER));
            move(archive, argv[2], espaco, compresso, pos);
            i++;
        } while (argv[i] != NULL);
    }
    else
    {
        buffer = argv[3];
        i = 3;
        if (argv[1] == "x")
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (argv[1] == "r")
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (argv[1] == "c")
        {
        }
        else if (argv[1] == "i")
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (argv[1] == "p")
        {
            do
            {

            } while (argv[i] != NULL);
        }
    }

    fclose(archive);
}