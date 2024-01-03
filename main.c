#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Tenreg {

  char nom[20];
  char prenom[20];
  char cle[6];
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

typedef struct fichier {
  FILE* f;
  Tentete entete;

}fichier;

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


// Function to read a block of student records from the file into a buffer

void LireBloc(fichier *file, int i, Tbloc *buf) {

    fseek(file->f, sizeof(Tentete) + i * sizeof(Tbloc), SEEK_SET);  // Move the file pointer to the appropriate position for the specified block
    // i is an index that specify which block to read from the file.
    fread(buf, sizeof(Tbloc), 1, file->f);    // Read the block of records into the provided buffer
}

void Recherche(char key[], fichier *file){

  bool trouve=false;
  int i=0;

  fseek(file->f, sizeof(Tentete), SEEK_SET);  //Move the file pointer past the header


  if (file->entete.nbblocs == 0) {
        printf("Erreur : le fichier est vide.\n");
        return;
    }
 // Initializes the entire structure to zero

while(i<file->entete.nbblocs && !trouve){
    Tbloc buffer;
    LireBloc(file,i,&buffer);
      //buffer contains the bloc with index i
    int j=0;
    while(j<buffer.nbeng && !trouve){
      if(strcmp(buffer.eng[j].cle, key) == 0){
          trouve=true;
      }else {j++;}
    }
    i++;
}
if(trouve==true){
   printf("L'étudiant avec la cle %s existe.\n", key);
   }else
    printf("L'étudiant avec la cle %s n'existe pas.\n", key);
}

int main() {

    fichier* sfsd=ouvrir("sfsd",'B');

    // Check if the file is successfully opened
    if (sfsd->f == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    // Writing a block of records (Tbloc) with two student records (Tenreg)
    Tbloc block;
    block.nbeng = 2;

    strcpy(block.eng[0].cle, "12345");
    strcpy(block.eng[0].nom, "Doe");
    strcpy(block.eng[0].prenom, "John");
    block.eng[0].eff = true;

    strcpy(block.eng[1].cle, "67890");
    strcpy(block.eng[1].nom, "Smith");
    strcpy(block.eng[1].prenom, "Alice");
    block.eng[1].eff = true;

    // Write the block to the file
    fwrite(&block, sizeof(Tbloc), 1, sfsd->f);

    Tbloc anotherBlock;
    anotherBlock.nbeng = 1;
    strcpy(anotherBlock.eng[0].cle, "54321");
    strcpy(anotherBlock.eng[0].nom, "Doe");
    strcpy(anotherBlock.eng[0].prenom, "Jane");
    anotherBlock.eng[0].eff = true;

    fwrite(&anotherBlock, sizeof(Tbloc), 1, sfsd->f);

    sfsd->entete.nbblocs=2;
    Recherche("67890", sfsd);

    fclose(sfsd->f);

    return 0;
}

