#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#define  cte 310
#define  expr 96100

typedef enum {true=1,false=0} boolean;
typedef struct liste
{
    char   membre[7];
    struct liste *suivant;
    struct liste *tete;
    struct liste *queue;
    int nbrelem;
}liste;

char testString[expr];

int lire(char *chaine, int longueur);
void viderBuffer();
char *space(char *chaine);
char *plusoumoins(char *chaine);
int verification(char *chaine);
double *addition(double x, double y);
double *soustraction(double a,double b);
double *multiplication(double x, double y);
double *division(double x, double y);
double *puissance(double x, double y);
double *factorielle(double x);
int priority(char op);
void vidertmpstring(char *tmpstring);
void videroperateur(char tmp[cte][cte]);
liste *creer_liste();
boolean liste_vide(liste* l);
void ajouter_en_queue(liste *l,char *membre);
char *retirer_en_queue(liste *l);
void empiler(char expression[cte][cte],int *k,char *membre,liste *lst);
void maPile(char* chaine,liste *lst);
double* evaluer(char expression[cte][cte]);
void decallage_binaire(char expression[cte][cte],int *i);
void decallage_unaire(char expression[cte][cte],int *i);

                        /** PROGRAMME PRINCIPAL **/
int main()
{
   int feu;
   liste *lst;
   while(1)
   {
     do
     {
        printf("\n\n\t Entrer Votre expression \n\n\t ");
        feu=lire(testString,expr);
        if(feu==0)
        {
            printf("\n\t N'oubliez pas : Nous sommes en C ; ");
            printf("\n\t Un nombre reel de type double doit avoir au plus 310 caracteres. ");
            printf("\n\n\t");
            system("pause");
            system("cls");
        }
     } while(feu!=1);
     feu=0;
     strcpy(testString,space(testString));
     strcpy(testString,plusoumoins(testString));
     if(verification(testString))
     {
         lst=creer_liste();
         if(lst==NULL)
         {
             perror("\n\t Erreur lors de la creation de la liste. ");
             printf("\n\t Appuyez sur une touche pour quitter le programme. ");
             getch();
             exit(EXIT_FAILURE);
         }
         maPile(testString,lst);
     }
    }
  return 0;
}

/************************************************ PREMIERE ETAPE ******************************************************/

                /** Prendre la chaîne de caractère et supprimer les espaces **/
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
      c = getchar();
    }
}

int lire(char *chaine, int longueur)
{
    char *positionEntree = NULL;

    if (fgets(chaine, longueur, stdin) != NULL) /** Si la lecture se passe bien, on remplace le '\n' par le '\0', on retourne le code de succès : 1 et on sort de la fonction. **/
    {
        positionEntree = strchr(chaine, '\n');
        if (positionEntree != NULL)
        {
          *positionEntree = '\0'; /** C'est ici qu'on remplace le '\n' par le '\0' **/
        }
        else /** Si la lecture c'est faite, et qu'on ne trouve pas le '\n', il y'a une erreur( du au fait que la chaine était trop longue pour nous )
                on vide le flux d'entrée standard stdin et on retourne le code d'erreur 0. **/
        {
          printf("\n\t Votre expression est trop longue ");
          viderBuffer();
          return 0;
        }
      return 1; /** Si on n'arrive ici, c'est que tout c'est bien passée et on retourne le code de succèss 1. **/
    }
    else /** Si pour je ne sais quel raison la lecture c'est la lecture n'a pas pu être faite, il y'a erreur et on retourne le code d'erreur 0. **/
    {
        viderBuffer();
        return 0;
    }
}

char *space(char *chaine)
{
  char *ptr;
  int i=0;

  for(i=0;chaine[i]!='\0';i++)
  {
      if(isspace(chaine[i]))
      {
          ptr=&chaine[i];
          strcpy(ptr,ptr+1);
          --i;
      }
  }

   return chaine;
}
           /*** Reprendre la chaîne de caractère sans espace et arranger les + et - qui se suivent. **/
           /** Soit à dire que - * - == +, + * + == +, - * + == -, + * - == - **/
char *plusoumoins(char *chaine)
{
    char *ptr;
    int nbredeplus,nbredemoins;
    int longueur;
    int i,feu,findesplusoumoins,position;
    i=0;
    feu=0;
    nbredeplus=nbredemoins=findesplusoumoins=position=longueur=0;
    for(i=0;chaine[i]!='\0';i++)
    {
      if(chaine[i]=='+'||chaine[i]=='-')
      {
       position=i;
       if(chaine[i]=='+')
           nbredeplus++;
           if(chaine[i]=='-')
             nbredemoins++;
       while(chaine[i+1]=='+'||chaine[i+1]=='-')
       {
          if(chaine[i+1]=='+')
           nbredeplus++;
           if(chaine[i+1]=='-')
             nbredemoins++;
           i++;
           feu=1;
       }
      }
       if(feu==1)
       {
         nbredemoins%=2;
         nbredeplus%=2;
         findesplusoumoins=i;
         longueur=findesplusoumoins-position+1;
          i-=longueur;
          ptr=&chaine[position];
         if( nbredemoins==0 && (nbredeplus==0||nbredeplus==1))
         {
            *ptr='+';
            strcpy(ptr+1,ptr+longueur);
         }
         if( nbredemoins==1 && (nbredeplus==0||nbredeplus==1))
         {
           *ptr='-';
           strcpy(ptr+1,ptr+longueur);
         }

       }
       nbredemoins=nbredeplus=0;
       feu=0;
    }
    return chaine;
}

/****** DEUXIEME ETAPE : VERIFIER SI LA CHAÎNE QU'ON A FABRIQUEE EST UNE EXPRESSION MATHEMATIQUE BIEN FORMEE ****************/

int verification(char *chaine)
{
    int i,j,feu,parenthese;
    double resultat;
    char *errcode=NULL;
    char *ptr,*ptr1;
    char *tmpstring;

    /** Déclaration des variables globales */
    char fonction[36][8] = {"sin(", "cos(", "tan(","sinh(","cosh(","tanh(","acos(","asin(","atan(","asinh(","acosh(","atanh(","ln(", "log(","sqrt(", "exp(","mod(",
                            "e","E","sin", "cos", "tan","sinh","cosh","tanh","acos","asin","atan","asinh","acosh","atanh","ln", "log","sqrt", "exp","mod"};


    /**  On va s'assurer que le nombre de parenthèse ouvrante '(' est égale au nombre de paranthèse fermente ')' et qu'elles sont dans le bon sens
       qu'on n'a pas par exemple une expression du genre )30+252(. On vérifie par la même occasion que les parenthèses sont non vide.
                                                                  C'est parce que les parenthèses on la plus haute priorité **/
    /** Comptons maintenant le nombre de parenthèse ouvrante et le nombre de parenthèse fermente et comparons. S'il ne sont pas égaux, il y'a erreur. **/
    for ( i=0, parenthese=0; chaine[i]!='\0'; i++ )
    {
        if ( chaine[i]=='(' )
            parenthese++;
        else if ( chaine[i]==')' )
            parenthese--;
    }
    if ( parenthese!=0 )  /** Si le nombre de '(' != du nombre de ')' alors on retourne notre code d'erreur : 0 */
    {
        printf("\n\t Syntax ERROR 1 ");
         return 0;
    }
    /** Le test sur le nombre de parenthèse est terminer**/

  /** Testons  le sens des parenthèses et les parenthèses vide du genre 2+() est terminé. **/

    parenthese=0;i=0,j=0;

    ptr=calloc(sizeof(ptr),strlen(chaine)+1);
    if(ptr!=NULL)
    strcpy(ptr,chaine);
    else
    {
       printf("\n\t Erreur lors de l'allocation memoire. ");
       exit(0);
    }


    while(strchr(ptr,')'))
    {
        ptr1=strchr(ptr,')');
        j=ptr1-ptr;
        for(i=0;i<j-1;i++)
        {
         if(ptr[i]=='(')
         {
          parenthese++;
            break;
         }
        }
        if(parenthese==0)
        {
            printf("\n\t Syntax ERROR  2 ");
            free(ptr);
            return 0;
        }
        parenthese=0;
        strcpy(ptr1,ptr1+1);
    }
    /** le test sur le sens des parenthèses et les parenthèses vide du genre 2+() est terminé. **/

    /** On remplace tous les E par e et toutes les virgules française ',' par les virgules anglaise '.' **/
    for ( i=0; chaine[i]!='\0'; i++ )
    {
        if(chaine[i]=='E')chaine[i]='e';
        else if(chaine[i]==',')chaine[i]='.';
    }

    /** On initialise le tableau qui contient temporairement notre présumé nombre à '\0' pour pouvoir toujour avoir '\0' à la fin de notre chaine. **/
     tmpstring=calloc(cte,sizeof(char));
     if(tmpstring==NULL)
     {
         printf("\n\t Memory ERROR ");
         printf("\n\t Press any key to quit program ... ");
         getch();
         exit(EXIT_FAILURE);
     }
    for ( i=0; chaine[i]!='\0'; i++ )
    {
        /** Si l'expression commence par le symbole du factoriel '!' alors il y'a erreur. **/
        if(chaine[0]=='!'||chaine[0]=='m')
        {
            printf("\n\t Syntax ERROR 3 ");
             return 0;
        }

        /** Si en début d'expression on n'a les opérateurs binaires *, /, ^ alors il y'a erreur. Exemple une expression du genre ^2+5, ou /5+3 ou *555-355
            Si on n'a ces opérateurs binaire après le début de l'expression, et que directement après, on n'a le caractère de fin de ligne '\0' ou la parenthèse fermente ')'
           alors il y'a erreur et on retourne le code d'erreur 0. Exemple : 25+36+  ou (3+225*) ou 19.5+ ou (10+ 20+) ou 50^ ou (50^) imaginez vous même la suite.  */

        if ( chaine[i]=='*' || chaine[i]=='/' || chaine[i]=='^')
        {
          if ( i==0 || chaine[i+1]=='\0' || chaine[i+1]==')' ) /** i==0 permet de savoir si on n'est en début de chaine ou pas, si on n'est pas en début de chaine,
                                                                   on teste donc le caractère suivant qui est dans chaine[i+1] **/
            {
              printf("\n\n\t Syntax ERROR 4 ");
                return 0;
            }

        }

        /** Si on n'a une suite du genre *'\0', +), -), -'\0', on retourne notre code d'erreur.*/

        else if ( chaine[i]=='+' || chaine[i]=='-' )
        {
            if ( chaine[i+1]=='\0' || chaine[i+1]==')' )
            {
                printf("\n\t Syntax ERROR  5 ");
                return 0;
            }
        }

        /** Si deux opérateurs se suivent directement genre on n'a **,!!, ^^,//,^/,/^,  etc... alors on retourne notre code d'erreur 0. */
         if(chaine[i]=='!'&&chaine[i+1]=='!')
        {
            printf("\n\t Syntax ERROR  6 ");
            return 0;
        }

        /** Si '!' est précédé d'autre expression qu'un nombre, ou une ')' alors il y'a erreur */
        if( chaine[i]=='!' && chaine[i-1]!='.' && !isdigit(chaine[i-1]) && chaine[i-1]!=')')
         {
             printf("\n\t Syntax ERROR 7 ");
             return 0;
         }

        /** Sinon si le symbole factoriel '!' est suivit d'un digit ou d'une virgule **/
        else if( chaine[i]=='!' && ( isdigit(chaine[i+1])|| chaine[i+1]=='.' ) )
        {
            printf("\n\t Syntax ERROR 8 ");
             return 0;
        }

        if(chaine[i]=='(')
         {
             if(chaine[i+1]=='*'||chaine[i+1]=='/'||chaine[i+1]=='!')
           {
               printf("\n\t Syntax ERROR 9 ");
               return 0;
           }
         }

        if ( chaine[i]=='^' || chaine[i]=='*' || chaine[i]=='/' || chaine[i]=='+' || chaine[i]=='-' )
        {
          if ( chaine[i+1]=='^' || chaine[i+1]=='*' || chaine[i+1]=='/' || chaine[i+1]=='!' )
            {
              printf("\n\t Syntax ERROR 10 ");
               return 0;
            }
        }
        /** Fin de test sur les opérateurs **/

         /**** recherche de fonction. ***/
        else if(isalpha(chaine[i])) /** Si c'est un caractère alphabétique, je recherche les caractère alphabétique suivant que je tasse ensuite pour pouvoir identifier la fonction.*/
        {

              for(j=0;j<8;j++) // On initialise le tableau qui contient temporairement notre présumé nombre à '\0' pour pouvoir toujour avoir '\0' à la fin de notre chaine.
              tmpstring[j]='\0';
              j=0;
              feu=0;
             /** Si une fontion commençant par m ("mod" ou "mod(" ) est précédée d'une expression incorrecte, alors il y'a erreur et stop on n'arrête les test. **/

             if(chaine[i]=='m' && !isdigit(chaine[i-1]) && chaine[i-1]!='!' && chaine[i-1]!='.' && chaine[i-1]!=')')
               {
                   printf("\n\t Syntax ERROR 11 ");
                   return 0;
               }
             while((chaine[i]=='(')||(j<8&&(isalpha(chaine[i])))) /** Tant que ce sont des caractère alphabétique j'incrémente i pour tester le caractère suivant. */
             {
               tmpstring[j]=chaine[i];
               if(chaine[i]=='(')
                 break;
               j++;
               i++;
               feu=1;
             }
             /** Maintenant on recherche la chaine obtenue par concaténation dans le tableau qui contient toutes nos fonctions mathématiques. */
            j=0;

            while( j<36 && stricmp(fonction[j],tmpstring)!=0)
               j++;
            // Si la fonction n'a pas pu être identifié dans le tableau, alors j vaut 16 et on retourne la valeur d'erreur 0.
             if(j == 36)
             {
                 printf("\n\t Syntax ERROR  12 ");
                  return 0;
             }
             if(stricmp(tmpstring,"e")==0)
             {
                 if(i-1==0)
                 {
                     printf("\n\t Syntax ERROR  13 ");
                     return 0;
                 }
             }
                if(chaine[i]!='(')
                {
                   if(!(isdigit(chaine[i])))
                      {
                          if(chaine[i]!='+'&&chaine[i]!='-')
                           {
                             printf("\n\t Syntax ERROR 14 ");
                             return 0;
                           }
                           if( !isdigit(chaine[i+1]) )
                           {
                              printf("\n\t Syntax ERROR  15 ");
                              return 0;
                           }
                      }
                }
             if(feu==1) // Que la fonction aie été trouvée ou pas, si le while a été fait au moins une fois, alors i avait été incrémentée une fois sans avoir été utilisé.
              {
                  --i;
                  feu=0;
              }
             for(j=0;j<8;j++) // On réinitialise le tableau qui contient temporairement nos présumées fontions à '\0' pour pouvoir toujour avoir '\0' à la fin de notre chaine.
              tmpstring[j]=0;
        }
        /** Recherche de fontion terminée **/

        /** Si le caractère est inconnu. **/
        else if ( !isdigit(chaine[i]) && chaine[i]!='(' && chaine[i]!=')' && chaine[i]!='.' && chaine[i]!='!')
        {
            printf("\n\t Syntax ERROR 16 ");
            return 0;
        }

         /** Si c'est un chiffre, alors l'expression est un nombre on va copier le nombre et vérifier qu'il est bon.*/
        if( isdigit(chaine[i])||chaine[i]=='.')
        {
           j=0;
           feu=0;
           if(chaine[i-1]=='+' || chaine[i-1]=='-')
           {
            /** On va vérifier que ce + ou - unaire avec un if avant de le copier dans tmpString. **/
            if(i-1==0 || chaine[i-2]=='(' || chaine[i-2]=='*' || chaine[i-2]=='/' || chaine[i-2]=='^' || isalpha(chaine[i-2]))
             {
               tmpstring[j]=chaine[i-1];
               ++j;
             }
           }

          while((j<cte)&&(isdigit(chaine[i])||chaine[i]=='.'||chaine[i]=='e')) /** Le nombre peut être un réel d'où la présence de la virgule anglais '.' ou  française ',' */
          {
            tmpstring[j]=chaine[i]; /** On copie le nombre dans la variable tampon tmpstring tout en sachant que le nombre ne doit pas avoir plus de 15 caractères. */
            j++;
            i++;

            if(chaine[i-1]=='e'&&(chaine[i]=='+'||chaine[i]=='-'))
            {
                tmpstring[j]=chaine[i];
                ++j;
                ++i;
            }
            else if( chaine[i]=='e' && !isdigit(chaine[i+1]) && chaine[i+1]!='+' && chaine[i+1]!='-' )
            {
                printf("\n\t Syntax ERROR 17 ");
                return 0;
            }
           feu=1;
          }

         if(feu==1)  /** Si nous sommes entrée dans la boucle while, i a été incrémentée au moins une fois sans avoir toute fois été utilisée. Donc on va le décrémenter de 1. */
         {
             --i;
             feu=0;
         }

           /** On convertit la chaine de caractère tmpstring en nombre et on y recherche des erreurs */
            resultat=strtod(tmpstring,&errcode);
            if (strlen(errcode)>1 )  /** S'il y a eu erreur, si aucun nombre n'est lu on arrête la vérification */
            {
              printf("\n\t Syntax ERROR 18 ");
              return 0;
            }
             else if(resultat == HUGE_VALF || errno==ERANGE ) /** S'il y'a eu overflow : Le nombre est hors de l'intervalle des doubles */
             {
               printf("\n\t Overflow \n");
               errno=0;
                  return 0;
             }
             else if(resultat==-HUGE_VALF || errno==ERANGE)
             {
                 printf("\n\t Underflow ");
                 errno=0;
                 return 0;
             }

           for(j=0;j<cte;j++) /** On réinitialise le tableau qui contient temporairement notre présumé nombre à '\0' pour pouvoir toujour avoir '\0' à la fin de notre chaine. */
           tmpstring[j]='\0';
        }
    }
    free(tmpstring);
    return 1;
}

               /*********** TROISIEMME ETAPE : EVALUER L'EXPRESSION BIEN FORMEE. ****************/

/** On va maintenant créer la pile( en fait c'est tableau bidimentionnel dynamique ) qui va contenir l'expression à évalue. **/

/** Priorité des opérateurs */
int priority(char op)
{

       if(op=='(')
        return 5;
       if(isalpha(op))
        return 4;
        else if (op == '!')
            return 3;
        else if(op == '^')
            return 2;
        else if((op == '*') || (op== '/'))
            return 1;
        else if((op == '+') || (op == '-'))
            return 0;
        else
            return -1;
        return -1;
}

void vidertmpstring(char *tmpstring)
{
         int i;
         for(i=0;i<20;i++)
            tmpstring[i]='\0';
}

void videroperateur(char tmp[cte][cte])
{
        int i,j;
        for(i=0;i<cte;i++)
         for(j=0;j<cte;j++)
          tmp[i][j]='\0';
}

liste *creer_liste()
{
    liste *l;
    l=malloc(sizeof(liste));
    if(l==NULL)
    {
        perror("\n\t Memory error ! ");
        printf("\n\t Press any key to quit the program ... ");
        getch();
        exit(EXIT_FAILURE);
    }
    l->tete=l->queue=NULL;
    l->nbrelem=0;
    return l;
}

boolean liste_vide(liste* l)
{
    if(l->nbrelem == 0)
     return true;
    else
     return false;
}

void ajouter_en_queue(liste *l,char *membre)
{
    liste *e;
    e=malloc(sizeof(liste));
    if(e==NULL)
    {
        perror("\n\t Memory default ");
        getch();
        exit(0);
    }
    else if(e!=NULL)
    {
        strcpy(e->membre,membre);
        e->suivant=NULL;

        if(l->tete==NULL)
        {
            l->tete=e;
        }

        if(l->queue!=NULL)
        {
            l->queue->suivant=e;
        }
        l->queue=e;
        (l->nbrelem)++;
    }
    //printf("\n\t Ajout en queue avec success ");
}

char* retirer_en_queue(liste *l)
{
    liste *pred=NULL, *partant;
    char  *res;

    partant=l->tete;

    while(partant->suivant!=NULL)
    {
        pred=partant;
        partant=partant->suivant;
    }
    res=malloc((strlen(partant->membre)+1)*sizeof(char));
    if(res==NULL)
    {
      perror("\n\t Erreur de memoire ");
      printf("\n\t Appuyez sur n'importe quelle touche pour quitter le programme. ");
      getch();
      exit(0);
    }
    strcpy(res,partant->membre);
    free(partant);
    l->queue=pred;
    if(pred!=NULL) /** S'il y'avait au moins deux elements dans la liste **/
      pred->suivant=NULL; /** Alors le suivant du dernier element doit pointer sur NULL **/
    else
     l->tete=NULL; /** S'il n'y avait qu'un seul element alors la tete et la queue pointent sur NULL **/
    l->nbrelem--;
    return res;
}

void empiler(char expression[cte][cte],int *k,char *membre,liste *lst)
{
    liste *element;

        element=lst->queue;
        if(liste_vide(lst))
            ajouter_en_queue(lst,membre);
        else if((element->membre[0]=='(')||priority(membre[0])>priority(element->membre[0]))
        {
            ajouter_en_queue(lst,membre);
        }
        else if(priority(membre[0])<=priority(element->membre[0]))
        {
            while((!liste_vide(lst))&&(element->membre[0]!='(')&&
                                (priority(membre[0])<=priority(element->membre[0])))
            {
                strcpy(expression[*k],retirer_en_queue(lst));
                ++(*k);
                element=lst->queue;
            }
            ajouter_en_queue(lst,membre);
        }
}

void maPile(char* chaine,liste *lst)
{
       double *res;
       unsigned int i, j, feu;
       int   k, pouvrante, parenthese; /*r est utilisé pour cherchez les erreurs. */
       char  expression[cte][cte];
       liste *element;
       char  membre[cte];


            k=0;/** Pour parcourir la matrice expression. **/
           j=0;
          feu=0;
        videroperateur(expression);
      vidertmpstring(membre);
    for ( i=0; chaine[i]!='\0'; i++ )
    {
           /** Pour commencer on remplie notre table qui nous sert de tampon membre du caractère de fin de ligne '\0'
                Ceci ce fait grâce à la procédure vidertmpstring. **/


            element=lst->queue;
                /** Maintenant on va Appliquer la NPI(Notation Polonaise Inversé) sur notre chaine passé en argument. **/

           /** Si c'est un '+' ou '-' unaire, on le met dans notre tampon membre. un '+' ou '-' en début de notre variable
             tampon membre signifie que ce '+' ou '-' est unaire.  **/
            if((chaine[i]=='+'||chaine[i]=='-')&&(i==0 || chaine[i-1]=='(' ||
                chaine[i-1]=='*' || chaine[i-1]=='/' || chaine[i-1]=='^' || isalpha(chaine[i-1])) )
            {
                membre[j]=chaine[i];
                j++;
            }
           /** On recherche le début et la fin d'un nombre dans la variable tampon membre
           qu'on copie ensuite dans la matrice expression si c'est un nombre. **/
           else if(isdigit(chaine[i])||chaine[i]=='.')
           {
                   if(i>0&&chaine[i-1]==')')/** Si on n'a par exemple (25+25)777, il faudra signaler cette parenthèse fermente trouvée et considéré */
                   {                               /** l'existence d'un opérateur '*' entre le digit et la parenthèse fermente. **/

                     /** Insertion de '*'. **/
                      membre[0]='*';
                      empiler(expression,&k,membre,lst);
                      vidertmpstring(membre);
                      j=0;
                   }
                   if(membre[0]=='+')
                   {
                      membre[0]='\0';
                      j=0;
                   }
                /** Tant que la case suivante est un digit ou une virgule, on la met dans la variable tampon membre **/
                    while(isdigit(chaine[i]) || chaine[i]=='.'||chaine[i]=='e')
                    {
                        membre[j]=chaine[i];
                        i++;
                        j++;
                        if(chaine[i-1]=='e'&&(chaine[i]=='+'||chaine[i]=='-'))
                        {
                         membre[j]=chaine[i];
                         ++j;
                         ++i;
                        }
                        feu=1;
                    }
                 /** Si feu == 1, on n'est entrée au moins une fois dans le while et i a été incrémenté au moins une fois sans avoir été utilisée. **/
                   if(feu==1)
                   {
                     --i;     /** C'est pour cette raison qu'on va décrémenter i à ce niveau. **/
                     feu=0;  /** On remet feu à zéro pour une possible prochaine utilisation. **/
                   }
                  strcpy(expression[k],membre);/** On copie ce que contient membre dans la matrice expression à la ligne k **/
                 ++k; /** Puis on prépare la matrice expression à acceuillir la prochaine expression en incrémentant i de 1. **/
                vidertmpstring(membre);
                j=0; /** Je met i à 0 parce que c'est lui qui me permet de parcourir  membre. **/
               //printf("\n\t Partie digit copie de membre dans la matrice expression ");
            }
              /** Copie de nombre terminée avec SUCCES. **/

                /*** Recherche d'une fonction  ***/
           else if (isalpha(chaine[i]))/** Si c'est un caractère alphabétique 'A'..'Z' ou 'a'..'z' **/
           {
            /** Si avant la fonction, il y'a directement un digit exp : 2sin30 ou un '!' exp : 3!cos60
             ou une ')' (25+25)sin60 ,et que cette fonction n'est n'est mod exp 5mod2,
                alors '*' est placé implicitement entre les deux. **/
               if(isdigit(chaine[i-1])||chaine[i-1]=='!'||chaine[i-1]==')')
               {
                   if(chaine[i]!='m'&&chaine[i]!='M')
                    {
                      /** Insertion de '*'. **/
                      membre[0]='*';
                      empiler(expression,&k,membre,lst);
                      vidertmpstring(membre);
                      j=0;
                    }
               }

               /** Ici, on va traité du + ou - unaire devant une fonction. **/
               /** Si c'est un '-' unaire, exemple : -sin30 = -1 * sin30 **/
               else if(membre[0]=='-')
               {
                  strcpy(expression[k],"-1");
                  k++;
                  /** Insertion de '*' **/
                  membre[0]='*';
                  empiler(expression,&k,membre,lst);
                  vidertmpstring(membre);
                  j=0;
                }
                else if(membre[0]=='+')
                {
                  membre[0]='\0';  /** En fait on ne tient pas compte du + unaire devant un nombre **/
                  j=0;                 /** encore moins devant une fonction. **/
                }
                /** Traitement du + ou - unaire terminé. **/

                while(isalpha(chaine[i])) /** Tant le caractère suivant est alphabétique, alors on le copie dans membre. **/
                {
                    membre[j]=chaine[i];
                    j++;
                    i++;
                    feu=1;
                }
                /** Si feu == 1, on n'est entrée au moins une fois dans le while et i a été incrémenté au moins une fois
                sans avoir été utilisée. **/
                if(feu==1)
                {
                    --i;
                    feu=0;
                }
                ajouter_en_queue(lst,membre); /** Ici, membre contient la fonction recherché mais sans la parenthèse et **/
                                                /** sans le '+' ou '-' unaire. **/
                vidertmpstring(membre);
                j=0;
                feu=0;
           }
            /** Traitment sur les fonctions terminées avec succès. **/

                /*** Traitons maintenant des Opérateurs  ***/
                /** Je tient à préciser qu'ici, le '+' ou le '-' est forcément binaire. **/
                /** On va l'empiler dans la pile des opérateurs selon sa priorité il sera remplaçant ou mis au dessus. **/

           else if ( chaine[i]=='+' || chaine[i]=='*' || chaine[i]=='/' ||
                     chaine[i]=='^' || chaine[i]=='-' || chaine[i]=='!' )
           {
                membre[0]=chaine[i];
                empiler(expression,&k,membre,lst);
                vidertmpstring(membre);
                j=0;
           }

         /** On va maintenant traiter de ce qui se trouve à l'intérieur des Parenthèses. */
           else if (chaine[i]=='(') /** Si c'est une parenthèse ouvrante '(' alors on y va ... ***/
           {
              parenthese=-1;
              pouvrante=0;
              for(i=i; chaine[i]!=')'||parenthese!=0; i++)
              {

                  /** Dans ce for, c'est expressément qu'on teste chaine[i] avant parenthese. On sort de ce for lorsque
                    chaine[i]==')' et parenthese == 0. N'oublions pas que sortir de ce for c'est sortir de la condition
                  else if. Donc il faudra vider la pile des opérateurs jusqu'à trouver '(' à la sortie de else if. **/

                  /** Si derrière une parenthèse ouvrante '(' on n'a un digit Exemple : 25(25+35)
                      ou une parenthèse fermente ')' EXP : (25+35)(60+40) ou un '+' ou '-' unaire EXP : -(25+35),
                      alors il y'a un signe '*' implicitement placé entre les deux. **/
                     if((i>0) && chaine[i]=='(' && (chaine[i-1]==')' || isdigit(chaine[i-1]))) /** i<0 est pour éviter d'avoir à tester une case avec un indice négatif i>0 ==> i-1 >=0 **/
                     {
                       /** Insertion de '*'. **/
                        membre[0]='*';
                        empiler(expression,&k,membre,lst);
                        vidertmpstring(membre);
                        j=0;
                     }
                     /** Si on n'a plus tôt un '+' ou '-' unaire, un traitement similaire s'impose. **/
                     else if((i>0) && chaine[i]=='(' && i-1==0 && (chaine[i-1]=='+'||chaine[i-1]=='-'))
                     {
                          if(chaine[i-1]=='-')
                          {
                               strcpy(expression[k],"-1");
                               ++k;
                               /** Insertion de '*'. **/
                               membre[0]='*';
                               empiler(expression,&k,membre,lst);
                               vidertmpstring(membre);
                               j=0;
                           }
                     }
                    /** Insertion s'il était nécessaire, terminé. **/

                    /** Dans tous les cas, il faut empiler la parenthèse ouvrante '(' **/
                     if(chaine[i]=='(')
                     {
                        membre[0]='(';
                        ajouter_en_queue(lst,membre);
                        vidertmpstring(membre);
                        j=0;
                        parenthese++;
                        pouvrante++;
                     }
                     else if ( chaine[i]==')' )
                        parenthese--;

                     /** Si c'est un '+' ou '-' unaire, on le met dans notre tampon membre. un '+' ou '-'
                        en début de notre variable tampon(membre)signifie que ce '+' ou '-' est unaire.  **/
                   if((chaine[i]=='+'||chaine[i]=='-') && (i==0 || chaine[i-1]=='(' || chaine[i-1]=='*'
                       || chaine[i-1]=='/' || chaine[i-1]=='^' || isalpha(chaine[i-1])))
                   {
                        membre[j]=chaine[i];
                        j++;
                   }
                   /** On recherche le début et la fin d'un nombre dans la variable tampon membre qu'on copie ensuite dans la matrice expression*/
                   /** Si c'est un nombre. **/
                   else if(isdigit(chaine[i])||chaine[i]=='.')
                   {
                        if(chaine[i-1]==')')/** Si on n'a par exemple (25+25)777, il faudra signaler cette parenthèse fermente trouvée et considéré */
                        {                               /** l'existence d'un opérateur '*' entre le digit et la parenthèse fermente. **/

                          /** Insertion de '*'. **/
                            membre[0]='*';
                            empiler(expression,&k,membre,lst);
                            vidertmpstring(membre);
                            j=0;
                        }
                        if(membre[0]=='+')
                        {
                             membre[0]='\0';
                            j=0;
                        }
                        /** Tant que la case suivante est un digit ou une virgule, on la met dans la variable tampon membre **/
                        while(isdigit(chaine[i]) || chaine[i]=='.'||chaine[i]=='e')
                        {
                            membre[j]=chaine[i];
                            i++;
                            j++;
                            if(chaine[i-1]=='e'&&(chaine[i]=='+'||chaine[i]=='-'))
                            {
                                membre[j]=chaine[i];
                                ++j;
                                ++i;
                            }
                            feu=1;
                        }
                        /** Si feu == 1, on n'est entrée au moins une fois dans le while et i a été incrémenté au moins une fois sans avoir été utilisée. **/
                        if(feu==1)
                        {
                            --i;     /** C'est pour cette raison qu'on va décrémenter i à ce niveau. **/
                            feu=0;  /** On remet feu à zéro pour une possible prochaine utilisation. **/
                        }
                         /** On copie ce que contient membre dans la matrice expression à la ligne k **/
                        strcpy(expression[k],membre);
                        /** Puis on prépare la matrice expression à acceuillir la prochaine expression en incrémentant i **/
                        k++;
                        vidertmpstring(membre);
                        j=0; /** Je met i à 0 parce que c'est lui qui me permet de parcourir  membre. **/
                   }
                   /** Copie de nombre terminée avec SUCCES. **/

                    /** Les sinon si sont dû au fait que si ce n'est pas un digit, alors c'est soit :
                    une fonction, un opérateur ou bien une parenthèses ouvrante '('. **/


                   /*** Recherche d'une fonction  ***/
                   else if ( isalpha(chaine[i]) )/** Si c'est un caractère alphabétique 'A'..'Z' ou 'a'..'z' **/
                   {
                        if(isdigit(chaine[i-1])||chaine[i-1]=='!'||chaine[i-1]==')')
                        {
                            if(chaine[i]!='m'&&chaine[i]!='M')
                            {
                                /** Insertion de '*'. **/
                                membre[0]='*';
                                empiler(expression,&k,membre,lst);
                                vidertmpstring(membre);
                                j=0;
                            }
                        }

                        /** Ici, on va traité du + ou - unaire devant une fonction. **/
                        /** Si c'est un '-' unaire, exemple : -sin30 = -1 * sin30 **/
                        if(membre[0]=='-')
                        {
                            strcpy(expression[k],"-1");
                            k++;
                            /** Insertion de '*' **/
                            membre[0]='*';
                            empiler(expression,&k,membre,lst);
                            //vidertmpstring(membre);
                            //j=0;
                        }
                        else if(membre[0]=='+')
                        {
                            membre[0]='\0';  /** En fait on ne tient pas compte du + unaire devant un nombre **/
                            j=0;                 /** encore moins devant une fonction. **/
                        }
                        /** Traitement du + ou - unaire terminé. **/

                        while(isalpha(chaine[i])) /** Tant le caractère suivant est alphabétique, alors on le copie dans membre. **/
                        {
                            membre[j]=chaine[i];
                            j++;
                            i++;
                            feu=1;
                        }
                        /** Si feu == 1, on n'est entrée au moins une fois dans le while et i a été incrémenté au moins une fois
                                sans avoir été utilisée. **/
                        if(feu==1)
                        {
                            --i;
                            feu=0;
                        }
                        ajouter_en_queue(lst,membre); /** Ici, membre contient la fonction recherché mais sans la parenthèse et **/
                                                      /** sans le '+' ou '-' unaire. **/
                        vidertmpstring(membre);
                        j=0;
                        feu=0;

                   }
                    /** Traitment sur les fonctions terminées avec succès. **/

                    /*** Traitons maintenant des Opérateurs  ***/
                    /** Je tient à préciser qu'ici, le '+' ou le '-' est forcément binaire. **/
                    /** On va l'empiler dans la pile des opérateurs selon sa priorité il sera remplaçant ou mis au dessus. **/

                   else if ( chaine[i]=='+' || chaine[i]=='*' || chaine[i]=='/' || chaine[i]=='^' || chaine[i]=='-' || chaine[i]=='!' )
                   {
                        membre[0]=chaine[i];
                        empiler(expression,&k,membre,lst);
                        vidertmpstring(membre);
                        j=0;
                   }
                     /** Si on trouve une parenthèse fermente dans le if, alors il y'a forcément une parentèse non vide
                     ouverte derrière elle. On va dépiler la matrice operateur jusqu'à trouver cette parenthèse là. **/
                   if(pouvrante>1 && chaine[i]==')')
                   {
                      element=lst->queue;
                        while(element->membre[0]!='(')
                        {
                           strcpy(expression[k],retirer_en_queue(lst));
                           element=lst->queue;
                           ++k;
                        }
                        --pouvrante;
                        retirer_en_queue(lst);
                      //  vidertmpstring(membre);
                      //  j=0;
                   }
            }
             feu=1;
        }

            /** Là nous sommes hors du else if( chaine[i]='(' ), donc comme prévue il faut vider la pile des opérateurs.
                Jusqu'à la parentèse ouvrante qui bien sûre ne sera pas empiler. **/
            if(feu==1) /** feu == 1 me rappel que j'étais dans le else if. **/
            {
               element=lst->queue;
               /** A la fin d'une parenthèse principal, il faut dépiler jusqu'à retrouver cette parenthèse. **/
               while(element->membre[0]!='(')
                {
                    strcpy(expression[k],retirer_en_queue(lst));
                    element=lst->queue;
                    ++k;
                }
                retirer_en_queue(lst);
               // vidertmpstring(membre);
               // j=0;
                feu=0;
            }
            if(chaine[i+1]=='\0')
            {
                while(!liste_vide(lst))
                {
                    strcpy(expression[k],retirer_en_queue(lst));
                    ++k;
                }
            }
        }
        /** Ici, on n'a fini de construire la pile. Donc on va gentilment visualiser son contenu.  **/


       //  Regardons ce qu'il y'a dans la matrice expression.
       //      printf("\n\n\t L'expression est :\n\n\t ");
      //        for(r=0;r<k;r++)
      //         printf("%s ",expression[r]);
      //  Oberservation de la matrice terminée.
      res=evaluer(expression);
        if(res!=NULL)
       printf("\n\n\t Le resultat est : %lg \n",*res);
}

double* evaluer(char expression[cte][cte])
{
    int i,j;
    double res,x,y,*valeur;
    j=0;
    res=0.0;
    for(i=0;expression[i][0]!='\0';i++)
    {
         if(stricmp(expression[i],"+")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             valeur=addition(x,y);
             if(valeur==NULL)
              return NULL;
             res=*valeur;
             sprintf(expression[i],"%lg" ,res);
               /** Décalage vers i ( vers le bas ) **/
               decallage_binaire(expression,&i);
         }
         else if(stricmp(expression[i],"-")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             valeur=soustraction(x,y);
             if(valeur==NULL)
              return NULL;
             res=*valeur;
             sprintf(expression[i],"%lg" ,res);
             /** Décalage **/
            decallage_binaire(expression,&i);
         }

         else if(stricmp(expression[i],"*")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             valeur=multiplication(x,y);
             if(valeur==NULL)
              return NULL;
             res=*valeur;
             sprintf(expression[i],"%lg" ,res);
             /** Décalage **/
            decallage_binaire(expression,&i);
           }
         else if(stricmp(expression[i],"/")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             valeur=division(x,y);
             if(valeur==NULL)
              return NULL;
             res=*valeur;
             sprintf(expression[i],"%lg" ,res);
             /** Décalage **/
          decallage_binaire(expression,&i);
         }
         else if(stricmp(expression[i],"^")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             valeur=puissance(x,y);
             if(valeur==NULL)
              return NULL;
             else
              res=*valeur;
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                decallage_binaire(expression,&i);
         }
         else if(stricmp(expression[i],"!")==0)
         {

             x=strtod(expression[i-1],NULL);
             valeur=factorielle(x);
             if(valeur==NULL)
              return NULL;
             else
              res=*valeur;
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                if(i-2>=0)
                {
                   j=i-2;
                   while(j>=0)
                   {
                       strcpy(expression[j+1],expression[j]);
                       --j;
                   }
                }
         }

         else if(stricmp(expression[i],"mod")==0)
         {
             x=strtod(expression[i-2],NULL);
             y=strtod(expression[i-1],NULL);
             res=fmod(x,y);
             if(y==0)
             {
                printf("\n\t Error Division by zero ");
                return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                decallage_binaire(expression,&i);
         }

         else if(stricmp(expression[i],"sin")==0)
         {

             x=strtod(expression[i-1],NULL);
             if(fmod(x,180)==0)
              res=0;
              else if(fmod(x,180)!=0)
              {
                x*=M_PI/180;
                res=sin(x);
              }
             if(res==HUGE_VALF||res==-HUGE_VALF||res==ERANGE)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
            decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"cos")==0)
         {

             x=strtod(expression[i-1],NULL);
             x*=M_PI/180;
             if(fmod((x-M_PI),(2*M_PI))==0)
              res=cos(M_PI);
                else
                  res=cos(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
            decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"tan")==0)
         {

             x=strtod(expression[i-1],NULL);
             x*=M_PI/180;
             if(fmod((x-M_PI_2),M_PI)==0)
             {
                printf("\n\t Erreur de Domaine ");
                return NULL;
             }
             else if(fmod((x-M_PI),M_PI)==0)
              res=tan(0);
             else
              res=tan(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"sinh")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=sinh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
              decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"cosh")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=cosh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
          decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"tanh")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=tanh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
              decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"asin")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=asin(x);
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             res*=180/M_PI;
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"acos")==0)
         {
             x=strtod(expression[i-1],NULL);
             res=acos(x);
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             res*=180/M_PI;
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
             decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"atan")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=atan(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             res*=180/M_PI;
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"asinh")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=asinh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"acosh")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=acosh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"atanh")==0)
         {
             x=strtod(expression[i-1],NULL);
             res=atanh(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"ln")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=log(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
                decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"log")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=log10(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }
         else if(stricmp(expression[i],"exp")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=exp(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
               decallage_unaire(expression,&i);
         }

         else if(stricmp(expression[i],"sqrt")==0)
         {

             x=strtod(expression[i-1],NULL);
             res=sqrt(x);
             if(res==HUGE_VALF||res==-HUGE_VALF)
             {
                 printf("\n\t Math ERROR ");
                 errno=0;
                 return NULL;
             }
             if(errno==EDOM)
             {
                 printf("\n\t Erreur de Domaine ");
                 errno=0;
                 return NULL;
             }
             sprintf(expression[i],"%lg" ,res);
                /** Décalage **/
            decallage_unaire(expression,&i);
         }


    }
    --i;
    res=strtod(expression[i],NULL);
    valeur=&res;
    return valeur;
}

void decallage_binaire(char expression[cte][cte],int *i)
{
    int j;

               if(*i-3>=0)
               {
                   j=*i-3;
                   while(j>=0)
                   {
                       strcpy(expression[j+2],expression[j]);
                       --j;
                   }

               }
}

void decallage_unaire(char expression[cte][cte],int *i)
{
    int j;

               if(*i-2>=0)
               {
                   j=*i-2;
                   while(j>=0)
                   {
                       strcpy(expression[j+1],expression[j]);
                       --j;
                   }

               }
}

double *puissance(double x, double y)
{

    double res, *valeur;

              res=pow(x,y);
              if(errno==EDOM)
              {
                  printf("\n\t Domain Error ");
                  errno=0;
                  return NULL;
              }
              if(res==HUGE_VALF)
              {
                  printf("\n\t Overflow  ");
                  errno=0;
                  return NULL;
              }
              if(res==-HUGE_VALF)
              {
                  printf("\n\t Underflow ");
                  errno=0;
                  return NULL;
              }
              if(errno==ERANGE)
              {
                  printf("\n\t Math Error ");
                  errno=0;
                  return NULL;
              }
        valeur=&res;
    return valeur;
}

double *factorielle(double x)
{
      double fact=1.0,*res,decimal,integer;

       decimal=modf(x,&integer);

       if(x<0||decimal!=0)
       {
           printf("\n\t Math ERROR ");
           return NULL;
       }
        if(x<=1)
        {
           res=&fact;
           return res;
        }

       else if(x>1)
       {
           while(x>1)
           {
               fact=fact*x;
               if(fact==HUGE_VAL || errno==ERANGE)
               {
                   printf("\n\t Overflow");
                   errno=0;
                   return NULL;
               }
               if(fact==0 || fact==-HUGE_VALF)
               {
                    printf("\n\t Underflow ");
                    errno=0;
                    return NULL;
               }
               --x;
           }
       }
       res=&fact;
       return res;
}

double *addition(double a, double b)
{
    double res, *valeur;

     res=a+b;
     if(res==HUGE_VALF ||errno==ERANGE)
     {
         printf("\n\t Overflow ");
         errno=0;
         return NULL;
     }
     if(res==-HUGE_VALF)
     {
         printf("\n\t Underflow ");
         errno=0;
         return NULL;
     }
    valeur=&res;
    return valeur;
}

double *soustraction(double a,double b)
{
    double res, *valeur;

     errno=0;
     res=a-b;
     if(res==HUGE_VALF || errno==ERANGE)
     {
         printf("\n\t Overflow ");
         errno=0;
         return NULL;
     }

     if(res==-HUGE_VALF)
     {
         printf("\n\t Underflow ");
         errno=0;
         return NULL;
     }
    valeur=&res;
    return valeur;
}

double *multiplication( double a,  double b)
{
    double res, *valeur;
     res=a*b;

     if(res==HUGE_VALF || errno==ERANGE )
     {
         printf("\n\t Overflow ");
         errno=0;
         return NULL;
     }
     if(res==-HUGE_VALF)
     {
         printf("\n\t Underflow ");
         errno=0;
         return NULL;
     }
    valeur=&res;
    return valeur;
   return NULL;
}

double *division(double a, double b)
{
     double res, *valeur;

     errno=0;
     res=a/b;
     if(b==0)
     {
         printf("\n\t Divizion by zero ");
         return NULL;
     }
     if(res==HUGE_VALF ||errno==ERANGE)
     {
         printf("\n\t Overflow ");
         errno=0;
         return NULL;
     }
     if(res==-HUGE_VALF)
     {
         printf("\n\t Underflow ");
         errno=0;
         return NULL;
     }
    valeur=&res;
    return valeur;
}

