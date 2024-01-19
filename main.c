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
          if (strcmp(buffer.eng[i].cle,cle)==0)
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
//*******************************************************************************************************************************
//fonction creation
void creation(fichier *file, int nbenreg) {
    Tbloc buffer;
    int nbbloc = (nbenreg + 4) / 5;  // Calculer le nombre total de blocs nécessaires
    file->entete.nbblocs = 0;
    file->entete.dernierbloc = -1;
    file->entete.nblibres = 0;
    fseek(file->f, 0, SEEK_SET);
    fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
    fseek(file->f, sizeof(Tentete), SEEK_SET);
    for (int j = 0; j < nbbloc; j++) {
        int recordsToWrite = (nbenreg > 5) ? 5 : nbenreg;
        memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
        buffer.nbeng = recordsToWrite;
        for (int i = 0; i < recordsToWrite; i++) {
            sprintf(buffer.eng[i].cle, "%d", j * 5 + i + 1);
            sprintf(buffer.eng[i].nom, "nomEtudiant%d", j * 5 + i + 1);
            sprintf(buffer.eng[i].prenom, "prenomEtudiant%d", j * 5 + i + 1);
        }
        fwrite(&buffer, sizeof(Tbloc), 1, file->f);
        file->entete.nbblocs++;
        file->entete.dernierbloc++;
        nbenreg -= recordsToWrite; // Ajuster le nombre restant d'enregistrements à écrire
    }
    fseek(file->f, 0, SEEK_SET);
    fwrite(&(file->entete), sizeof(Tentete), 1, file->f);
}


int main() {

    fichier* sfsd=ouvrir("sfsd.txt",'B');
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
    
     do {
        printf("\nMenu:\n");
        printf("1. Creation\n");
        printf("2. Affichage\n");
        printf("3. Insertion\n");
        printf("4. Suppression\n");
        printf("5. Recherche\n");
        printf("6. Exit\n");

        printf("Entrez votre choix: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nDonnez le nombre d'enregistrement que vous souhaitez creer: ");
                scanf("%d",&nbeng);
                creation(sfsd,nbeng);
                break;
            case 2:  
                do {
                printf("\nDonnez le numero de bloc que vous souhaitez afficher (0 to %d): ", sfsd->entete.nbblocs - 1);
                scanf("%d", &blocIndex);
                if (blocIndex < 0 || blocIndex >= sfsd->entete.nbblocs) {
                    printf("Numéro de bloc invalide. Veuillez entrer un numéro entre 0 et %d.\n", sfsd->entete.nbblocs - 1);
                }
            } while (blocIndex < 0 || blocIndex >= sfsd->entete.nbblocs);  // Repeat until a valid block number is given

              LireBloc(sfsd, blocIndex, &b);
              afficher(b, b.nbeng);

                break;
            case 3:
               printf("\nDonnez les donnees de l'étudiant que vous souhaitez inserer : \n");
               printf("\nEntrez le nom de l'etudiant : ");
               scanf("%s",&nom);
               printf("\nEntrez le prenom de l'etudiant :");
               scanf("%s",&prenom);
               printf("\nEntrez la cle de l'etudiant: ");
               scanf("%s",&cle);
               Inserer(sfsd, cle, nom, prenom);

                break;
            case 4:

               printf("\nDonnez la clé de l'étudiant que vous souhaitez supprimer : ");
               scanf("%s",&cle);
               suppression(sfsd,cle);
               afficher(b, blocIndex);

                break;
            case 5:

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
            case 6:
                printf("Fermeture du programme.\n");
                break;
            default:
                printf("Choix invalide. Veuillez saisir une option valide.\n");
        }
    } while (choice != 6);

    fclose(sfsd->f);
    return 0;
}

