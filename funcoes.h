//////////////////////////////////////////////////// Lê o tamanho do registro

int leia_reg(char buffer[],int tam, FILE *ui)
{
    short int tam_reg;
    if(fread(&tam_reg,sizeof(short int),1,ui) != 1)
    return 0;

    if(tam_reg<tam)
    {
        fread(buffer,sizeof(char),tam_reg,ui);

        buffer[tam_reg]='\0';
        return tam_reg;
    }else return 0;

}

///////////////////////////Faz a leitura de n caracteres até '\n'

int leia_linha(char campo[], int tam, FILE *ui)
{
    int i=0;
    char ch;
    ch=fgetc(ui);

    while(ch!=EOF && ch!='\n')
    {
        if(i<tam-1)
        {
            campo[i]=ch;
            i++;
        }
        ch=fgetc(ui);
    }
    campo[i]='\n';
    campo[i+1]='\0';

    return i;
}