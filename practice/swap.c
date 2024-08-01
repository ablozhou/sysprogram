#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char first[20];
    char last[20];
} Person;

void swapInt(int* first, int* second) {
   
    int temp = *first;
    *first= *second;
    *second=temp;
    
}

void swapPersons(Person *p1, Person *p2) {
    Person t = *p1;
    *p1 = *p2;
    *p2 = t;

}

void swap(void* p1, void* p2, int size) {
    void* t = malloc(size);
    memcpy(t,p1,size);
    memcpy(p1,p2, size);
    memcpy(p2,t, size);
}

void main(int argc, char* argv[]) {
    
    if (argc != 3 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0) {
        printf("Usage %s n1 n2 \n", argv[0]);
        exit(1);
    }

    int arg1 = atoi(argv[1]);
    int arg2 = atoi(argv[2]);

    printf("Before %d, %d\n", arg1, arg2);
    swapInt(&arg1,&arg2);
    printf("After %d, %d\n", arg1, arg2);

    Person p1={"alice", "Smith"};
    Person p2={"Bob", "Doe"};
    printf("Before %s, %s; %s, %s\n", p1.first, p1.last, p2.first, p2.last);
    //swapPersons(&p1,&p2);
    swap(&p1,&p2,sizeof(Person));
    printf("After %s, %s; %s, %s\n", p1.first, p1.last, p2.first, p2.last);
    exit(0);
}