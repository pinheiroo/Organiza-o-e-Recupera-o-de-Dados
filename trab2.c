#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max=5

int hash(char key[],int maxaddr)
{
    short SUM=0;
    int j=0;
    while(j<12)
    {
        SUM = (SUM + 100*key[j]+key[j+1])%19937;
        j = j+2;
        return SUM%maxaddr;
    }
}
int make_adress(char key[],int profund)
{
    int retval=0;
    int mask=1;
}
void importa_chave()
{
    
}

int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-i") == 0) {

        printf("Modo de importacao de chaves ativado\nO arquivo %s de chaves foi importado.\n", argv[2]);
        importa_chave(argv[2]);

    } else if (argc == 2 && strcmp(argv[1], "-pd") == 0) {

        printf("Modo de impressão das informações do diretório.");
        imprimir_diretorio();

    } else if (argc == 2 && strcmp(argv[1], "-pb") == 0) {

        printf("Modo de impressão das informações dos buckets.\n");
        imprimir_buckets();

    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -pd|-pb\n", argv[0]);
        exit(EXIT_FAILURE);

    }
    return 0;
}