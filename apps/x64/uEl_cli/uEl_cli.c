#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../../../libuEliece/src/uEliece.h"


void printHelp(char *argv[]){
    printf("Usage:\n");
    printf(" %s <-e | -d> <input file> <output file>\n", argv[0]);
    printf(" %s [Options]\n\n", argv[0]);
    
    printf("Options:\n");
    printf(" -e\t Encrypt input file and save as encrypted output file\n");
    printf(" -d\t Decrypt input file and save as decrypted output file\n");
    printf(" -s\t Set default private key path\n");
    printf(" -v\t Set default public key path\n");
    
    printf("\n -h\t Print this help and exit\n");
}

int encryptFile(char *inFilePath, char *outFilePath){
    uEl_PubKey publicKey;
    FILE *pubkeyFile, *inFile, *outFile, *cpkFile;
    uint64_t fileSize;
    uint16_t i;
    uint8_t *msg;
    uEl_msglen_t length, len;
    char *pubKeyPath, *cpkFileDir, c;
    /*char *pubKeyDir;
    
    pubkeyDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/public_keys/uEl_pub.key") + 1);
    strcpy(pubkeyDir, getenv("HOME"));
    strcat(pubkeyDir, "/.uEliece/public_keys/uEl_pub.key");
    
    if((pubkeyFile = fopen(pubkeyDir, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open public key file\n");
        exit(EXIT_FAILURE);
    }
    free(pubkeyDir);*/

    cpkFileDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/current_pubkey.txt") + 1);
    strcpy(cpkFileDir, getenv("HOME"));
    strcat(cpkFileDir, "/.uEliece/current_pubkey.txt");

    if((cpkFile = fopen(cpkFileDir, "r")) == NULL){
        fprintf(stderr, "ERROR: current_pubkey.txt file not found\nPlease set default public key path\n");
        exit(EXIT_FAILURE);
    }
    free(cpkFileDir);
    
    pubKeyPath = (char*) malloc(sizeof(char));
    i = 0;
    while((c = fgetc(cpkFile)) != EOF){
        pubKeyPath[i] = c;
        i++;
        pubKeyPath = realloc(pubKeyPath, (i + 1) * sizeof(char));
    }
    fclose(cpkFile);

    if((pubkeyFile = fopen(pubKeyPath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open public key file\n");
        exit(EXIT_FAILURE);
    }
    free(pubKeyPath);
    

    if(fread(publicKey, sizeof(uint8_t), (UEL_MDPC_M / 8) + 1, pubkeyFile) != (UEL_MDPC_M / 8) + 1){
        exit(EXIT_FAILURE); //TODO Error Message 
    }
    fclose(pubkeyFile);
    
    if((inFile = fopen(inFilePath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open input file\n");
        exit(EXIT_FAILURE);
    }
    
    fseek(inFile, 0, SEEK_END);
    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    msg = malloc(fileSize);

    fread(msg, fileSize, 1, inFile);
    fclose(inFile);
    
    len = fileSize * 8;

    fprintf(stderr, "Encrypting...\n");
    uEliece_encrypt(&msg, len, &length, publicKey, uEl_default_rng());

    if((outFile = fopen(outFilePath, "wb")) == NULL){
        fprintf(stderr, "ERROR: Open output file for write\n");
        exit(EXIT_FAILURE);
    }

    fwrite(msg, length, 1, outFile);
    fclose(outFile);
    
    free(msg);

    return 0;
}

int decryptFile(char *inFilePath, char *outFilePath){
    uEl_PrivKey privateKey;
    FILE *privkeyFile, *inFile, *outFile, *cpkFile;
    uint64_t fileSize;
    uint16_t i;
    uint8_t *msg;
    uEl_msglen_t length, len;
    char *privKeyPath, *cpkFileDir, c;
    /*char *privkeyDir;
    
    privkeyDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/private_keys/uEl_priv.key") + 1);
    strcpy(privkeyDir, getenv("HOME"));
    strcat(privkeyDir, "/.uEliece/private_keys/uEl_priv.key");
    
    if((privkeyFile = fopen(privkeyDir, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open private key file\n");
        exit(EXIT_FAILURE);
    }
    free(privkeyDir);*/

    cpkFileDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/current_privkey.txt") + 1);
    strcpy(cpkFileDir, getenv("HOME"));
    strcat(cpkFileDir, "/.uEliece/current_privkey.txt");

    if((cpkFile = fopen(cpkFileDir, "r")) == NULL){
        fprintf(stderr, "ERROR: current_privkey.txt file not found\nPlease set default private key path\n");
        exit(EXIT_FAILURE);
    }
    free(cpkFileDir);
    
    privKeyPath = (char*) malloc(sizeof(char));
    i = 0;
    while((c = fgetc(cpkFile)) != EOF){
        privKeyPath[i] = c;
        i++;
        privKeyPath = realloc(privKeyPath, (i + 1) * sizeof(char));
    }
    fclose(cpkFile);

    if((privkeyFile = fopen(privKeyPath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open private key file\n");
        exit(EXIT_FAILURE);
    }
    free(privKeyPath);
    
    if(fread(privateKey[0], sizeof(uint16_t), (UEL_MDPC_W / 2), privkeyFile) != (UEL_MDPC_W / 2)){
        exit(EXIT_FAILURE); //TODO Error Message 
    }
    if(fread(privateKey[1], sizeof(uint16_t), (UEL_MDPC_W / 2), privkeyFile) != (UEL_MDPC_W / 2)){
        exit(EXIT_FAILURE); //TODO Error Message 
    }
    fclose(privkeyFile);
    
    if((inFile = fopen(inFilePath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Open input file\n");
        exit(EXIT_FAILURE);
    }
    
    fseek(inFile, 0, SEEK_END);
    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    msg = malloc(fileSize);

    fread(msg, fileSize, 1, inFile);
    fclose(inFile);
    
    len = fileSize * 8;

    fprintf(stderr, "Decrypting...\n");
    if(uEliece_decrypt(&msg, len, &length, privateKey) & UEL_BAD_INTEGRITY){
        fprintf(stderr, "Bad integrity of message\n");
    }

    if((outFile = fopen(outFilePath, "wb")) == NULL){
        fprintf(stderr, "ERROR: Open output file for write\n");
        exit(EXIT_FAILURE);
    }

    fwrite(msg, length, 1, outFile);
    fclose(outFile);
    
    free(msg);

    return 0;
}

int setCurrentPubKey(char *pubKeyPath){
    FILE *cpkFile;
    char *cpkPath;

    cpkPath = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/current_pubkey.txt") + 1);
    strcpy(cpkPath, getenv("HOME"));
    strcat(cpkPath, "/.uEliece/current_pubkey.txt");

    if((cpkFile = fopen(cpkPath, "w")) == NULL){
        fprintf(stderr, "ERROR: Open current_pubkey.txt file\n");
        exit(EXIT_FAILURE);
    }
    free(cpkPath);
    
    if((fputs(pubKeyPath, cpkFile)) == EOF){
        fprintf(stderr, "ERROR: Write to current_pubkey.txt failed\n");
        exit(EXIT_FAILURE);
    }

    fclose(cpkFile);

    return 0;
}

int setCurrentPrivKey(char *privKeyPath){
    FILE *cpkFile;
    char *cpkPath;

    cpkPath = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/current_privkey.txt") + 1);
    strcpy(cpkPath, getenv("HOME"));
    strcat(cpkPath, "/.uEliece/current_privkey.txt");

    if((cpkFile = fopen(cpkPath, "w")) == NULL){
        fprintf(stderr, "ERROR: Open current_privkey.txt file\n");
        exit(EXIT_FAILURE);
    }
    free(cpkPath);
    
    if((fputs(privKeyPath, cpkFile)) == EOF){
        fprintf(stderr, "ERROR: Write to current_privkey.txt failed\n");
        exit(EXIT_FAILURE);
    }

    fclose(cpkFile);

    return 0;
}

int parseArgs(int argc, char *argv[]){
    int opt;

    while((opt = getopt(argc, argv, "he:d:s:v:")) != -1){
        switch(opt){
        case 'h':
            printHelp(argv);
            exit(EXIT_SUCCESS);

        case 'e':
            if(argc != 4){
                if(!argv[2]){
                    fprintf(stderr, "Please specify input file\n");
                    exit(EXIT_FAILURE);
                }
                else if(!argv[3]){
                    fprintf(stderr, "Please specify output file\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    fprintf(stderr, "Too much arguments\n");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                encryptFile(argv[2], argv[3]);
            }
            break;

        case 'd':
            if(argc != 4){
                if(!argv[2]){
                    fprintf(stderr, "Please specify input file\n");
                    exit(EXIT_FAILURE);
                }
                else if(!argv[3]){
                    fprintf(stderr, "Please specify output file\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    fprintf(stderr, "Too much arguments\n");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                decryptFile(argv[2], argv[3]);
            }
            break;

        case 's':
            if(argc > 3){
                fprintf(stderr, "Too much arguments\n");
                exit(EXIT_FAILURE);
            }
            else{
                setCurrentPrivKey(argv[2]);
            }
            break;

        case 'v':
            if(argc > 3){
                fprintf(stderr, "Too much arguments\n");
                exit(EXIT_FAILURE);
            }
            else{
                setCurrentPubKey(argv[2]);
            }
            break;

        default:
            printHelp(argv);
            exit(EXIT_FAILURE);
       }
    }

    return 0;
}

int inituElDir(){
    struct stat st = {0};
    char *homeDir, *uElDir, *workDir;

    if((homeDir = getenv("HOME")) == NULL){
        perror("ERROR: Couldn't get HOME directory path\n");
        return -1;
    }

    //Creating uEliece directory
    uElDir = (char*) malloc(strlen(homeDir) + strlen("/.uEliece") + 1);
    strcpy(uElDir, homeDir);
    strcat(uElDir, "/.uEliece");

    if(stat(uElDir, &st) == -1){ //Create new directory if doesn't exist
        if(mkdir(uElDir, 0700) == -1){
            perror("ERROR: Creating new uEliece directory failed\n");
            return -1;
        }
    }

    //Creating public_keys directory
    workDir = (char*) malloc(strlen(uElDir) + strlen("/public_keys") + 1);
    strcpy(workDir, uElDir);
    strcat(workDir, "/public_keys");

    if(stat(workDir, &st) == -1){
        if(mkdir(workDir, 0700) == -1){
            perror("ERROR: Creating new public_keys directory failed\n");
            return -1;
        }
    }
    free(workDir);

    //Creating private_keys directory
    workDir = (char*) malloc(strlen(uElDir) + strlen("/private_keys") + 1);
    strcpy(workDir, uElDir);
    strcat(workDir, "/private_keys");

    if(stat(workDir, &st) == -1){
        if(mkdir(workDir, 0700) == -1){
            perror("ERROR: Creating new private_keys directory failed\n");
            return -1;
        }
    }
    free(workDir);
    
    free(uElDir);

    return 0;
}


int main(int argc, char *argv[]){
    inituElDir();
    parseArgs(argc, argv);

    return 0;
}
