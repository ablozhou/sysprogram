#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>

typedef int (*visitor)(int);
#define MAX 100
char* upcase(char* s) {
    char* origin = s;
    if( s==NULL) {
        printf("message should not be null");
        return s;
    }
    while( *s != '\0') {
        *s = toupper(*s);
        s++;
    }
    return origin;
}
char* lowercase(char* s) {
    char* origin = s;
    if( s==NULL) {
        printf("message should not be null");
        return s;
    }
    while( *s != '\0') {
        *s = tolower(*s);
        s++;
    }
    return origin;
}

char* transform(char* s,  visitor v) {
    char* origin = s;
    if( s==NULL) {
        printf("message should not be null");
        return s;
    }
    while( *s != '\0') {
        *s = v(*s);
        s++;
    }
    return origin;
}
void main(int argc, char* argv[]) {
    char message[MAX]={0};
    if(argc!=2){
        printf("Usage: %s str\n", argv[0]);
        exit(1);
    }
    printf("Before: %s\n",argv[1]);
    strcpy(message, argv[1]);
    char* up= upcase(argv[1]);
    if(up!=NULL) {
        printf("up:%s\n", up);
    }

    char* lower = lowercase(message);
    if(lower!=NULL) {
        printf("lower:%s\n", lower);
    }

    
    char* up1= transform(message, toupper);
    if(up1!=NULL) {
        printf("up1:%s\n", up1);
    }
    lower = transform(message, tolower);
    if(lower!=NULL) {
        printf("lower1:%s\n", lower);
    }

    char *s[] = {"helloo"};
    char *s1 = malloc(strlen(*s));
    strcpy(s1,*s);
    up1= transform(s1, toupper);
    if(up1!=NULL) {
        printf("up1:%s\n", up1);
    }
    exit(0);
}