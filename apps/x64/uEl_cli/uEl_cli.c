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
    printf(" %s [Options]\n\n", argv[0]);
    
    printf("Options:\n");
    printf(" -e\t\t\t Encrypt file\n");
    printf(" -d\t\t\t Decrypt file\n");
    printf("\n -i <input file path>\t Input file path\n");
    printf(" -o <output file path>\t Output file path\n");
    printf("\n -k <key path>\t\t Current path of public/private key\n");
    printf("\t\t\t Specified key will replace default key in\n");
    printf("\t\t\t current encryption/decryption process\n");
    printf("\n -s <private key path>\t Set default private key path\n");
    printf("\t\t\t Specified key will be used in every decryption process\n");
    printf("\n -v <public key path>\t Set default public key path\n");
    printf("\t\t\t Specified key will be used in every encryption process\n");
    
    printf("\n -h\t\t\t Print this help and exit\n");
}

int encryptFile(char *inFilePath, char *outFilePath, char *pubKeyPath){
    uEl_PubKey publicKey;
    FILE *pubkeyFile, *inFile, *outFile, *dpkFile;
    uint64_t fileSize;
    uint16_t i;
    uint8_t *msg;
    uEl_msglen_t length, len;
    char *dpkFileDir, c;

    if(pubKeyPath == NULL){
        dpkFileDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/default_pubkey.txt") + 1);
        strcpy(dpkFileDir, getenv("HOME"));
        strcat(dpkFileDir, "/.uEliece/default_pubkey.txt");

        if((dpkFile = fopen(dpkFileDir, "r")) == NULL){
            fprintf(stderr, "ERROR: default_pubkey.txt file not found\n");
            fprintf(stderr, "Please specify path to public key (option '-k') or set default public key path (option '-v')\n");
            exit(EXIT_FAILURE);
        }
        free(dpkFileDir);
    
        pubKeyPath = (char*) malloc(sizeof(char));
        i = 0;
        while((c = fgetc(dpkFile)) != EOF){
            pubKeyPath[i] = c;
            i++;
            pubKeyPath = realloc(pubKeyPath, (i + 1) * sizeof(char));
        }
        pubKeyPath[i] = '\0';
        fclose(dpkFile);
    }

    if((pubkeyFile = fopen(pubKeyPath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Can't open public key file '%s'\n", pubKeyPath);
        exit(EXIT_FAILURE);
    }

    if(fread(publicKey, sizeof(uint8_t), (UEL_MDPC_M / 8) + 1, pubkeyFile) != (UEL_MDPC_M / 8) + 1){
        fprintf(stderr, "ERROR: Public key file '%s' is invalid\n", pubKeyPath);
        exit(EXIT_FAILURE); 
    }
    fclose(pubkeyFile);
    
    if((inFile = fopen(inFilePath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Can't open input file '%s'\n", inFilePath);
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
        fprintf(stderr, "ERROR: Can't open output file '%s' for write\n", outFilePath);
        exit(EXIT_FAILURE);
    }

    fwrite(msg, length, 1, outFile);
    fclose(outFile);
    
    free(msg);

    return 0;
}

int decryptFile(char *inFilePath, char *outFilePath, char *privKeyPath){
    uEl_PrivKey privateKey;
    FILE *privkeyFile, *inFile, *outFile, *dpkFile;
    uint64_t fileSize;
    uint16_t i;
    uint8_t *msg;
    uEl_msglen_t length, len;
    char *dpkFileDir, c;

    if(privKeyPath == NULL){
        dpkFileDir = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/default_privkey.txt") + 1);
        strcpy(dpkFileDir, getenv("HOME"));
        strcat(dpkFileDir, "/.uEliece/default_privkey.txt");

        if((dpkFile = fopen(dpkFileDir, "r")) == NULL){
            fprintf(stderr, "ERROR: default_privkey.txt file not found\n");
            fprintf(stderr, "Please specify path to private key (option '-k') or set default private key path (option '-s')\n");
            exit(EXIT_FAILURE);
        }
        free(dpkFileDir);
    
        privKeyPath = (char*) malloc(sizeof(char));
        i = 0;
        while((c = fgetc(dpkFile)) != EOF){
            privKeyPath[i] = c;
            i++;
            privKeyPath = realloc(privKeyPath, (i + 1) * sizeof(char));
        }
        privKeyPath[i] = '\0';
        fclose(dpkFile);
    }

    if((privkeyFile = fopen(privKeyPath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Can't open private key file '%s'\n", privKeyPath);
        exit(EXIT_FAILURE);
    }
    
    if(fread(privateKey[0], sizeof(uint16_t), (UEL_MDPC_W / 2), privkeyFile) != (UEL_MDPC_W / 2)){
        fprintf(stderr, "ERROR: Private key file '%s' is ivalid\n", privKeyPath);
        exit(EXIT_FAILURE);
    }
    if(fread(privateKey[1], sizeof(uint16_t), (UEL_MDPC_W / 2), privkeyFile) != (UEL_MDPC_W / 2)){
        fprintf(stderr, "ERROR: Private key file '%s' is ivalid\n", privKeyPath);
        exit(EXIT_FAILURE);
    }
    fclose(privkeyFile);
    
    if((inFile = fopen(inFilePath, "rb")) == NULL){
        fprintf(stderr, "ERROR: Can't open input file '%s'\n", inFilePath);
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
        fprintf(stderr, "ERROR: Can't open output file '%s' for write\n", outFilePath);
        exit(EXIT_FAILURE);
    }

    fwrite(msg, length, 1, outFile);
    fclose(outFile);
    
    free(msg);

    return 0;
}

int setDefaultPubKey(char *pubKeyPath){
    FILE *dpkFile;
    char *dpkPath;

    dpkPath = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/default_pubkey.txt") + 1);
    strcpy(dpkPath, getenv("HOME"));
    strcat(dpkPath, "/.uEliece/default_pubkey.txt");

    if((dpkFile = fopen(dpkPath, "w")) == NULL){
        fprintf(stderr, "ERROR: Open default_pubkey.txt file\n");
        exit(EXIT_FAILURE);
    }
    free(dpkPath);
    
    if((fputs(pubKeyPath, dpkFile)) == EOF){
        fprintf(stderr, "ERROR: Write to default_pubkey.txt failed\n");
        exit(EXIT_FAILURE);
    }

    fclose(dpkFile);

    return 0;
}

int setDefaultPrivKey(char *privKeyPath){
    FILE *dpkFile;
    char *dpkPath;

    dpkPath = (char*) malloc(strlen(getenv("HOME")) + strlen("/.uEliece/default_privkey.txt") + 1);
    strcpy(dpkPath, getenv("HOME"));
    strcat(dpkPath, "/.uEliece/default_privkey.txt");

    if((dpkFile = fopen(dpkPath, "w")) == NULL){
        fprintf(stderr, "ERROR: Open default_privkey.txt file\n");
        exit(EXIT_FAILURE);
    }
    free(dpkPath);
    
    if((fputs(privKeyPath, dpkFile)) == EOF){
        fprintf(stderr, "ERROR: Write to default_privkey.txt failed\n");
        exit(EXIT_FAILURE);
    }

    fclose(dpkFile);

    return 0;
}

int parseArgs(int argc, char *argv[]){
    int opt;
    int e = -1;
    int d = -1;
    int i = -1;
    int o = -1;
    int k = -1;
    int s = -1;
    int v = -1;
   
    char *iArg, *oArg, *kArg, *sArg, *vArg;

    while((opt = getopt(argc, argv, "hedi:o:k:s:v:")) != -1){
        switch(opt){
        case 'h':
            printHelp(argv);
            exit(EXIT_SUCCESS);

        case 'e':
            e = 1;
            break;

        case 'd':
            d = 1;
            break;

        case 'i':
            i = atoi(optarg);
            iArg = optarg;
            break;
        
        case 'o':
            o = atoi(optarg);
            oArg = optarg;
            break;

        case 'k':
            k = atoi(optarg);
            kArg = optarg;
            break;

        case 's':
            s = atoi(optarg);
            sArg = optarg;
            break;

        case 'v':
            v = atoi(optarg);
            vArg = optarg;
            break;

        default:
            printHelp(argv);
            exit(EXIT_FAILURE);
       }
    }

    if(s != -1){
        setDefaultPrivKey(sArg);
    }

    if(v != -1){
        setDefaultPubKey(vArg);
    }

    if(e != -1){
        if(i == -1){
            fprintf(stderr, "Please specify input file\n");
            exit(EXIT_FAILURE);
        }

        if(o == -1){
            fprintf(stderr, "Please specify output file\n");
            exit(EXIT_FAILURE);
        }

        if(d != -1){
            fprintf(stderr, "Please specify only one of options (-e | -d)\n");
            exit(EXIT_FAILURE);
        }

        if(k != -1){
            encryptFile(iArg, oArg, kArg);
        }
        else{
            encryptFile(iArg, oArg, NULL);
        }
    }

    if(d != -1){
        if(i == -1){
            fprintf(stderr, "Please specify input file\n");
            exit(EXIT_FAILURE);
        }

        if(o == -1){
            fprintf(stderr, "Please specify output file\n");
            exit(EXIT_FAILURE);
        }

        if(e != -1){
            fprintf(stderr, "Please specify only one of options (-e | -d)\n");
            exit(EXIT_FAILURE);
        }

        if(k != -1){
            decryptFile(iArg, oArg, kArg);
        }
        else{
            decryptFile(iArg, oArg, NULL);            
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
