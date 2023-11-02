/* Comp 211: Homework 9 Challenge: Multiple Line buffer
*
* Alex Glotzer
*/

#include <stdio.h>
#include <stdlib.h>
#include "chaleditbuffer.h"


struct node {
    //stores the actual letter
    char val ;
    //location of the next node in the list
    struct node* next ;
    //location of the previous node in the list
    struct node* prev ;
} ;
typedef struct node node;

struct linenode {
    //head node of the actual line
    node* line;
    //location of the next line
    struct linenode* next;
} ;
typedef struct linenode linenode;

struct buffer{
    //head of the linked list
    linenode* hd;
    //node where the insertion mark is. To the left of the character
    node* insert;
    //integer position of the insert mark. pos of <ab|c> would be 2
    struct pos loc;
    //number of values in the buffer. size of <abc|> would be 3
    int totsize;
    //number of lines
    int linecount;
    //number of values in the current line
    int linesize;
} ;


/* This line just makes 'buffer' an abbreviation for 'struct buffer'.
 */
typedef struct buffer buffer ;

/* empty = an empty buffer.
 */
buffer* buf_empty(){
    buffer* b = malloc(sizeof(buffer));
    b->hd = malloc(sizeof(linenode)); //creating empty list of lines
    b->hd->line = malloc(sizeof(node));
    b->hd->line->prev = NULL;
    b->hd->line->val = '\0';
    b->hd->line->next = NULL;
    b->hd->next = NULL;
    //setting the insertion mark location to hd
    b->insert = b->hd->line;
    //declaring that the insertion mark position structure starts with no values
    //on the first row
    b->loc.row = 0;
    b->loc.column = 0;
    //delcaring the size to be 0
    b->totsize = 0;
    //declaring the size to be 1 since an empty buffer would still be no
    //characters on the first line
    b->linecount = 1;
    //declaring an empty line size
    b->linesize = 0;
    return b;
}

/* insert(buf, c) inserts c to the left of the insertion point of buf.  After
 * the insertion, the insert mark is to the right of c.
 *
 * THIS FUNCTION MUST BE CONSTANT TIME!
 */
void buf_insert(buffer* b, char c){
    node* n = malloc(sizeof(node));
    n->next = b->insert->next;
    if(b->insert->next != NULL) {
        b->insert->next->prev = n;
    }
    n->prev = b->insert;
    b->insert->next = n;
    b->insert = n;
    b->insert->val = c;
    b->loc.column++;
    b->totsize++;
    b->linesize++;
    return;
}

/* insert_newline(buffer *):  starts a new line just after the current line.
 *
 * Pre-conditions:  insert is to the left of position (r, c).
 * Post-condition:  lines r+1,..., are moved to lines r+2,....
 *                  line r consists of characters (r, 0),...,(r, c-1).
 *                  line r+1 consists of characters (r, c), (r, c+1),...
 *                  insert is at the beginning of line r+1.
 *
 * THIS FUNCTION MUST BE CONSTANT SPACE!  That is, the amount of memory
 * allocated by this function must always be the same, no matter what the
 * contents of the buffer.
 */
void buf_insert_newline(buffer* b) {
    linenode* ln = b -> hd;
    while(ln->next != NULL) {
        ln = ln->next;
    }
    ln->next = malloc(sizeof(linenode));
    ln = ln->next;
    ln->line = malloc(sizeof(node));
    ln->line->prev = NULL;
    ln->line->val = '\0';
    ln->line->next = NULL;
    ln->next = NULL;
    b->loc.row++;
    b->loc.column = 0;
    b->insert = ln->line;
    b->linecount++;

    //printf("b->hd->next->line->val: %c\n", b->hd->next->line->val);
    b->linesize = 0;
}

/* delete_left(buf) deletes the character to the left of the insert mark.  If
 * there is no character to the left of the insert mark, this function has no
 * effect.
 *
 * THIS FUNCTION MUST BE CONSTANT TIME!
 */
void buf_delete_left(buffer* b) {
    if (b->loc.column > 0) {
       if (b->insert->next != NULL) {
           b->insert->prev->next = b->insert->next;
           b->insert->next->prev = b->insert->prev;
       }
       node* n = b->insert;
       if(b->insert->next == NULL) {
           b->insert = b->insert->prev;
           b->insert->next = NULL;
       }
       else {
           b->insert = b->insert->prev;
       }
       free(n);
       b->loc.column--;
       b->totsize--;
       b->linesize--;
   }
   return;
}


/* delete_right(buf) deletes the character to the right of the insert mark.  If
 * there is no character to the right of the insert mark, this function has no
 * effect.
 *
 * THIS FUNCTION MUST BE CONSTANT TIME!
 */
void buf_delete_right(buffer* b) {
    if (b->loc.column < b->linesize) {
       b->insert = b->insert->next;
       if (b->insert->next != NULL){
           b->insert->prev->next = b->insert->next;
           b->insert->next->prev = b->insert->prev;
       }
       node* n = b->insert;
       if(b->insert->next == NULL) {
           b->insert = b->insert->prev;
           b->insert->next = NULL;
       }
       else {
           b->insert = b->insert->prev;
       }
       free(n);
       b->totsize--;
       b->linesize--;
   }
   return;
}

/* move_left(buf) moves the insert mark one character to the left.  If there is
 * no character to the left of the insert mark, this functdion has no effect.
 *
 * THIS FUNCTION MUST BE CONSTANT TIME!
 */
void buf_move_left(buffer* b) {
    if(b->insert->prev == NULL && b->loc.column != 0) {
        b->loc.column = 0;
        return;
    }
    else if(b->insert->prev != NULL) {
        b->insert = b->insert->prev;
        b->loc.column--;
        return;
    }
    else{
        return;
    }
}

/* move_right(buf) moves the insert mark one character to the right.  If there
 * is no character to the right of the insert mark, this functdion has no
 * effect.
 *
 * THIS FUNCTION MUST BE CONSTANT TIME!
 */
void buf_move_right(buffer* b) {
    if(b->insert->next == NULL && b->loc.column != b->linesize) {
        b->loc.column = b->linesize;
        return;
    }
    if(b->insert->next != NULL) {
        b->insert = b->insert->next;
        b->loc.column++;
        return;
    }
    else {
        return;
    }
}

/* buf_get_line_size(buf, lineno) = the number of characters in line lineno.
 */
int buf_get_line_size(buffer* b, int lineno) {
    linenode* n = b -> hd;
    for(int i = 0; i < lineno && n -> next != NULL; i++) {
        n = n -> next;
    }
    int count = 0;
    node* x = n->line->next;
    while(x != NULL) {
        count++;
        x = x->next;
    }
    return count;
}

/* move_up(buf) moves the insertion mark to the line above the current line.
 * The horizontal position of the insertion mark will be min(h, l), where h is
 * the horizontal position of the insert mark in buf, and l is the length of
 * the line above the current line.  In other words, the horizontal position is
 * unchanged, unless the line above is too short, in which case the insertion
 * mark is placed at the end of the line above.  If there is no line above,
 * this function has no effect.
 */
void buf_move_up(buffer* b) {
    if(b->loc.row == 0){
        return;
    }
    else{
        linenode* n = b->hd;
        for(int i = 0; i < b->loc.row-1; i++) {
            n = n->next;
        }
        int c;
        node* x = n->line;
        for(c = 0; c < b->loc.column && x != NULL; c++) {
            x = x -> next;
        }
        b->insert = x;
        b->loc.row--;
        if(c > buf_get_line_size(b, b->loc.row)) {
            b->loc.column = buf_get_line_size(b, b->loc.row);
        }
        b->linesize = buf_get_line_size(b, b->loc.row);
        return;
    }
}

/* move_down(buf) moves the insertion mark to the line below the current line.
 * The horizontal position of the insertion mark will be min(h, l), where h is
 * the horizontal position of the insert mark in buf, and l is the length of
 * the line below the current line.  In other words, the horizontal position is
 * unchanged, unless the line below is too short, in which case the insertion
 * mark is placed at the end of the line below.  If there is no line below,
 * this function has no effect.
 */
void buf_move_down(buffer* b) {
    if(b->loc.row == b->linecount-1){
        return;
    }
    else{
        linenode* n = b->hd;
        for(int i = 0; i < b->loc.row+1; i++) {
            n = n->next;
        }
        int c;
        node* x = n->line;
        for(c = 0; c < b->loc.column && x != NULL; c++) {
            x = x -> next;
        }
        b->insert = x;
        b->loc.row++;
        if(c > buf_get_line_size(b, b->loc.row)) {
            b->loc.column = buf_get_line_size(b, b->loc.row);
        }
        b->linesize = buf_get_line_size(b, b->loc.row);
        return;
    }
}

/* set(buf, pos) sets the insert mark so that it is to the left of the
 * character at position pos.  It is permissible for pos.col to be equal the
 * number of characters in row pos.row, in which case the insert mark is set to
 * be to at the end of the line at p.row.
 *
 * Pre-conditions:
 * - 0 ≤ pos.row < the number of lines in the buffer.
 * - 0 ≤ pos.col ≤ the number of characters in row pos.row.
 */
void buf_set(buffer* b, struct pos p) {
    linenode* n = b->hd;
    for(int i = 0; i <= p.row && n->next != NULL; i++) {
        n = n -> next;
    }
    node* x = n->line;
    for(int i = 0; i < p.column && x->next != NULL; i++) {
        x = x->next;
    }
    b->insert = x;
    b->loc.row = p.row;
    b->loc.column = p.column;
}

/* If p = get_position(buf), then the insert mark is just to the left of the
 * character at position (p.row, p.column).  It is possible that p.column is
 * equal to the number of characters in pos.row, in which case this means that
 * the insert mark is at the end of the line.
 */
struct pos buf_get_position(buffer* b) {
    return b->loc;
}

/* buf_get_nlines(buf) = the number of lines in the buffer.
 */
int buf_get_nlines(buffer* b) {
    return b->linecount;
}



/* get_line(buffer*, r, line):  get the line of text at row r.
 *
 * Pre-condition:  the length of line is at least one greater than the number
 * of characters in row r.
 *
 * Post-condition:  line is a NULL-terminated string corresponding to the line
 * at row r.
 */
void buf_get_line(buffer* b, int r, char line[]) {
    linenode* n = b->hd;
    for(int i = 0; i < r && n->next != NULL; i++) {
        n = n->next;
    }
    node* x = n->line->next;
    for(int i = 0; x != NULL; i++){
        line[i] = x->val;
        x = x->next;
    }
    line[buf_get_line_size(b, r)] = '\0';
    return;
}

/* buf_free(buf):  free all resources associated to buf, including buf itself.
 */
void buf_free(buffer* b) {
    linenode* ln = b->hd;
    linenode* temp;
    while(ln->next != NULL){
        node* n = ln->line;
        while(n->next != NULL){
            node* tempnode = n;
            n = n->next;
            free(tempnode);
        }
        free(n);
        temp = ln;
        if(ln->next != NULL){
            ln = ln->next;
        }
        free(temp);
    }
    free(b);
    return;
}



/*  print_buffer(buf).
 *
 *  Post-condition:  Some information about buf will be printed to ther
 *  terminal.
 *
 *  The implementation of this function is entirely up to you.  It is here to
 *  help you with debugging.  We will never use it when testing your code, but
 *  there is an option in the driver program that will call this function with
 *  the current buffer.
 */
void buf_print(buffer* b) {
    printf("Buffer size: %d\n", b->totsize);
    printf("Insert mark row: %d\n", b->loc.row);
    printf("Insert mark column: %d\n", b->loc.column);
    printf("Number of lines: %d\n", b->linecount);
    printf("Size of current row: %d\n", b->linesize);
}
