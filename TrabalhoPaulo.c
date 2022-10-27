#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcoes.h"

void importa_arquivo(char* argv)
{
    FILE *arq,*arq2;
    arq = fopen(argv,"rb");
    arq2 = fopen("dados.dat", "wb");

    char buffer[1000];
    short int comp_linha;
    int offset=-1;

    fwrite(&offset,sizeof(int),1,arq2);
    comp_linha = leia_linha(buffer,1000,arq);
    while(comp_linha>0)
    {
        fwrite(&comp_linha,sizeof(short int),1,arq2);
        fwrite(buffer,sizeof(char),comp_linha,arq2);
        comp_linha = leia_linha(buffer,1000,arq);
    }

    if(comp_linha==0)
    printf("\nImportação concluida com sucesso.\n");

    fclose(arq);
    fclose(arq2);
}

int busca_registro(char identificador[], int tami, char buffer[], int tamb,FILE *arq)
{

    char key[10],ch;
    int i=0,k=0;
    short tam_reg=0;
    int position=0;

    fseek(arq,4,SEEK_SET);
    position=ftell(arq);
    tam_reg=leia_reg(buffer,1000,arq);
    while(tam_reg>0)
    {
        while(buffer[i]!='|' && buffer[i]!='*')
        {
            key[i] = buffer[i];
            i++;
        }
        ch=buffer[i];
        key[i]='\0';

        if(strcmp(identificador,key)==0)
        {
            if(ch=='*')
            {
                printf("\nO registro %s ja foi removido.\n",identificador);
                return -1;
            }
            else
            {
                return position;
            }
        }else
        {
            i=0;
            key[i]='\0';
            buffer[i]='\0';
            position=ftell(arq);
            tam_reg=leia_reg(buffer,1000,arq);
        }
    }
    fseek(arq,0,SEEK_SET);
    return -1;
}

void LED(int byteoffset, FILE *arq)
{   
    char ch = '*', ch2 ='h';
    int vazio = -1;
    int num=0;
    fseek(arq, 0, SEEK_SET);
    fread(&num,sizeof(int),1,arq);
    if (num == -1)
    {
        fseek(arq, 0, SEEK_SET);
        fwrite(&byteoffset,sizeof(int),1,arq);
        
        fseek(arq,(byteoffset+2), SEEK_SET);
        
        while(ch2 != '|')
        {
            ch2 = fgetc(arq);
        }
        fseek(arq,-1,SEEK_CUR);
        fputc(ch,arq);
        fwrite(&vazio,sizeof(int),1,arq);

    }else
    {
        fseek(arq, 0, SEEK_SET);
        fwrite(&byteoffset,sizeof(int),1,arq);

        fseek(arq,(byteoffset+2), SEEK_SET);
        
        while(ch2 != '|')
        {
            ch2 = fgetc(arq);
        }
        fseek(arq,-1,SEEK_CUR);

        fputc(ch,arq);
        fwrite(&num,sizeof(int),1,arq);
    }

}

void remove_registro(char identificador[],int tam,FILE *arq)
{
    int position=0;
    char buffer[1000];

    fseek(arq,0,SEEK_SET);
    position=busca_registro(identificador,tam,buffer,1000,arq);

    if(position != -1)
    {
        printf("\nRegistro %s removido!(%ld bytes)\nLocal: %d bytes (0x%x)\n",identificador,strlen(buffer),position,position);
        LED(position,arq);
    }
    else
    {
        printf("\nNão foi possivel remover o registro %s.\n",identificador);
    }
    
}

int insere_registro(char identificador[], short int tami,FILE *arq)
{
   
    char buffer[1000];
    int num; //numero do byteoffset que cabe o registro
    int byteoffset_num; //byteoffset de onde ''num'' ta armazenado
    int num_prox; //proximo espaço da led disponivel
    int verificador=0;
    short int tamanho;
    int i = 0;
    char key[10], pipe = '|';

    while(identificador[i]!='|' && identificador[i]!='*')
    {
        key[i] = identificador[i];
        i++;
    }
    
    key[i]='\0';
    
    verificador=busca_registro(key,10,buffer,1000,arq);

    if(verificador==-1)
    {
        fseek(arq,0,SEEK_SET);
        byteoffset_num = 0;
        fread(&num,sizeof(int),1,arq);
        while (num!= -1)
        {
            char ch = 'h';
        
            fseek(arq,num,SEEK_SET);
            fread(&tamanho,sizeof(short int),1,arq);

            while (ch != '*') //LE PROXIMO
            {
                ch = fgetc(arq);
            }

            fread(&num_prox,sizeof(int),1,arq);

            if(tamanho >= tami)
            {
                fseek(arq, num+sizeof(short int),SEEK_SET);
                fwrite(identificador,sizeof(char),tami,arq);

                printf("\nO registro com chave %s foi inserido.\nLocal: %d (0x%x).\n",key,num,num);

                fseek(arq,byteoffset_num,SEEK_SET);
                fwrite(&num_prox,sizeof(int),1,arq);

                return 1;

            }else
            {
                num = num_prox;
                fseek(arq,num+sizeof(short),SEEK_SET);

                while (ch != '*') //LE PROXIMO
                {
                    ch = fgetc(arq);
                }

                fread(&byteoffset_num,sizeof(int),1,arq);
            }
        }

        fseek(arq,0,SEEK_END);
        int byteoffset_fim = ftell(arq);
        fwrite(&tami,sizeof(short int),1,arq);
        fwrite(identificador,sizeof(char),(tami),arq);
        printf("\nO registro com chave %s foi inserido.\nLocal: %d (0x%x).\n",key,byteoffset_fim,byteoffset_fim);
       
        return 1;

    }else
    {
        printf("\nNão foi possivel inserir o registro. Motivo: chave %s ja existe.\n",key);
        return 0;
    }

}


void imprimir_led()
{
    FILE *arq;
    arq = fopen("dados.dat","rb");
    if(arq == NULL)
    {
        printf("\nO arquivo dados.dat nao existe ou foi corrompido.\n");
    }

    int num;
    char ch;

    fseek(arq,0,SEEK_SET);
    fread(&num,sizeof(int),1,arq);
    printf("Lista de Espacoes Disponiveis:\n");
    while (num!= -1)
    {
        printf("%d <- ",num);
        fseek(arq,num,SEEK_SET);
        ch=fgetc(arq);
        while(ch!='*')
        {
            ch = fgetc(arq);
        }
        fread(&num,sizeof(int),1,arq);
    }
    printf("-1\n");

    fclose(arq);
}

void executar_operacoes(char* argv)
{
    FILE *arq,*arq2;
    arq = fopen(argv,"r");
    arq2 = fopen("dados.dat","rb+");
    char chave[1000],buffer[1000];
    char ch;
    short i=0;
    int verificador=0;

    ch=fgetc(arq);
    while(ch!=EOF)
    {
        if(ch == 'b')
        {
            ch=fgetc(arq);
            ch=fgetc(arq);
            while(ch != '\n')
            {
                chave[i]=ch;
                i++;
                ch=fgetc(arq);
            }

            chave[i]='\0';
            verificador=busca_registro(chave,1000,buffer,1000,arq2);
            if(verificador != -1)
                printf("\nRegistro %s encontrado!\n%s (%ld bytes)\nLocal: %d bytes (0x%x)\n",chave,buffer,strlen(buffer),verificador,verificador);
            else
                printf("\nNão foi possivel encontrar o registro %s.\n",chave);
        }else if(ch == 'i')
        {
            ch=fgetc(arq);
            ch=fgetc(arq);
            while(ch != '\n')
            {
                chave[i]=ch;
                i++;
                ch=fgetc(arq);
            }

            chave[i]='\n';
            chave[i+1]='\0';

            insere_registro(chave,i,arq2);
        }else if(ch == 'r')
        {
            ch=fgetc(arq);
            ch=fgetc(arq);
            while(ch != '\n')
            {
                chave[i]=ch;
                i++;
                ch=fgetc(arq);
            }
            chave[i]='\0';
            remove_registro(chave,1000,arq2);
        }

        i=0;
        chave[i]='\0';
        ch=fgetc(arq);
    }
    
    fclose(arq);
    fclose(arq2);
}

int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-i") == 0) {

        printf("Modo de importacao de ativado\nO arquivo %s foi importado para binario.\n", argv[2]);
        importa_arquivo(argv[2]);

    } else if (argc == 3 && strcmp(argv[1], "-e") == 0) {

        printf("Modo de execucao de operacoes ativado. Arquivo de operacoes: %s\n", argv[2]);
        executar_operacoes(argv[2]);

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Modo de impressao da LED ativado\n");
        imprimir_led();

    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);

    }
    return 0;
}