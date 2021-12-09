/*
    Author: Lute Lillo Portero
    Date: 12/12/2021
    Client - Server Database
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"
#include <signal.h>

#define PORT_NUM 20047


typedef struct student{
    char lname[10];
    char fname[10];
    char initial;
    unsigned long SID;
    float gpa;
}student_record;

/* Handle Control + C signal termination */
void sigint_handler(int sig){
    write(STDERR_FILENO, " -> Closing server", 18);
    if(remove("database.bin") == 0){
        printf("\nCopy DB succesfully removed\n");
    
    }else{
        printf("Error deleting Copy DB\n");
    }
    exit(1);
}
/* Save function makes a copy of the "copy DB" (temporary database where the new data is being written to into the final Database */
int save(){
    
    FILE *cp_db, *final_db;
    
    int read_n, write_n;
    char buffer[101];

    cp_db = fopen("database.bin", "rb");
    if (cp_db == NULL){
        printf("Open CP Database error.\n");
        return 0;
    }

    final_db = fopen("DB.bin", "a+w");
    if (final_db == NULL){
        fclose(cp_db);
        printf("Open Final Database error.\n");
        return 0;
    }

    while (feof(cp_db) == 0){
        read_n = fread(buffer, 1, 100, cp_db);
        if (ferror(final_db) != 0){
            printf("Read final DB error.\n");
            break;
        }

        write_n = fwrite(buffer, sizeof(char), read_n, final_db);
        if (write_n != read_n){
            printf("Write error.\n");
            break;
        }
    }

    fclose(cp_db);
    fclose(final_db);

    return 1;
}

/* Function use to delete a record from the database by using a temporary copy of the database */
int delete(char *db_name, int SID_search){
    FILE *db;
    FILE *db_tmp;
    
    int deleted = 0;
    student_record srec;

    db = fopen(db_name, "rb");
    
    if((db = fopen(db_name, "rb")) == NULL){
        printf("Unable to open DB.bin\n");
        return -1;
    }
    
    db_tmp = fopen("tmp.bin", "wb");
    if((db_tmp = fopen("tmp.bin", "wb")) == NULL) {
        printf("Unable to open temp_DB\n");
        return -1;
    }
    
    /* Read from DB.bin (final DB), if SID found, don't write it back to temp_DB */
    while(fread(&srec,sizeof(student_record), 1, db) == 1) {
        if (SID_search == srec.SID) {
            printf("Record found and deleted\n");
            deleted = 1;
        } else { /* If record not found, write it to temp_DB */
            
            fwrite(&srec, sizeof(student_record), 1, db_tmp);
        }
    }
       
    if(deleted == 0){
        printf("Record not found nor deleted\n");
        fclose(db);
        fclose(db_tmp);

        remove(db_name);
        rename("tmp.bin", db_name);
        return -1;
    }
   
    /* Set files back to normal */
    fclose(db);
    fclose(db_tmp);

    remove(db_name);
    rename("tmp.bin", db_name);

    return 0;
}


/* Helper function:
 * to initialize individual student records
 * in an array of student record struct */
int init_record(student_record *srec, char *fn, char *ln, char init, unsigned long SID, float gpa){
    if (srec == NULL){
        return -1;
    }

    memmove(&srec->fname, fn, strlen(fn)+1);
    memmove(&srec->lname, ln, strlen(ln)+1);
    srec->initial = init;
    srec->SID = SID;
    srec->gpa = gpa;

    return 0;
}

void put(char *arr[]){
    FILE *database;
    student_record srec;

    if ((database = fopen("database.bin","ab+")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    /* Copy first and last name up to 10 characters */
    strncpy(srec.lname, arr[0], 8);
    srec.lname[9] = '\0';
    
    strncpy(srec.fname, arr[1], 8);
    srec.fname[9] = '\0';
    
    srec.initial = arr[2][0];
    srec.SID = atol(arr[3]);
    srec.gpa = atof(arr[4]);
    
    fwrite(&srec, sizeof(student_record), 1, database);

    fclose(database);
}

/* Gets database and sends back */
student_record* get(int n_records, int n_original_rec){
    FILE *database;
    FILE *orig;
    student_record srec;
    int n;
    int orig_n;
    int temp;
    
    student_record *arr = malloc(sizeof(student_record) * (n_records + n_original_rec));
    
    if(n_original_rec == 0){
        printf("Saved DB is still non-existent\n");
        temp =0;
    }else{
        if ((orig = fopen("DB.bin","rb")) == NULL){
            printf("Error! opening file");
            exit(1);
        }
    }
    
    /* Read content of saved Database and init records */
    if(n_original_rec != 0){
        for(orig_n = 0; orig_n < n_original_rec; ++orig_n){
            
            fread(&srec, sizeof(student_record), 1, orig);
            printf("ORIGINAL DB: %s %s %lu %c %.2f \n", srec.fname, srec.lname, srec.SID, srec.initial, srec.gpa);
            init_record(&arr[orig_n], srec.fname, srec.lname, srec.initial, srec.SID, srec.gpa);
        }
        temp = n_original_rec;
        fclose(orig);
    }
    
    /* Read the content of newly 'soon to be added' content to the DB */
    if(n_records == 0){
        printf("No new records soon to be added\n");

    }else{
        if ((database = fopen("database.bin","rb")) == NULL){
            printf("Error! opening file");
            exit(1);
        }
    
        for(n = 0; n < n_records; ++n){
        
            fread(&srec, sizeof(student_record), 1, database);
        
            printf("COPY DB: %s %s %lu %c %.2f \n", srec.fname, srec.lname, srec.SID, srec.initial, srec.gpa);
                
            /* Initialize the records to be sent back as array of structs */
            init_record(&arr[temp], srec.fname, srec.lname, srec.initial, srec.SID, srec.gpa);
            temp++;
        }
        fclose(database);
    }
    
    return arr;
}

/* Helper function to sort based on the get command */
void sort_get(student_record list[], int s, int order)
{
    int i, j;
    student_record temp;
    
    
    for (i = 0; i < s - 1; i++){
        for (j = 0; j < (s - 1-i); j++){
            switch(order){
                case 1: /* Order by first name */
                    if(strcmp(list[j].fname, list[j + 1].fname) > 0){
                        temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                    break;
                case 2: /* Order by last name */
                    if(strcmp(list[j].lname, list[j + 1].lname) > 0){
                        temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                    break;
                case 3: /* Order by SID */
                    if(list[j].SID > list[j + 1].SID){
                        temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                    break;
                case 4:/* Order by GPA */
                    if(list[j].gpa < list[j + 1].gpa){
                        temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                    break;
            }
        }
    }
}

int main(int argc, char *argv[]){
    
    int sockid;
    int new_sockid;
    int port_num;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    
    char buffer[2000];
    student_record srec;
    student_record* arr;
    char *command;
    char *get_order;
    
    int size;
    int size_orig;
    int n_records;
    int n_original_rec;
    
    struct stat st;
    int save_status;
    int orig_delete = 0;
    
    int p;
    int l;
    
    char SID_str[12];
    char gpa_buff[12];
    char* pad_record  = (char*)malloc(40*sizeof(char));
    char* message = (char*)malloc(1240*sizeof(char));
    char* space = " ";
    char delim[] = " ";
    char *put_arr[5];

    int total_records;
    int delete_status;
    char *get_SID;
    int get_N_SID;
    
    
    /* Check if file exists */
    if(access("DB.bin", F_OK ) == 0) { /* File exists -> Get bytes size */
        stat("DB.bin", &st);
        size_orig = st.st_size;
        n_original_rec = (size_orig / sizeof(srec));
    } else { /* File does NOT exist yet */
        size_orig = 0;
        n_original_rec = 0;
    }
    
    
    /* This eventually should never exist */
    if(access("database.bin", F_OK ) == 0) { /* File exists -> Get bytes size */
        stat("database.bin", &st);
        size = st.st_size;
        n_records = (size / sizeof(srec));
    } else { /* File does NOT exist yet */
        size = 0;
        n_records = 0;
    }
    
    /* Assign Port Number */
    port_num = PORT_NUM;
    
    /* Create the socket */
    sockid = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockid < 0){
        error("Error: Creation socket");
    }
    
    /* Build server address structure */
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_num);
    
    /* Bind socket server and assign address */
    if (bind(sockid, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        error("Error: Binding socket");
    }
    
    /* Open server for listening */
    if (listen(sockid, 5) < 0){
        error("Error: Listening on socket");
    }
    
    /* Communicate with the client */
    memset(buffer, 0, 2000);

    for (;;){
        
        /* Accept incoming connections */
        client_len = sizeof(client_addr);
        new_sockid = accept(sockid, (struct sockaddr *) &client_addr, &client_len);
        
        if(new_sockid < 0){
            error("Error: Accepting connection");
        }
        
        /* Communicate with the client */
        for(;;){
            int length;
            signal(SIGINT, sigint_handler);
            /* Read the length field */
            length = read_length(new_sockid);

            if(length >= sizeof(buffer)){
                error("Message exceeds the buffer size");
            }

            /* Read the message field */
            readn(new_sockid, buffer, length);
      
            if(sizeof(buffer) <= 0){
                printf("Error\n");
            }
        
            buffer[length] = '\0';
            
            /* Handle the close command for client exit */
            if(!strncmp(buffer, "close", 5)){
                printf("Client close connection\n");
            
                /* Remove temporary copy database if client closes without saving */
                if(remove("database.bin") == 0){
                    printf("Copy DB succesfully removed\n");
                    n_records = 0;
                
                }else{
                    printf("Error deleting Copy DB\n");
                }
                break;
            }
            
            /* PARSE THE BUFFER AND OPERATE */

            command = strtok(buffer, delim);
       
            /* buffer and parse variables */
      
            pad_record  = (char*)calloc(40, sizeof(char));
            message = (char*)calloc(1240, sizeof(char));
          
            
            /* CASE 1: GET*/
            if(!strcmp(command, "get")){
            
                get_order = strtok(NULL,delim);
            
                arr = get(n_records, n_original_rec);
                total_records = n_records + n_original_rec;
            
                if(!strncmp(get_order, "fname", 5)){ /* Order -> 1 */
                    sort_get(arr, total_records, 1);
    
                }else if(!strncmp(get_order, "lname", 5)){ /* Order -> 2 */
                    sort_get(arr, total_records, 2);
                
                }else if(!strncmp(get_order, "SID", 3)){ /* Order -> 3 */
                    sort_get(arr, total_records, 3);
                
                }else if(!strncmp(get_order, "gpa", 3)){ /* Order -> 4 */
                    sort_get(arr, total_records, 4);
        
                }
            
                for(p = 0; p < n_records+n_original_rec; ++p){
                
                    int t;
                    /* Pading SID */
                    strcat(pad_record, "|");
                    strcat(pad_record, space);
                    
                    sprintf(SID_str, "%lu", arr[p].SID);
                    
                    for(t = strlen(SID_str); t < 5; t++){
                        strcat(pad_record, "0");
                    }
                    strcat(pad_record, SID_str);
                    
                    strcat(pad_record, space);
                    strcat(pad_record, "|");
                    strcat(pad_record, space);
                    
                    /* Pading last name */
                    strcat(pad_record, arr[p].lname);
                    for(t = strlen(arr[p].lname); t < 10; t++){
                        strcat(pad_record, space);
                    }
                    strcat(pad_record, "|");
                    strcat(pad_record, space);
                    
                    /* Pading first name */
                    strcat(pad_record, arr[p].fname);
                    for(t = strlen(arr[p].fname); t < 10; t++){
                        strcat(pad_record, space);
                    }
                    strcat(pad_record, "|");
                    strcat(pad_record, space);
                    
                    /* Pading middle initial name */
                    strcat(pad_record, &arr[p].initial);
                    strcat(pad_record, space);
                    strcat(pad_record, "|");
                    strcat(pad_record, space);
                    
                    /* Pading gpa name */
                    sprintf(gpa_buff, "%.2f", arr[p].gpa);
                    strcat(pad_record, gpa_buff);
                    strcat(pad_record, space);
                
                    strcat(message, pad_record);
                    strcat(message, "|\n");
           
                    memset(pad_record, 0, strlen(pad_record));
                 
                }
            
                length = strlen(message);
                write_length(new_sockid, length);
                writen(new_sockid, message, length);

            }else if(!strcmp(command, "put")){ /* CASE 2: PUT */
            
                /* Tokenize words to obtain data for student record*/
                for(l = 0; l < 5; l++ ){
                    command = strtok(NULL, ",");
                    put_arr[l] = command;
                    printf("Element is: %s\n", put_arr[l]);
                }
            
                /* Send to put() function and create the new Student Record */
                put(put_arr);
                n_records = n_records + 1;
    
                message = "New Record written to database\n";
                length = strlen(message);
                write_length(new_sockid, length);
                writen(new_sockid, message, length);
            
            
            }else if(!strcmp(command, "delete")){ /* CASE 3: Delete */
        
                get_SID = strtok(NULL,delim); /* Get SID to be deleted */
                get_N_SID = atoi(get_SID);
            
                delete_status = delete("DB.bin", get_N_SID);
                if(delete_status == 0){
                
                    /* Recalculate number of records */
                    /* Check if file exists */
                    if(access("DB.bin", F_OK ) == 0) { /* File exists -> Get bytes size */
                        stat("DB.bin", &st);
                        size_orig = st.st_size;
                        n_original_rec = (size_orig / sizeof(srec));
                    } else { /* File does NOT exist yet */
                        size_orig = 0;
                        n_original_rec = 0;
                    }
                    
                    delete_status = 1;
                    orig_delete = 1;
                    
                    message = "Record deleted.\n";
                }
                
                delete_status = delete("database.bin", get_N_SID);
                
                if(delete_status == 0){
                    
                    /* Recalculate number of records */
                    /* Check if file exists */
                    if(access("database.bin", F_OK ) == 0) { /* File exists -> Get bytes size */
                        stat("database.bin", &st);
                        size = st.st_size;
                        n_records = (size / sizeof(srec));
                    } else { /* File does NOT exist yet */
                        size = 0;
                        n_records = 0;
                    }
                    
                    message = "Record deleted.\n";
        
                }else if(orig_delete == 0){
                    delete_status = 2;
                }
                
                if(delete_status == 2){
                    message = "Record doesn't exist and is not deleted\n";
                }
                
                length = strlen(message);
                write_length(new_sockid, length);
                writen(new_sockid, message, length);
            
            }else if(!strncmp(buffer, "save", 4)){ /* CASE 4: Save file */
            
                save_status = save();
            
                if(save_status == 0){
                    message = "Error saving database\n";
                    length = strlen(message);
                    write_length(new_sockid, length);
                    writen(new_sockid, message, length);
                
                }else{
                    message = "DATABASE SAVED\n";
                
                    /* Remove copy database if final DB is saved */
                    if(remove("database.bin") == 0){
                        printf("Copy DB succesfully removed\n");
                        n_records = 0;
                    
                    }else{
                        printf("Error deleting Copy DB\n");
                    }
                
                    /* Check if file exists */
                    if(access("DB.bin", F_OK ) == 0) { /* File exists -> Get bytes size */
                        stat("DB.bin", &st);
                        size_orig = st.st_size;
                        n_original_rec = (size_orig / sizeof(srec));
                    
                    } else { /* File does NOT exist yet */
                    
                        size_orig = 0;
                        n_original_rec = 0;
                    }
                
                    length = strlen(message);
                    write_length(new_sockid, length);
                    writen(new_sockid, message, length);
                }
            }else if(!strncmp(buffer, "info", 4)){
                message = "DATABASE COMMANDS\n1: GET (sort by):\nget fname \nget lname \nget SID\nget gpa\n2: PUT:\nput lname,fname,initial(char),SID,gpa\n3: SAVE:\nsave\n4: DELETE:\ndelete SID (5-digit integer)\n5: INFORMATION:\ninfo\n6: CLOSE CLIENT:\nclose\n";
                length = strlen(message);
                write_length(new_sockid, length);
                writen(new_sockid, message, length);
                
            }else{
                message = "Unknown Command, try again. Use 'info' for information about commands\n";
                length = strlen(message);
                write_length(new_sockid, length);
                writen(new_sockid, message, length);
            }
        }
    }
    return 0;
}
