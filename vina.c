#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "lz.h"
#include "archive.h"

/* usar os stats para conseguir mod_Date e UID, que é o id do usuário e não do arquivo kkkkkkk */

struct member *existe(struct directory *d, char *name)
{
    if (!d)
        return NULL;
    for (unsigned long i = 0; i < d->size; i++)
    {
        if (d->members[i]->name == name)
            return d->members[i];
    }
    return NULL;
}

unsigned long max_Size(struct directory *d)
{
    if (!d)
        return 0;
    unsigned long max = 0;
    for (unsigned long i = 0; i < d->size; i++)
    {
        if (d->members[i]->comp_Size > max)
            max = d->members[i]->comp_Size;
    }
    return max;
}

unsigned long new_Offset(struct directory *d)
{
    if (!d)
        return 0;
    unsigned long max = 0;
    for (unsigned long i = 0; i < d->size; i++)
    {
        if (d->members[i]->offset > max)
            max = d->members[i]->offset;
    }
    return max;
}

int main(int argc, char **argv)
{
    FILE *archive, *membro;
    char *buffer;
    struct directory *d;
    unsigned long i, max;

    if (argv[1] == "m")
        archive = fopen(argv[3], "+a");
    else
        archive = fopen(argv[2], "+a");

    d = create_Directory(archive);

    max = max_Size(d);

    if (!(buffer = malloc(max * sizeof(char))))
        return -1;

    if (argv[1] == "m")
    {
        i = 4;
        do
        {
            /* apenas um membro vai ser movido, o membro logo após -m */
            buffer = argv[i];
            unsigned long espaco, compresso, alvo;
            espaco = search_Directory(d, buffer)->offset;
            compresso = search_Directory(d, buffer)->comp_Size;
            if (argv[2] != "NULL")
            {
                alvo = search_Directory(d, argv[2])->offset;
                alvo += search_Directory(d, argv[2])->comp_Size;
            }
            else
                alvo = 0;
            move(archive, espaco, compresso, alvo, max);
            for (unsigned long j = 0; j < d->size; j++)
            {
                if (alvo > espaco)
                {
                    if ((d->members[j]->order > search_Directory(d, argv[2])->order) && (d->members[j]->offset > alvo))
                    {
                        (d->members[j]->order)--;
                        d->members[j]->offset -= search_Directory(d, argv[2])->comp_Size;
                    }
                }
                else
                {
                    if ((d->members[j]->order < search_Directory(d, argv[2])->order) && (d->members[j]->offset > alvo))
                    {
                        (d->members[j]->order)++;
                        d->members[j]->offset += search_Directory(d, argv[2])->comp_Size;
                    }
                }
            }
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
                buffer = argv[i];
                struct member *aux = search_Directory(d, buffer);
                for (unsigned long j = 0; j < d->size; j++)
                {
                    if (d->members[j]->order > aux->order)
                    {
                        (d->members[j]->order)++;
                        d->members[j]->offset -= aux->comp_Size;
                    }
                }
                extract_Member(archive, d, buffer, max);
                i++;
            } while (argv[i] != NULL);
        }
        else if (argv[1] == "r")
        {
            do
            {
                buffer = argv[i];
                struct member *aux = search_Directory(d, buffer);
                for (unsigned long j = 0; j < d->size; j++)
                {
                    if (d->members[j]->order > aux->order)
                    {
                        (d->members[j]->order)++;
                        d->members[j]->offset -= aux->comp_Size;
                    }
                }
                aux = remove_Member(d, buffer);
                fseek(archive, 0, SEEK_END);
                move(archive, aux->offset, aux->comp_Size, ftell(archive), max);
                fseek(archive, 0, SEEK_END);
                ftruncate(fileno(archive), ftell(archive) - aux->comp_Size);

                free(aux);
                i++;
            } while (argv[i] != NULL);
        }
        else if (argv[1] == "c")
        {
            for (unsigned long j = 0; j < d->size; j++)
                print_Member(d->members[j]);
        }
        else if (argv[1] == "i") /* use os stats para conseguir o UID e a data de modificação, e faça o caso em que será inserido um arquivo que já foi inserido */
        {
            do
            {
                buffer = argv[i];
                FILE *new = fopen(buffer, "w+");
                fseek(new, 0, SEEK_END);
                if (max < ftell(new))
                {
                    max = ftell(new);
                    realloc(buffer, (max * sizeof(char)));
                }
                struct member *aux = existe(d, buffer);
                if (aux == NULL)
                {
                    aux = create_Member(buffer, (new_Id(d) + 1), new_Offset(d));
                    insert_member(d, aux);
                    rewind(new);
                    fgets(buffer, max, new);
                    fseek(archive, 0, SEEK_END);
                    fputs(buffer, archive);
                }
                else
                {
                }
                i++;
                fclose(new);
            } while (argv[i] != NULL);
        }
        else if (argv[1] == "p")
        {
            do
            {
                buffer = argv[i];
                FILE *new = fopen(buffer, "w+");
                struct member *aux = create_Member(buffer, (new_Id(d) + 1), new_Offset(d));
                fseek(new, 0, SEEK_END);
                aux->og_Size = ftell(new);
                LZ_Compress(buffer, buffer, ftell(new));
                fclose(new);
                new = fopen(buffer, "w+");
                fseek(new, 0, SEEK_END);
                if (max < ftell(new))
                {
                    max = ftell(new);
                    realloc(buffer, (max * sizeof(char)));
                }
                insert_member(d, aux);
                rewind(new);
                fgets(buffer, max, new);
                fseek(archive, 0, SEEK_END);
                fputs(buffer, archive);
                i++;
                fclose(new);
            } while (argv[i] != NULL);
        }
    }

    destroy_Directory(d);

    fclose(archive);
}