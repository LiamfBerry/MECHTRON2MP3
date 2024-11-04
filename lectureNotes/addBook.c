#include <stdio.h>
#include <string.h>

#define MAX_BB 2
#define MAX_NAME 50
#define MAX_BOOKS 100
#define MAX_MEMBERS 5

struct Book {
    char title[MAX_NAME];
    char author[MAX_NAME];
    int pubYear;
};

struct LibraryMember {
    char name[MAX_NAME];
    int id;
    struct Book bb[MAX_BB];
    int numBB;
};

void bb(struct LibraryMember *member , struct Book book) {
    if (member->numBB<MAX_BB) {
        member->bb[member->numBB] = book;
        member->numBB++;
        printf("%s borrowed \"%s\" by %s.\n", member->name, book.title, book.author);
    }
    else {
        printf("%s cannot borrow more books.\n", member->name);
    }
}

int main() {

    struct Book book1={"The Great Gatsby", "F. Scott Fitzgerald", 1925};
    struct Book book2={"What is Man?", "Mark Twain", 1906};
    struct Book book3={"To Kill a Mockingbird", "Harper Lee", 1960};
    struct Book book4={"1984", "George Orwell", 1949};

    struct LibraryMember *members = (struct LibraryMember *)malloc(MAX_MEMBERS * sizeof(struct LibraryMember));
    int numMembers = 0;
}