#include <stdio.h>
#include <stdlib.h>

typedef struct Tenreg {
 char nom[20];
 char prenom[20];
 char cle[5];
 bool eff;
 } Tenreg;

 typedef struct Tbloc {
 Tenreg eng[5];
 int nbeng; //nb de cases remplies
 } Tbloc;

 typedef struct Tentete {
 int nbblocs;
 int dernierbloc;
 int nblibres;

 }Tentete;

 Tbloc buffer;

typedef struct fichier {
    FILE* f;
    Tentete entete;

}fichier;



fichier* ouvrir(char nomfich[20],char mode)
{
    fichier *t= malloc(sizeof(fichier));
    if(mode=='A')
    {
        t->f=fopen(nomfich,"rb+"); //ouverture mode lecture +
        if(t->f==NULL)
            printf("error");
        else
            fread(&(t->entete),sizeof(Tentete),1,t->f); //recuperation de l'entete en MC
    }
    else
    {
        t->f=fopen(nomfich,"wb+"); //ouverture en mode ecriture +
        if(t->f==NULL)
            printf("error");
        else
        {
             //initialiser l'entete
            (t->entete).dernierbloc=0;
            (t->entete).nblibres=0;
            (t->entete).nbblocs=0;

            fwrite(&(t->entete),sizeof(Tentete),1,t->f);  //ecrire l'entete dans le fichier
        }

    }
   return t;
}


int main()
{
   fichier* sfsd=ouvrir("sfsd",'b');

















fclose(sfsd);





return 0;
}





