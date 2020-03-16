#define _BSD_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>
#define IS_CHILD == 0
#define IS_PARENT > 0
#define MSJ_LOGAT "Logat cu succes"
#define MSJ_NELOGAT "Nelogat!Try again!"

void myfind(char*, const char*);
void mystat(char*);

pid_t pid;
int socketCami[2], eLogat;
int p[2];

int main()
{
    socketpair(AF_UNIX, SOCK_STREAM, 0, socketCami);
    pipe(p);
    pid = fork();

    if (pid == -1)
    {
        printf("Eroare la fork!!!");
        exit(1);
    }

    if (pid IS_CHILD)
    {
        close(socketCami[1]);
        fflush(stdout);
        int lgUser;
        char *userIntrodus = (char *)malloc(400);
        char raspuns[400];

        while (!eLogat)
        {
            memset(raspuns, 0, 400);
            memset(userIntrodus, 0, 400);
            read(socketCami[0], &lgUser, sizeof(int));
            read(socketCami[0], userIntrodus, lgUser);
            FILE *fisierUsers = fopen("users.txt", "r");

            while (!feof(fisierUsers))
            {
                fgets(raspuns, 400, fisierUsers);
                raspuns[strlen(raspuns) - 1] = 0;
                if (strcmp(raspuns, userIntrodus) == 0)
                {
                    eLogat = 1;
                    break;
                }
            }

            fclose(fisierUsers);
            char mesaj[100];
            int lgMesaj;
            memset(mesaj, 0, 100);

            if (eLogat)
                strcpy(mesaj, MSJ_LOGAT);
            else
                strcpy(mesaj, MSJ_NELOGAT);

            lgMesaj = strlen(mesaj);
            write(socketCami[0], &lgMesaj, sizeof(int));
            write(socketCami[0], mesaj, lgMesaj);
        }
        char *comanda = (char *)malloc(400);
        char *parametru = (char *)malloc(400);
        int lgComanda, lgParametru;

        close(p[1]);

        while (1)
        {
            fflush(stdout);
            memset(comanda, 0, 400);
            memset(parametru, 0, 400);
            read(p[0], &lgComanda, sizeof(int));
            read(p[0], comanda, lgComanda);

             if (!strcmp(comanda, "quit"))
            {
                return 0;
            }

            if (comanda[0]=='m' && comanda[3]=='i' && comanda[5]=='d')
            {
                printf("am gasit myfind");
                printf("%s\n",parametru);
                myfind("temacn.c","/home/cami");
            }
                
            else 
                if (comanda[0]=='m' && comanda[3]=='t' && comanda[5]=='t')
                {
                    mystat(parametru);
                }               
        }
    }
    if (pid IS_PARENT)
    {
        close(socketCami[0]);
        fflush(stdout);
        eLogat = 0;
        char *userIntrodus = (char *)malloc(400);
        char *mesaj = (char *)malloc(400);
        int lgMesaj;
        int lgUserIN;

        while (!eLogat)
        {
            printf("Login: username: \n");
            memset(userIntrodus, 0, 400);
            memset(mesaj, 0, 400);
            scanf("%s", userIntrodus);
            lgUserIN = strlen(userIntrodus);
            write(socketCami[1], &lgUserIN, sizeof(int));
            write(socketCami[1], userIntrodus, lgUserIN);
            read(socketCami[1], &lgMesaj, sizeof(int));
            read(socketCami[1], mesaj, lgMesaj);
            printf("mesaj : %s\n", mesaj);

            if (strcmp(mesaj, MSJ_LOGAT) == 0)
            {
                eLogat = 1;
                printf("Te-ai logat cu succes\n");
            }
            else
            {
                printf(MSJ_NELOGAT);
            }
        }
          
        char *comanda = (char *)malloc(400);
        char *parametru = (char *)malloc(400);
        int lgComanda, lgParametru;
        fflush(stdout);
        close(p[0]);

        while (1)
        {
            memset(comanda, 0, 400);
            memset(parametru, 0, 400);
            scanf("%s", comanda);
            lgComanda = strlen(comanda);
        
           if (!strcmp(comanda, "quit"))
            {
                write(p[1], &lgComanda, sizeof(int));
                write(p[1], comanda, lgComanda);
                exit(0);
            }

          if(!strcmp(comanda, "myfind") || !(strcmp(comanda, "mystat")))
            {
                scanf("%s",parametru);
                fflush(stdout);
                strcat(comanda, parametru);
                lgComanda = strlen(comanda);
                write(p[1], &lgComanda, sizeof(int));
                write(p[1], comanda, lgComanda);
            }
        }   
    }

    return 0;
}

char * create_path(drum_vechi,drum_nou)
{ 
    char drum[256]="";
    strcat(drum,drum_vechi);
    strcat(drum,"/");
    strcat(drum,drum_nou);
    return drum;
}

void myfind(char * find_it,const char * start_it)
{
    DIR * file;
    struct dirent * dfile;
    file = opendir(start_it);
    if( file == NULL)
    {
        perror("Eroare la deschidere fisier\n");
        return;
        
    }

    while((dfile = readdir(file)))
    {
        printf("%s\n",dfile->d_name);
        if (strcmp(dfile->d_name, ".") && strcmp(dfile->d_name, ".."))
        {
            fflush(stdout);
            char path[1000]="";
            strcpy(path,create_path(find_it,dfile->d_name));
            if (path == DT_DIR)
                myfind(find_it, path);
            if(!strcmp(dfile->d_name,find_it)){
                printf("%s\n",path);
                mystat(path);
                return;
            }
        }
    }
}

void mystat(char* numeFisier)
{
    struct stat camistat;
    char permissions[9]="---------";

    fflush(stdout);

    printf("the permissions of the file: ");
    if((camistat.st_mode & S_IRUSR)) permissions[0]='r';
    if((camistat.st_mode & S_IWUSR)) permissions[1]='w';
    if((camistat.st_mode & S_IXUSR)) permissions[2]='x';
    if((camistat.st_mode & S_IRGRP)) permissions[3]='r';
    if((camistat.st_mode & S_IWGRP)) permissions[4]='w';
    if((camistat.st_mode & S_IXGRP)) permissions[5]='x';
    if((camistat.st_mode & S_IROTH)) permissions[6]='r';
    if((camistat.st_mode & S_IWOTH)) permissions[7]='w';
    if((camistat.st_mode & S_IXOTH)) permissions[8]='x';

     printf("%s\n",permissions);

   printf("File Size :%lld",(long long)camistat.st_size);
  
}