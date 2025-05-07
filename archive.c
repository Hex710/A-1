#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "archive.h"

// criar o caso base para não ter um arquivo/diretório ainda
struct directory *create_Directory(FILE *arc)
{
    struct directory *d;
    long i = 0;

    d->members = NULL;

    d = malloc(sizeof(struct directory));

    fread(d->size, sizeof(long), 1, arc);

    d->members = malloc(sizeof(struct member));

    for (i; i < d->size; i++)
    {
        realloc(d->members, sizeof(struct member) * (i + 1));
        d->members[i] = malloc(sizeof(struct member));
        fread(d->members[i], sizeof(struct member), 1, arc);
    }

    return d;
}

struct member *create_Member(char *name, unsigned long uid, unsigned long off)
{
    struct member *m;
    strncpy(m->name, name, 1025);
    FILE *f = fopen(name, "r");
    time_t t;
    time(&t);
    struct tm *mod = localtime(&t);
    m->mod_Date = (long)((1000000 * mod->tm_mday) + (10000 * mod->tm_mon) + mod->tm_year);
    fseek(f, 0, SEEK_END);
    m->comp_Size = ftell(f);
    m->uid = m->order = uid;
    m->offset = off;
    fclose(f);

    return m;
}

int move(FILE *archive, unsigned long start, unsigned long tam, unsigned long target, unsigned long max)
{
    if (((start + tam) == target) || (start == target))
        return 0;

    FILE *aux;
    char buffer[max];
    unsigned long i, j = target;

    aux = fopen("target", "w+");

    fseek(archive, start, SEEK_SET);
    fgets(buffer, tam, archive);
    fputs(buffer, aux);

    if (target > start)
    {
        i = (start + tam + 1);
        while ((i + max) < target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, max, archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
            start += max;
            j += max;
            i += max;
        }
        if (i < target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, (target - i), archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
        }
        fgets(buffer, tam, aux);
        fputs(buffer, archive);
    }
    else
    {
        i = (start - 1);
        while ((i - max) > target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, max, archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
            start += max;
            j += max;
            i -= max;
        }
        if (i > target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, (i - target), archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
        }
        fgets(buffer, tam, aux);
        fseek(archive, target, SEEK_SET);
        fputs(buffer, archive);
    }

    fclose(aux);
    remove(target);

    return 1;
}

int insert_member(struct directory *d, struct member *m)
{
    if ((!d) || (!m))
        return 0;

    unsigned long i, aux;

    if (!(d->size))
        d->members = malloc(sizeof(struct member));
    else
        realloc(d->members, ((d->size + 1) * sizeof(struct member)));
    d->members[(d->size)++] = m;

    return 1;
}

struct member *remove_Member(struct directory *d, char *name)
{
    if ((!d) || !(d->size))
        return NULL;

    struct member *m, *h;
    long aux, start, end;

    while (d->members[aux]->name != name)
        aux++;
    m = d->members[aux];
    (d->size)--;
    if (aux != d->size)
    {
        h = d->members[d->size];
        d->members[aux] = h;
    }

    realloc(d->members, d->size);

    return m;
}

int print_Member(struct member *m)
{
    if (!m)
        return 0;

    unsigned long day, month, year;

    year = (m->mod_Date % 10000);
    month = ((m->mod_Date / 10000) % 100);
    day = ((m->mod_Date / 1000000) % 100);

    printf("nome: %s\n", m->name);
    printf("UID: %lu\n", m->uid);
    printf("tamanho original: %lu\n", m->og_Size);
    printf("tamnho em disco: %lu\n", m->comp_Size);
    printf("data de modificação: %lu/%lu/%lu\n", day, month, year);
    printf("%lu\n", m->order);

    return 1;
}

struct member *search_Directory(struct directory *d, char *name)
{
    if (!d)
        return NULL;

    struct member *m;

    for (unsigned long i = 0; i < d->size; i++)
    {
        m = d->members[i];
        if (m->name == name)
            return m;
    }
    return NULL;
}

unsigned long find_Size(struct directory *d)
{
    if (!d)
        return 0;

    unsigned long i = 0;

    while (d->members[i]->order != 1)
        i++;

    return d->members[i]->offset;
}

int overwrite(FILE *archive, struct directory *d, unsigned long max)
{
    if ((!archive) || (!d))
        return 0;

    long add;
    unsigned long tam;

    tam = find_Size(d);

    if (tam != (sizeof(struct member) * d->size))
    {
        move(archive, tam, fseek(archive, 0, SEEK_END), (d->size * sizeof(struct member)), max);
        add = (d->size * sizeof(struct member) - tam);
        for (unsigned long i = 0; i < d->size; i++)
            d->members[i]->offset += add;
    }
    rewind(archive);
    fwrite(d->size, sizeof(long), 1, archive);
    for (unsigned long i = 0; i < d->size; i++)
    {
        fwrite(d->members[i], sizeof(struct member), 1, archive);
    }
    return 1;
}

int destroy_Directory(struct directory *d)
{
    if (!d)
        return 0;

    struct member *m;

    while (d->size > 0)
    {
        m = remove_Member(d, d->members[--(d->size)]->name);
        if (!m)
            return 0;
        free(m);
    }

    free(d->members);
    free(d);

    return 1;
}

int extract_Member(FILE *arc, struct directory *d, char *name, unsigned long max)
{
    if (!d)
        return 0;

    FILE *aux;
    struct member *m;
    char *buffer[max];
    unsigned long end;

    m = remove_Member(d, name);
    aux = fopen(m->name, "w+");
    fseek(arc, m->offset, SEEK_SET);
    fgets(buffer, m->comp_Size, arc);
    fputs(buffer, aux);
    fseek(arc, 0, SEEK_END);
    end = ftell(arc);
    move(arc, m->offset, m->comp_Size, end, max);
    truncate(arc, (end - m->comp_Size));

    fclose(aux);

    return 1;
}