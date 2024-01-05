#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Tenreg {

  char nom[20];
  char prenom[20];
  char cle[6];

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

typedef struct fichier {
  FILE* f;
  Tentete entete;

}fichier;

//*******************************************************************************************************
//function that plays the role of fopen
fichier* ouvrir(char nomfich[20],char mode)
{
    fichier *t= malloc(sizeof(fichier));
    switch (mode)
    {
    case 'A':
      t->f=fopen(nomfich,"rb+"); //ouverture mode lecture +
        if(t->f==NULL)
            printf("error");
        else
            fread(&(t->entete),sizeof(Tentete),1,t->f); //recuperation de l'entete en MC
      break;
    case 'B':
      t->f=fopen(nomfich,"wb+"); //ouverture en mode ecriture +
        if(t->f==NULL){
            printf("error");
        }else
        {
          //initialiser l'entete
          (t->entete).dernierbloc=0;
          (t->entete).nblibres=0;
          (t->entete).nbblocs=0;
          fwrite(&(t->entete),sizeof(Tentete),1,t->f);  //ecrire l'entete dans le fichier
        }
        break;

    default: printf("Invalid mode '%c' provided. Please use 'A' or 'B'.\n", mode);
      break;
    }
    return t;
}
//************************************************************************************************************************************
//fonction pour afficher un bloc
void afficher (Tbloc buffer,int numbloc)
{
    printf("Nombre d'enregistrements dans le bloc: %d\n", numbloc);
    for (int i=0;i<buffer.nbeng;i++)
    {
        printf("Enregistrement %d:\n", i + 1);
    printf("Nom: %s\t", buffer.eng[i].nom);
    printf("Prenom: %s\t", buffer.eng[i].prenom);
    printf("Cle: %s\t", buffer.eng[i].cle);
    printf("\n");
    }
 }

//************************************************************************************************************************************
//fonction pour écrire un bloc dand le fichier
void EcrireBloc(fichier *file,int i,Tbloc *buffer)
 {
      fseek(file->f,sizeof(Tentete)+i*sizeof(Tbloc),SEEK_SET);
        fwrite(buffer,sizeof(Tbloc),1,file);
 }

 //***********************************************************************************************************************************
// Function to read a block of student records from the file into a buffer
void LireBloc(fichier *file, int i, Tbloc *buf) {

    fseek(file->f, sizeof(Tentete) + i * sizeof(Tbloc), SEEK_SET);  // Move the file pointer to the appropriate position for the specified block
    // i is an index that specify which block to read from the file.
    fread(buf, sizeof(Tbloc), 1, file->f);    // Read the block of records into the provided buffer
}
//*************************************************************************************************************************************
//fonction qui recherche une clé
int Recherche(char key[], fichier *file){

  bool trouve=false;
  int i=0;
  int numbloc;
  fseek(file->f, sizeof(Tentete), SEEK_SET);  //Move the file pointer past the header

  if (file->entete.nbblocs == 0) {
      printf("Erreur : le fichier est vide.\n");
      return;
    }
while(i<file->entete.nbblocs && !trouve){
    Tbloc buffer;
    LireBloc(file,i,&buffer);    //buffer contains the bloc with index i
    int j=0;
    while(j<buffer.nbeng && !trouve){
      if(strcmp(buffer.eng[j].cle, key) == 0){
        trouve=true;
        numbloc=i;
      }else {j++;}
    }
    i++;
}
if(trouve==true){
   printf("L'étudiant avec la cle %s existe.\n", key);
   return numbloc;
  }else{
    printf("L'étudiant avec la cle %s n'existe pas.\n", key);
    return -1;
  }

}

//**************************************************************************************************************************************
//Fonction d'insertion :
void Inserer(fichier *file, char cle[], char nom[], char prenom[]) {
    Tbloc buffer;
    int i=0;
    int blocTrouve = Recherche(cle,file);
    if(blocTrouve == -1){  //l'étudiant n'existe pas//
     //on va faire la recherche d'un bloc qui contient des vides avant le dernier bloc//
     while(i<file->entete.dernierbloc){
        LireBloc(file,i,&buffer);
        if(buffer.nbeng < 5){
            //en insère dans la place trouver
         strcpy(buffer.eng[buffer.nbeng].cle, cle);
         strcpy(buffer.eng[buffer.nbeng].nom, nom);
         strcpy(buffer.eng[buffer.nbeng].prenom, prenom);
          buffer.nbeng++;
          EcrireBloc(file, i, &buffer);
         // Mettre à jour l'entête
         file->entete.nblibres++;
         fseek(file->f, 0, SEEK_SET);
         fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
         return; // Sortir de la fonction après l'insertion
        }
        i++;
     }
     //Maintenant si aucun bloc à des vide  n'est trouvé,on insère dans le dernier bloc
        LireBloc(file, file->entete.dernierbloc, &buffer);

        if (buffer.nbeng == 5) {
            // Si le dernier bloc est plein, ajouter un nouveau bloc
            file->entete.nbblocs++;
            file->entete.dernierbloc++;
            fseek(file->f, 0, SEEK_SET);
            fwrite(&(file->entete), sizeof(Tentete), 1, file->f);

            // Lire le nouveau dernier bloc
            LireBloc(file, file->entete.dernierbloc, &buffer);
        }

        // Ajouter l'étudiant dans le dernier bloc
        strcpy(buffer.eng[buffer.nbeng].cle, cle);
        strcpy(buffer.eng[buffer.nbeng].nom, nom);
        strcpy(buffer.eng[buffer.nbeng].prenom, prenom);
        buffer.nbeng++;

        EcrireBloc(file, file->entete.dernierbloc, &buffer);

        // Mettre à jour l'entête
        file->entete.nblibres++;
        fseek(file->f, 0, SEEK_SET);
        fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
    } else {
        // L'étudiant existe déjà donc on affiche
        printf("L'étudiant avec la cle %s existe déjà dans le bloc %d.\n", cle, blocTrouve);
    }
}
//*******************************************************************************************************************************
//fonction suppression
void suppression( fichier *file,char cle[])
{ //suppression est physique au lieu de logique pour gagner de l'éspace et se bénifier du fait que le tableau n'est pas ordonné
    Tbloc buffer;
 if (file->entete.nbblocs==0)
 {
     printf("\nERREUR:le fichier est vide\n");
 }
 else {
        int numbloc=Recherche(cle,file); //recupérer l'indice du bloc contentant cette clé

        if (numbloc<0){ printf("\nil n'existe pas d'etudiant avec cette cle\n");} //verification si la cle existe
        else {
        LireBloc(file,numbloc,&buffer);
        int numeng=-1;
        int i=0;
        //chercher la case du bloc où la clé se trouve
        while (i<buffer.nbeng && numeng==-1)
        {
          if (buffer.eng[i].cle==cle)
          {
              numeng=i;
          } else {i++;}

        }

    buffer.eng[numeng]=buffer.eng[buffer.nbeng-1];
    /*supprimer la case voulu en l'écrasant avec le dernier enregistrement
    du tableau ,donc au leu de faire des décalages coûteux on va juste déplacer le dernier enregistrement
    et le supprimer après en juste diminuant la taille du tableau*/
    buffer.nbeng--;//on diminue la taille du tableau

    if(buffer.nbeng==0) //si le bloc est devenu vide on le supprime en mettant a sa place le dernier bloc
    {
     LireBloc(file,file->entete.dernierbloc,&buffer);
     EcrireBloc(file,numbloc,&buffer);
     file->entete.dernierbloc--; //effectuer les changements nécéssaire
     file->entete.nbblocs--;
     fseek(file->f,0,SEEK_SET);
     fwrite(&(file->entete),sizeof(Tentete),1,file->f);//écrire l'entête dans le fichier
    }
   else{ EcrireBloc(file,numbloc,&buffer); } //écrire le bloc dans son emplacement après avoir éffectué les modifications

            }
        }

}

int main() {

    fichier* sfsd=ouvrir("sfsd",'B');

    // Check if the file is successfully opened
    if (sfsd->f == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    // Writing a block with two student records 
    Tbloc buffer;
    buffer.nbeng = 2;

    strcpy(buffer.eng[0].cle, "12345");
    strcpy(buffer.eng[0].nom, "Doe");
    strcpy(buffer.eng[0].prenom, "John");

    strcpy(buffer.eng[1].cle, "67890");
    strcpy(buffer.eng[1].nom, "Smith");
    strcpy(buffer.eng[1].prenom, "Alice");

    EcrireBloc(sfsd->f,0,&buffer);

    //add another block with one student record 
  buffer.nbeng = 1;
    strcpy(buffer.eng[0].cle, "54321");
    strcpy(buffer.eng[0].nom, "Doe");
    strcpy(buffer.eng[0].prenom, "Jane");

    EcrireBloc(sfsd->f,1,&buffer);

    sfsd->entete.nbblocs=2;
    sfsd->entete.dernierbloc=1;
    fseek(sfsd->f,0,SEEK_SET);
    fwrite(&(sfsd->entete),sizeof(Tentete),1,sfsd->f); //ecrire la nouvelle entete dans le fichier

    printf("le numero de bloc est: %d\n", Recherche("12345", sfsd));
    printf("le numero de bloc est: %d\n", Recherche("67890", sfsd));
    printf("le numero de bloc est: %d\n", Recherche("54321", sfsd));

    Inserer(sfsd,"25486","Salmi","Mohamed");
    printf("le numero de bloc est: %d", Recherche("25486", sfsd));
    fclose(sfsd->f);

    return 0;
}

