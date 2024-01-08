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

    default: printf("Mode invalide '%c' fourni. Veuillez utiliser 'A' ou 'B'.\n", mode);
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
//fonction pour écrire un bloc dans le fichier
void EcrireBloc(fichier *file,int i,Tbloc *buffer)
 {
      fseek(file->f,sizeof(Tentete)+i*sizeof(Tbloc),SEEK_SET);
        fwrite(buffer,sizeof(Tbloc),1,file);
 }

 //***********************************************************************************************************************************
// Fonction pour lire un bloc d'enregistrements  depuis le fichier dans un buffer.
void LireBloc(fichier *file, int i, Tbloc *buf) {

    fseek(file->f, sizeof(Tentete) + i * sizeof(Tbloc), SEEK_SET);  // Move the file pointer to the appropriate position for the specified block
     // i est un indice qui spécifie quel bloc lire depuis le fichier.
    fread(buf, sizeof(Tbloc), 1, file->f);     // Lire le bloc d'enregistrements dans le buffer fourni.
}
//*************************************************************************************************************************************
//fonction qui recherche une clé
int Recherche(char key[], fichier *file){

  bool trouve=false;
  int i=0;
  int numbloc;
  fseek(file->f, sizeof(Tentete), SEEK_SET); //Déplacez le pointeur de fichier au-delà de l'entete.

  if (file->entete.nbblocs == 0) {
      return -1;
    }
while(i<file->entete.nbblocs && !trouve){
    Tbloc buffer;
    LireBloc(file,i,&buffer);    //Le buffer contient le bloc avec l'index i.
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
   return numbloc;
  }else{
    return -1;
  }

}

//**************************************************************************************************************************************
//Fonction d'insertion :
void Inserer(fichier *file, char cle[], char nom[], char prenom[]) {
    Tbloc buffer;
    int blocTrouve = Recherche(cle,file);
    if(blocTrouve == -1){  //l'étudiant n'existe pas//
      if (file->entete.nbblocs == 0) {
        // Si le fichier est vide, ajouter un nouveau bloc
        file->entete.nbblocs = 1;
        file->entete.dernierbloc = 0;
        fseek(file->f, sizeof(Tentete), SEEK_SET);
       fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
       // Ajouter l'étudiant dans le premier bloc
        LireBloc(file, file->entete.dernierbloc, &buffer);
        strcpy(buffer.eng[buffer.nbeng].cle, cle);
        strcpy(buffer.eng[buffer.nbeng].nom, nom);
        strcpy(buffer.eng[buffer.nbeng].prenom, prenom);
        buffer.nbeng++;

        fseek(file->f, sizeof(Tentete) + file->entete.dernierbloc * sizeof(Tbloc), SEEK_SET);
        fwrite(&buffer, sizeof(Tbloc), 1, file->f);

    } else {
        // Lire le dernier bloc pour vérifier s'il y a de la place pour un nouvel étudiant
        LireBloc(file, file->entete.dernierbloc, &buffer);
        if (buffer.nbeng == 5) {
            // Si le dernier bloc est plein, ajouter un nouveau bloc
            file->entete.nbblocs++;
            file->entete.dernierbloc++;
            fseek(file->f, 0, SEEK_SET);
            fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
        }
    }

    // Ajouter l'étudiant dans le dernier bloc
    LireBloc(file, file->entete.dernierbloc, &buffer);
    strcpy(buffer.eng[buffer.nbeng].cle, cle);
    strcpy(buffer.eng[buffer.nbeng].nom, nom);
    strcpy(buffer.eng[buffer.nbeng].prenom, prenom);
    buffer.nbeng++;

    fseek(file->f, sizeof(Tentete) + file->entete.dernierbloc * sizeof(Tbloc), SEEK_SET);
    fwrite(&buffer, sizeof(Tbloc), 1, file->f);
    }else {
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
    char cle[6];
    char nom[20];
    char prenom[20];
    Tbloc b,buffer;
    int blocIndex ;
    int choice;

     // Vérifiez si le fichier a été ouvert avec succès.
    if (sfsd->f == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        return 1;
    }
    
     // Écrire un bloc avec 3 enregistrement Etudiant
      buffer.nbeng = 3;
      strcpy(buffer.eng[0].cle, "12345");
      strcpy(buffer.eng[0].nom, "Baroud");
      strcpy(buffer.eng[0].prenom, "lina");

      strcpy(buffer.eng[1].cle, "67890");
      strcpy(buffer.eng[1].nom, "Hammar");
      strcpy(buffer.eng[1].prenom, "melissa");

      strcpy(buffer.eng[2].cle, "54321");
      strcpy(buffer.eng[2].nom, "Athmane");
      strcpy(buffer.eng[2].prenom, "lina");

      fwrite(&buffer, sizeof(Tbloc), 1, sfsd->f);

      //ajouter un autre bloc avec 1 enregistrement Etudiant
      Tbloc buf;
      buf.nbeng = 1;
      strcpy(buf.eng[0].cle, "14789");
      strcpy(buf.eng[0].nom, "salem");
      strcpy(buf.eng[0].prenom, "hocine");

      fwrite(&buf, sizeof(Tbloc), 1, sfsd->f);
      sfsd->entete.nbblocs=2;
      sfsd->entete.dernierbloc=1;
      fseek(sfsd->f,0,SEEK_SET); // Mettez à jour le fichier après avoir écrit le bloc.
      fwrite(&(sfsd->entete),sizeof(Tentete),1,sfsd->f); //ecrire la nouvelle entete dans le fichier

    do {
        printf("\nMenu:\n");
        printf("1. Affichage\n");
        printf("2. Insertion\n");
        printf("3. Suppression\n");
        printf("4. Recherche\n");
        printf("5. Exit\n");

        printf("Entrez votre choix: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nDonnez le numero de bloc que vous souhaitez afficher: ");
                scanf("%d",&blocIndex);
                LireBloc(sfsd, blocIndex, &b);
                afficher(b, blocIndex);

                break;
            case 2:
               printf("\nDonnez les donnees de l'étudiant que vous souhaitez inserer : \n");
               printf("\nEntrez le nom de l'etudiant : ");
               scanf("%s",&nom);
               printf("\nEntrez le prenom de l'etudiant :");
               scanf("%s",&prenom);
               printf("\nEntrez la cle de l'etudiant: ");
               scanf("%s",&cle);
               Inserer(sfsd, cle, nom, prenom);

                break;
            case 3:
               printf("\nDonnez la clé de l'étudiant que vous souhaitez supprimer : ");
               scanf("%s",&cle);
               suppression(sfsd,cle);
               afficher(b, blocIndex);

                break;
            case 4:
                printf("\nDonnez la clé de l'étudiant que vous souhaitez rechercher : ");
                scanf("%s",&cle);
                int blocTrouve = Recherche(cle,sfsd);
                if(blocTrouve==-1){
                printf("L'étudiant avec la cle %s n'existe pas.\n", cle);
                }else{
                printf("L'étudiant avec la cle %s existe.\n", cle);
                printf("le numero de bloc est: %d\n",blocTrouve);
                }
                break;
            case 5:
                printf("Fermeture du programme.\n");
                break;
            default:
                printf("Choix invalide. Veuillez saisir une option valide.\n");
        }
    } while (choice != 5);

    fclose(sfsd->f);
    return 0;
}

