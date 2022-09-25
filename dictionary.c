// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"

const unsigned int N = 27;

// Represents a node in a hash table (a node in a linked list of char arrays)
typedef struct node
{
    bool isword;
    struct node *children[N];
}
node;


// Hash table (array of pointers to linked lists of strings)
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    node *current = NULL;
    int wordl = strlen(word);

    //for each char in word string
    for (int i = 0; i < wordl; i++)
    {
        //give me hash value of the char
        char sbuffer[2];
        sbuffer[0] = word[i];
        sbuffer[1] = '\0';
        const char *csbuffer = sbuffer;
        unsigned int index = hash(csbuffer);

        //if it's the first letter, find its node in table
        if (i == 0)
        {
            current = table[index];
        }
        //otherwise, move to next node in trie
        else
        {
            current = current->children[index];
        }
        //if we reach a NULL node, return false
        if (current == NULL)
        {
            return false;
        }
        //if we have reached the last letter, check if we are at the end of a word. if so, return true.
        if (i == wordl - 1)
        {
            if (current->isword)
            {
                return true;
            }
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    char letter = word[0];
    unsigned int index;
    //if it's a letter, assign corresponding bucket 0-25
    if (isalpha(letter))
    {
        index = toupper(letter) - 'A';
    }
    //if its an apostrophe, assign last bucket
    else if (letter == '\'')
    {
        index = 26;
    }
    //this should never run because i can assume everything is a letter or apostrophe
    else
    {
        index = 27;
    }
    return index;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary) //dictionary is the name of a dictonary file
{
    //fill table arr with NULL values
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    //open input dictionary file
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        printf("Could not open dictionary.\n");
        return false;
    }

    char buffer;
    bool start = true;
    node *current = NULL;

    //for each char in dictionary file
    while (fread(&buffer, sizeof(char), 1, dict) == 1)
    {
        //give me hash value of the char
        char sbuffer[2];
        sbuffer[0] = buffer;
        sbuffer[1] = '\0';
        const char *csbuffer = sbuffer;
        unsigned int index = hash(csbuffer);

        //if it is the first char of a word, start at the table array
        if (isalpha(buffer) && start == true)
        {
            //make new node if it doesn't already exist
            if (table[index] == NULL)
            {
                node *n = malloc(sizeof(node));
                if (n == NULL)
                {
                    bool unloaded = unload();
                    return false;
                }
                n->isword = false;
                for (int i = 0; i < N; i++)
                {
                    n->children[i] = NULL;
                }
                table[index] = n;
            }
            //go to next node
            current = table[index];
            start = false;
        }
        //if it is not the first char in a word, continue from current location
        else if ((isalpha(buffer) || buffer == '\'') && start == false)
        {
            //make new node if it doesn't already exist
            if (current->children[index] == NULL)
            {
                node *n = malloc(sizeof(node));
                if (n == NULL)
                {
                    bool unloaded = unload();
                    return false;
                }
                n->isword = false;
                for (int i = 0; i < N; i++)
                {
                    n->children[i] = NULL;
                }
                current->children[index] = n;
            }
            //go to next node
            current = current->children[index];
        }
        //when we have reached the end of a word
        else if (buffer == '\n')
        {
            current->isword = true;
            current = NULL;
            start = true;
        }
    }

    //close dictionary file
    fclose(dict);
    
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int sizehelp(node *n)
{
    unsigned int sizeadd = 0;
    //if this node is the end of a word, increment size
    if (n->isword)
    {
        sizeadd++;
    }
    //find the size of all children nodes and add to running total node size
    for (int i = 0; i < N; i++)
    {
        if (n->children[i] != NULL)
        {
            sizeadd += sizehelp(n->children[i]);
        }
    }
    return sizeadd;
}

unsigned int size(void)
{
    unsigned int size = 0;
    //find the size of each node in table and add it to the running total size
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            size += sizehelp(table[i]);
        }
    }
    return size;
}

// Unloads dictionary from memory, returning true if successful, else false
void unloadhelp(node *n)
{
    //recursively free all the node's children
    for (int i = 0; i < N; i++)
    {
        if (n->children[i] != NULL)
        {
            unloadhelp(n->children[i]);
        }
    }
    //free the node
    free(n);
    n = NULL;
}

bool unload(void)
{
    //free each node in root table, if it exists
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            unloadhelp(table[i]);
            table[i] = NULL;
        }
    }
    return true;
}