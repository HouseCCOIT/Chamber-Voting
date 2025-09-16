#if !defined(AFX_HOUSE_H__F1B86DE3_6136_11D2_9F8D_00104B9A2A27__INCLUDED_)
#define AFX_HOUSE_H__F1B86DE3_6136_11D2_9F8D_00104B9A2A27__INCLUDED_
// #define MSC 1
// #include <window.h>
// #include <stdio.h>

#define OK  0
#define YES 1
#define NO  0
#define NULLF 0L

#define REVERSE_VIDEO (0x70)
#define NORMAL_VIDEO (0x07)
#define BOLD_VIDEO (0x0f)

#define CURSOR_OFF 0x0030
#define CURSOR_TOP 9
#define CURSOR_BOTTOM 11

#define TOP_CHOICE 1
#define LEFT_CHOICE 35
#define TOP_DISPLAY 1
#define LEFT_DISPLAY 7

#define INKEY_ECHO 1
#define INKEY_NOECHO 8
#define WWERR_BOUND 40

#define BUFFER_LENGTH 100
#define MAX_DIGITS 5
#define MD_WIDTH 16
#define MAX_MESSAGE MD_WIDTH
#define TD_WIDTH 40
#define MD_DOTS_PER_ROW (6 * MD_WIDTH)

#define CR 13
#define LF 11
#define DC1 17
#define ESC 27
#define BS 8
#define DEL 127
#define SPACE 32
#define EOS '\0'

#define MD_MOVE '\030'
#define MD_CLEAR '\014'
#define MD_BRIGHT '\031'
#define MD_TEST DC1

#define TD_CLEAR '\032'

#define PRINT_RESULT 98
#define TOTALS_ONLY 99
#define BLANK_LED 0xf

#define SEMA_MD 0
#define NO_TIMEOUT 0

#define VOTE_YES 2
#define VOTE_NO 1
#define VOTE_ABSENT 0

#define STATION_UNLOCKED 0
#define STATION_LOCKED 1

#define PRINTOUT_ROWS 45
#define PRINTOUT_COLS 3
#define PRINTOUT_TITLES 3
#define NAME_PRINTABLE 17

#define VS_MD_BASE        200
#define VS_MD_VOTE_OPEN   (VS_MD_BASE + 1)
#define VS_MD_VOTE_CLOSED (VS_MD_BASE + 2)
#define VS_MD_RESULTS     (VS_MD_BASE + 3)
#define VS_MD_REQHEAD     (VS_MD_BASE + 4)
#define VS_MD_LOCKS_DONE  (VS_MD_BASE + 5)


/* T Y P E   D E F I N I T I O N S */

typedef struct entry {
    char *top;
    char *bottom;
} ENTRY;
/*
typedef struct type {
    unsigned short start_row;
    WINDOW *ww;
    ENTRY *phrases;
    unsigned short number_phrases;
    char *header;
} TYPE;
*/
typedef struct offsets {
    unsigned short md;
    unsigned short td;
} OFFSETS;
/*
typedef struct queue {
    unsigned short current_page;
    unsigned short current_offset;
    unsigned short next_item;
    WINDOW *ww;
    char **entry;
    char *order_of_business;
} QUEUE;
*/
typedef struct led {
    unsigned hundred : 1;
    unsigned tens : 4;
    unsigned units : 4;
} LED;

typedef struct lex {
    char  name[35];
    unsigned char seat;
    unsigned char vote;
    unsigned int  seatID;
	char county[31];
} LEX;
typedef struct lex2 {
    char  name[35];
    char  fullName[41];
    unsigned char seat;
    unsigned char vote;
    unsigned int  seatID;
    char county[31];
} LEX2;
/*
typedef struct calendar_backup {
    char *fname;
    QUEUE *source_queue;
    FILE *file;
} CALENDAR_BACKUP;
*/
#endif
