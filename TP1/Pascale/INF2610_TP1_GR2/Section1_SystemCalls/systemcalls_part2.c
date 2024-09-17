/*
 * Ecole Polytechnique Montreal - GIGLL
 * Automne 2024
 * SystemCalls - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
 * Pascale Lafond 2204155, 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void part21 ()
{
    // TODO
    printf("7");
    printf("7");
    printf("d");
    printf("b");
    printf("c");
    printf("b");
    printf("0");
    printf("1");
    printf("f");
    printf("5");
    printf("7");
    printf("1");
    printf("f");
    printf("1");
    printf("c");
    printf("3");
    printf("2");
    printf("g");
    printf("1");
    printf("9");
    printf("6");
    printf("c");
    printf("3");
    printf("a");
    printf("7");
    printf("d");
    printf("2");
    printf("7");
    printf("f");
    printf("6");
    printf("2");
    printf("e");
    printf(" ");
    printf("(");
    printf("p");
    printf("r");
    printf("i");
    printf("n");
    printf("t");
    printf("e");
    printf("d");
    printf(" ");
    printf("u");
    printf("s");
    printf("i");
    printf("n");
    printf("g");
    printf(" ");
    printf("p");
    printf("r");
    printf("i");
    printf("n");
    printf("t");
    printf("f");
    printf(")");
    printf("\n");

    write(STDOUT_FILENO, "77dbcb01f571f1c32g196c3a7d27f62e (printed using write)\n",56);

}

void part22 ()
{
    // TODO
    setvbuf(stdout, NULL, _IONBF, 0);
    
    printf("7");
    printf("7");
    printf("d");
    printf("b");
    printf("c");
    printf("b");
    printf("0");
    printf("1");
    printf("f");
    printf("5");
    printf("7");
    printf("1");
    printf("f");
    printf("1");
    printf("c");
    printf("3");
    printf("2");
    printf("g");
    printf("1");
    printf("9");
    printf("6");
    printf("c");
    printf("3");
    printf("a");
    printf("7");
    printf("d");
    printf("2");
    printf("7");
    printf("f");
    printf("6");
    printf("2");
    printf("e");
    printf(" ");
    printf("(");
    printf("p");
    printf("r");
    printf("i");
    printf("n");
    printf("t");
    printf("e");
    printf("d");
    printf(" ");
    printf("u");
    printf("s");
    printf("i");
    printf("n");
    printf("g");
    printf(" ");
    printf("p");
    printf("r");
    printf("i");
    printf("n");
    printf("t");
    printf("f");
    printf(")");
    printf("\n");

    write(STDOUT_FILENO, "77dbcb01f571f1c32g196c3a7d27f62e (printed using write)\n",56);
    
}

int main (int argc, char* argv[])
{
    
    if (argc!=2)
    {   printf("Le programme a un seul paramètre : 1 ou 2\n");
        return 1;
    }
    switch (atoi(argv[1])) {
         case 1:        part21();
                        break;
         case 2:        part22();
                        break;
        default:        printf(" Le paramètre du programme est invalide\n");
                        return 1;
    }
    return 0;
}
