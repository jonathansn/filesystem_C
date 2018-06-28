#include "filesystem.h"

/*
----------------------------------------------
INFORMATIONS
----------------------------------------------

----------------------------------------------
    files_or_directory
    max: 50
        representation:
            file:                   arquivo.txt
            directory:              10
        name_size:                  16
        extension_size:             4
        max_blocks:                 2
        max_size:                   2 * 10B
----------------------------------------------
    metadata
        size:                       50B
----------------------------------------------
    inodes
    max: 50
        ex:                         id_inode_pai,id_inode,name,date,block
                                    2B + 2B + 20B + 8B + 8B = 40B
        representation:             1 to 50
        size:                       40B * 50 = 2KB
----------------------------------------------
    free space
    max: 50
        size:                       50B
----------------------------------------------
    blocks
    max: 50
        representation:             1 to 50
        name_size:                  3
        size:                       50 * 10B = 500B   
----------------------------------------------
*/

int main(int argc, char *argv[]){

    if(argc == 3){
        disk = argv[2];
        size = argv[1];
        char *cmd;
        strcpy(cmd, "truncate -s ");
        strcat(cmd, argv[1]);
        strcat(cmd, " ");
        strcat(cmd, argv[2]);
        system(cmd);
        printf("Disk %s created successfully!\n", argv[2]);
   }
   else if(argc == 0){
        disk = DISK_NAME;
        size = DISK_SIZE;
        char *cmd;
        strcpy(cmd, "truncate -s ");
        strcat(cmd, DISK_SIZE);
        strcat(cmd, " ");
        strcat(cmd, DISK_NAME);
        system(cmd);
        printf("Disk %s created successfully!\n", argv[2]);
   } else{
      printf("./filesystem [disk size] [disk name]\n");
   }

    bootLoader(disk);
    createDir(disk, "home/user");
       
}

void bootLoader(char *disk){
    FILE *fp;
    fp = fopen(disk, "rb+");

    metadataInit = "0,49;50,99;100,2099;2100,2599;";

    if(fp != NULL){
        fseek(fp, BEGIN_METADATA, SEEK_SET);
        fputs(metadataInit,fp);

        fseek(fp, BEGIN_FREE_SPACE, SEEK_SET);        
        for(int i=BEGIN_FREE_SPACE; i<=END_FREE_SPACE; i++)
            fputs("0",fp);

        fseek(fp, BEGIN_INODEMAP, SEEK_SET);
        fputs("1",fp);       
        for(int i=BEGIN_INODEMAP+1; i<=END_INODEMAP; i++)
            fputs("0",fp);

        fseek(fp, BEGIN_INODE, SEEK_SET);
        fputs("0001raiz",fp);
        fseek(fp, BEGIN_INODE+INODE_DATE, SEEK_SET);
        fputs("10102010",fp);
        fseek(fp, BEGIN_INODE+INODE_BLOCK, SEEK_SET);
        fputs("00000000",fp);

        fseek(fp, BEGIN_INODE+INODE_BYTE, SEEK_SET);
        fputs("0102home",fp);
        fseek(fp, BEGIN_INODE+INODE_BYTE+INODE_DATE, SEEK_SET);
        fputs("10112010",fp);
        fseek(fp, BEGIN_INODE+INODE_BYTE+INODE_BLOCK, SEEK_SET);
        fputs("00000000",fp);
        fclose(fp);
    } else {
        printf("Error while trying to open disk!\n");
    }
}

int positionMap(char *disk , int posBegin, int posEnd){
    FILE *fp;
    fp = fopen(disk, "rb+");

    int mapLength = posEnd - posBegin + 1;
    char posStr[mapLength];

    if(fp != NULL){

        fseek(fp, posBegin, SEEK_SET);

        if(fgets(posStr, mapLength, fp) != NULL){
            for(int i=1; i<=mapLength; i++){
                if(posStr[i] == '0'){
                    fclose(fp);
                    return i;
                }                    
            }
            printf("Disk not have space!\n");
            fclose(fp);
            return -1;

        } else {
            printf("fgets error!\n");
            fclose(fp);
            return -1;
        }

    } else {
        printf("Error while trying to access disk!\n");
        fclose(fp);
        return -1;
    }

}

void createDir(char *disk, char *path){

    int pos;
    char *inode_id;

    pos = positionMap(disk, BEGIN_INODEMAP, END_INODEMAP);

    //teste = BEGIN_INODE + (pos * INODE_BYTE);
    //sprintf(inode_id, "%02d", pos + 1);

    inode_id = checkDir(disk, path);

    if(inode_id == "00"){
        printf("This directory does not exist!\n");
    } else{
        printf("Found directory\n");
        fprintf(stderr, "dir_inode_id: %s\n", inode_id);
    }

    // if(fp != NULL){
    //     // fseek(fp, teste, SEEK_SET);
    //     // fputs(inode_id, fp);                 // inode_id 

    //     // fclose(fp);
    // } else {
    //     printf("Error while trying to create file!\n");
    // }
}

char *checkDir(char *disk, char *path){

    char *token;
    char delimiters[] = "/";
    char *ptr = malloc(sizeof((char) * path));

    int cont=0;
    char *dir;
    char *dir_name;
    char *dir_inode_id = malloc(sizeof(char) * INODE_ID_SIZE);
    char *teste;

    ptr = strdup(path);
    while((token = strsep(&ptr, delimiters)) != NULL){
        cont++;
    }

    if(cont == 1){
        return "01";
    } else {        
        FILE *fp;
        fp = fopen(disk, "rb+");

        ptr = strdup(path);
        if(token = strsep(&ptr, delimiters)){
            dir = token;            
            
            for(int i=BEGIN_INODE; i<=END_INODE; i+=40){
                fseek(fp, i + INODE_NAME, SEEK_SET);
                fgets(dir_name, INODE_NAME_SIZE, fp);

                if(!strcmp(dir_name, dir)){                    
                    fseek(fp, i + INODE_ID, SEEK_SET);
                    fgets(dir_inode_id, INODE_ID_SIZE+1, fp);

                    strtok(dir_inode_id, "\n");

                    fclose(fp);
                    return dir_inode_id;
                }
            }

            fclose(fp);
            return "00";
        }
    }

    free(ptr);
}