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

    if(createDir(disk, "raiz/home") == 1){
        printf("Directory created successfully!\n");
    } else {
        printf("Error while trying to create directory!\n");
    }

    if(createFile(disk, "home/teste.txt") == 1){
        printf("File created successfully!\n");
    } else {
        printf("Error while trying to create file!\n");
    }
       
}

void bootLoader(char *disk){
    FILE *fp;
    fp = fopen(disk, "rb+");

    metadataInit = "0,49;50,99;100,2099;2100,2599;2600,2649;";

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
        fseek(fp, BEGIN_INODE+INODE_BLOCK1, SEEK_SET);
        fputs("00000000",fp);
        fclose(fp);
    } else {
        printf("Error while trying to open disk!\n");
    }
}

int createFile(char *disk, char *path){

    int pos;
    char *inode_parent;
    int inode_address;
    char *inode_id;
    int tmp;
    char *file_name;
    char *ptr = malloc(sizeof((char) * path));

    pos = positionMap(disk, BEGIN_INODEMAP, END_INODEMAP);
    printf("pos: %d\n", pos);

    inode_parent = checkDir(disk, path);

    if(inode_parent == "00"){
        printf("This directory does not exist!\n");

        return 0;
    } else {

        printf("Found directory\n");

        ptr = strdup(path);
        file_name = strsep(&ptr, "/");
        file_name = strsep(&ptr, "/");
            printf("file_name: %s\n", file_name);

        fprintf(stderr, "inode_parent_id: %s\n", inode_parent);

        inode_address = checkInodeMap(disk);
        fprintf(stderr, "inode_adress: %d\n", inode_address);
        
        inode_id = getInodeId(inode_address);
        fprintf(stderr, "inode_id: %s\n", inode_id);

        writeFile(file_name, inode_address, inode_parent, inode_id);

        return 1;
    }

}

int createDir(char *disk, char *path){

    int pos;
    char *inode_parent;
    int inode_address;
    char *inode_id;
    int tmp;
    char *dir_name;
    char *ptr = malloc(sizeof((char) * path));

    pos = positionMap(disk, BEGIN_INODEMAP, END_INODEMAP);
    printf("pos: %d\n", pos);

    inode_parent = checkDir(disk, path);

    if(inode_parent == "00"){
        printf("This directory does not exist!\n");

        return 0;
    } else {

        printf("Found directory\n");

        ptr = strdup(path);
        dir_name = strsep(&ptr, "/");
        dir_name = strsep(&ptr, "/");
            printf("dir_name: %s\n", dir_name);

        fprintf(stderr, "inode_parent_id: %s\n", inode_parent);

        inode_address = checkInodeMap(disk);
        fprintf(stderr, "inode_adress: %d\n", inode_address);
        
        inode_id = getInodeId(inode_address);
        fprintf(stderr, "inode_id: %s\n", inode_id);

        writeDir(dir_name, inode_address, inode_parent, inode_id);

        return 1;
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
        printf("Root directory\n");
        return "01";
    } else {        
        FILE *fp;
        fp = fopen(disk, "rb+");

        ptr = strdup(path);
        if(token = strsep(&ptr, delimiters)){
            dir = token;
            
            for(int i=BEGIN_INODE; i<=END_INODE; i+=INODE_BYTE){
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

int checkInodeMap(char *disk){

    FILE *fp;
    fp = fopen(disk, "rb+");

    int inode_address;
    char *tmp = malloc(sizeof(char) * INODE_ID_SIZE);

    for(int i=BEGIN_INODEMAP; i<=END_INODEMAP; i++){
        fseek(fp, i, SEEK_SET);        
        fgets(tmp, 2, fp);
        if(!strcmp(tmp, "0")){
            fseek(fp, i, SEEK_SET);
            fputs("1", fp);
            fclose(fp);
            inode_address = ((i - BEGIN_INODEMAP) * INODE_BYTE) + BEGIN_INODE;
            return inode_address;
        }
    }

    return 1;

}

char *getInodeId(int inode_address){

    int tmp;

    char *inode_id = malloc(sizeof(char) * INODE_ID_SIZE);

    inode_address = (inode_address - BEGIN_INODE) / INODE_BYTE + 1;

    sprintf(inode_id, "%02d", inode_address);

    return inode_id;

}

void writeDir(char *dir_name, int inode_address, char *inode_parent,  char *inode_id){

    time_t rawtime;
    struct tm * timeinfo;
    char str[60];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(str, sizeof(str) , "%d%m%Y", timeinfo);

        writeInodeParentDir(inode_parent, inode_id);

        FILE *fp;
        fp = fopen(disk, "rb+");

        fseek(fp, inode_address, SEEK_SET);
        fputs(inode_parent, fp);                            // inode_parent
        fseek(fp, inode_address + INODE_ID, SEEK_SET);
        fputs(inode_id, fp);                                // inode_id
        fseek(fp, inode_address + INODE_NAME, SEEK_SET);
        fputs(dir_name, fp);                                  // inode_name
        fseek(fp, inode_address + INODE_DATE, SEEK_SET);
        fputs(str, fp);                                     // inode_date
        fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
        fputs("0000",fp);
        fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
        fputs("0000",fp);

    fclose(fp);
}

void writeFile(char *file_name, int inode_address, char *inode_parent,  char *inode_id){

    time_t rawtime;
    struct tm * timeinfo;
    char str[60];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(str, sizeof(str) , "%d%m%Y", timeinfo);

        writeInodeParentFile(inode_parent, inode_id);

        FILE *fp;
        fp = fopen(disk, "rb+");

        fseek(fp, inode_address, SEEK_SET);
        fputs(inode_parent, fp);                            // inode_parent
        fseek(fp, inode_address + INODE_ID, SEEK_SET);
        fputs(inode_id, fp);                                // inode_id
        fseek(fp, inode_address + INODE_NAME, SEEK_SET);
        fputs(file_name, fp);                               // inode_name
        fseek(fp, inode_address + INODE_DATE, SEEK_SET);
        fputs(str, fp);                                     // inode_date
        fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
        fputs("0000",fp);
        fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
        fputs("0000",fp);    

    fclose(fp);
}

void writeInodeParentDir(char *inode_parent, char *inode_son){

    FILE *fp;
    fp = fopen(disk, "rb+");

    strcat(inode_son, "00");

    int inode_address;
    inode_address = atoi(inode_parent);
    inode_address = ((inode_address - 1) * INODE_BYTE) + BEGIN_INODE;    
    char *block = malloc(sizeof(char) * INODE_BLOCK_SIZE);

    fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
    fgets(block, INODE_BLOCK_SIZE+1, fp);
    strtok(block, "\n");
        if(!strcmp(block, "0000")){
            fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
            fputs(inode_son, fp);
            printf("fputs %s:%d\n",inode_son, inode_address + INODE_BLOCK1);
            fclose(fp);
        } else {
            fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
            fgets(block, INODE_BLOCK_SIZE+1, fp);
            strtok(block, "\n");
            if(!strcmp(block, "0000")){
                fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
                fputs(inode_son, fp);
                printf("fputs %s:%d\n",inode_son, inode_address + INODE_BLOCK2);
                fclose(fp);
            }
        }
}

void writeInodeParentFile(char *inode_parent, char *inode_son){

    FILE *fp;
    fp = fopen(disk, "rb+");

    strcat(inode_son, "11");

    int inode_address;
    inode_address = atoi(inode_parent);
    inode_address = ((inode_address - 1) * INODE_BYTE) + BEGIN_INODE;    
    char *block = malloc(sizeof(char) * INODE_BLOCK_SIZE);

    fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
    fgets(block, INODE_BLOCK_SIZE+1, fp);
    strtok(block, "\n");
        if(!strcmp(block, "0000")){
            fseek(fp, inode_address + INODE_BLOCK1, SEEK_SET);
            fputs(inode_son, fp);
            printf("fputs %s:%d\n",inode_son, inode_address + INODE_BLOCK1);
            fclose(fp);
        } else {
            fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
            fgets(block, INODE_BLOCK_SIZE+1, fp);
            strtok(block, "\n");
            if(!strcmp(block, "0000")){
                fseek(fp, inode_address + INODE_BLOCK2, SEEK_SET);
                fputs(inode_son, fp);
                printf("fputs %s:%d\n",inode_son, inode_address + INODE_BLOCK2);
                fclose(fp);
            }
        }
}