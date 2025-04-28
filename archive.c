#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "archive.h"

char *search_Directory(char *dir, char *target, unsigned long base, unsigned long s)
{
    char *temp = strtok(dir, " ");

    char *aux[base - s];

    if (s > base)
    {
        while ((temp != target) && (temp != NULL))
            temp = strtok(NULL, " ");
    }
    else
    {
        aux[0] = temp;
        unsigned long i = 1;
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
        for (unsigned long i = base; i < s; i++)
            temp = strtok(NULL, " ");
    }

    return temp;
}

struct archive *create_Archive(char *name)
{
    FILE *arc = fopen(name, "r+");
    struct archive *a;
    long size;

    strtok(arc, " ");
    for (int i = 1; i < OFFSET; i++)
        strtok(NULL, " ");
    size = (atol(strtok(NULL, " ")) - 1);

    if (!(a->directory = malloc(size * sizeof(char))))
        return NULL;
    fgets(a->directory, size, arc);
    a->members = NULL;
    a->size = 0;
    return a;
}

struct member *create_Member(char *directory, char *name)
{
    struct member *m;
    char *aux;

    aux = malloc(1025 * sizeof(char));

    if (!(m = malloc(sizeof(struct member))))
        return NULL;

    strncpy(m->name, name, 1025);

    aux = strtok(directory, " ");
    while (aux != name)
        aux = strtok(NULL, " ");
    if (aux == NULL)
    {
        FILE *f = fopen(name, "r");
        time_t t;
        char ord, test;
        time(&t);
        struct tm *mod = localtime(&t);
        m->mod_Date = (long)((1000000 * mod->tm_mday) + (10000 * mod->tm_mon) + mod->tm_year);
        fseek(f, 0, SEEK_END);
        m->comp_Size = ftell(f);
        test = search_Directory(directory, "1", ORDER, ORDER);
        while (test)
        {
            ord = test;
            test = search_Directory(directory, (test + 1), ORDER, ORDER);
        }
        m->uid = m->order = (ord + 1);
        m->offset = ((atol(search_Directory(directory, ord, ORDER, OFFSET))) + (atol(search_Directory(directory, ord, ORDER, COMP))));
    }
    else
    {
        m->uid = atol(strtok(NULL, " "));
        m->og_Size = atol(strtok(NULL, " "));
        m->comp_Size = atol(strtok(NULL, " "));
        m->mod_Date = atol(strtok(NULL, " "));
        m->order = atol(strtok(NULL, " "));
        m->offset = atol(strtok(NULL, " "));
    }

    free(aux);
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

    return 1;
}

int insert_member(struct archive *a, struct member *m, unsigned long max)
{
    if ((!a) || (!m))
        return 0;

    char buffer[max];
    unsigned long i, aux;

    if (!(a->size))
        a->members = malloc(sizeof(struct member));
    else
        realloc(a->members, ((a->size + 1) * sizeof(struct member)));
    a->members[(a->size)++] = m;

    for (i = 0; i < strlen(m->name); i++)
        buffer[i] = m->name[i];

    buffer[i++] = " ";
    buffer[i++] = ltoa(m->uid);

    realloc(a->directory, (strlen(a->directory) + strlen(buffer)));

    return 1;
}

struct member *remove_Member(struct archive *a, char *name, unsigned long max)
{
    if ((!a) || !(a->size))
        return NULL;

    struct member *m, *h;
    long aux, start, end;
    FILE *dir;

    aux = atol(search_Directory(a->directory, name, NAME, ORDER));
    m = a->members[aux];
    (a->size)--;
    if (aux != a->size)
    {
        h = a->members[a->size];
        a->members[aux] = h;
        start = strcspn(a->directory, name);
        end = (strcspn(a->directory, ltoa(m->offset)) + (strspn(a->directory, ltoa(m->offset))));
        fputs(a->directory, dir);
        fseek(dir, 0, SEEK_END);
        aux = ftell(dir);
        move(dir, start, end, aux, max);
        realloc(a->directory, (aux - (end - start)));
    }

    realloc(a->members, a->size);

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

struct member *search_Archive(struct archive *a, char *name)
{
    if (!a)
        return NULL;

    struct member *m;

    for (unsigned long i = 0; i < a->size; i++)
    {
        m = a->members[i];
        if (m->name == name)
            return m;
    }
    return NULL;
}

int overwrite(FILE *archive, struct archive *a);

int destroy_Archive(struct archive *a, unsigned long max)
{
    if (!a)
        return 0;

    struct member *m;

    while (a->size > 0)
    {
        m = remove_Member(a, a->members[(a->size - 1)]->name, max);
        if (!m)
            return 0;
        free(m);
    }

    free(a->directory);
    free(a);

    return 1;
}

int extract_Member(FILE *arc, struct archive *a, char *name, unsigned long max)
{
    if (!a)
        return 0;

    FILE *aux;
    struct member *m;
    char *buffer[max];
    unsigned long end;
    m = remove_Member(a, name, max);
    fopen(m->name, "w+");
    fseek(arc, m->offset, SEEK_SET);
    fgets(buffer, m->comp_Size, arc);
    fputs(buffer, aux);
    fseek(arc, 0, SEEK_END);
    end = ftell(arc);
    move(arc, m->offset, m->comp_Size, end, max);
    truncate(arc, (end - m->comp_Size));

    return 1;
}