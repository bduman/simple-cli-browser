#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// COLOR DEFINES
#define BLACK           0
#define NAVY            1
#define DARKGREEN       2
#define DARKCYAN        3
#define MAROON          4
#define PURPLE          5
#define OLIVE           6
#define LIGHTGREY       7
#define DARKGREY        8
#define BLUE            9
#define GREEN           10
#define CYAN            11
#define RED             12
#define MAGENTA         13
#define YELLOW          14
#define WHITE           15

// PROTOTYPES
void setColor(int);
char *trimWhiteSpace(char *);

// GLOBALS
char *colors[] = {
    "black", "navy", "darkgreen", "darkcyan", "maroon", "purple",
     "olive", "lightgrey", "darkgrey", "blue", "green", "cyan",
     "red", "magenta", "yellow", "white"
};

char *endColors[] = {
    "end_black", "end_navy", "end_darkgreen", "end_darkcyan", "end_maroon", "end_purple",
     "end_olive", "end_lightgrey", "end_darkgrey", "end_blue", "end_green", "end_cyan",
     "end_red", "end_magenta", "end_yellow", "end_white"
};

char filename[32];
FILE *file;
char *lastFile;

char startNew[128];

char character;

COORD printPosition = {0, 0};
COORD currentPosition = {0, 0};

char prevCharacterFromFile;
char characterFromFile;
char stringFromFile[128];

struct hyperLink {
    char *fileName;
    COORD start;
    COORD finish;
};

struct hyperLink Links[1000];
int lastLinkKey = 0;

int main(int argc, char *argv[]) {

    setColor(WHITE);
    buildNewStart(argv[0]);

    if(argv[1]) {
        strcpy(filename, argv[1]);
    } else {
        printf("Write source file name (not including .htx):");
        gets(filename);
        system("cls");
    }

    strcat(filename, ".htx");

    if((file = fopen(filename, "r")) == NULL) {

        printf("\n!!! .htx file doesnt exist.");
        return 0;

    } else {

        do {

            readFromFile();
            // bu printf i kontrollu bastirmak gerek yani bastiklarimizin kordinatini bilerek bastirmak gerek
            // DONE: 1 - duz dosyadan alinan karakter
            printf("%c", characterFromFile);
            if(characterFromFile == '\n')
                updatePrintPosition(1, 1);
            else
                updatePrintPosition(1, 0);

        } while(!feof(file));

        movement();
    }
}

void readFromFile() {

    int isHyper = 0;
    int i = 0;
    char *token;

    prevCharacterFromFile = characterFromFile;
    characterFromFile = getc(file);

    if(characterFromFile == '/') {

        prevCharacterFromFile = characterFromFile;
        characterFromFile = getc(file);

        // comment baslatildi
        if(characterFromFile == '*' && prevCharacterFromFile == '/') {

            do {
                prevCharacterFromFile = characterFromFile;
                characterFromFile = getc(file);

                if(feof(file))
                    break;

            } while(prevCharacterFromFile != '*' || characterFromFile != '/');

            prevCharacterFromFile = characterFromFile;
            characterFromFile = getc(file);

        } else {

            // DONE: 2 - comment tespiti yaparken acilan slashin commente ait olmamasi durumundaki slashi basma durumu
            printf("%c", prevCharacterFromFile);
            updatePrintPosition(1, 0);
        }

    } else if(characterFromFile == '[') {

        do {


            prevCharacterFromFile = characterFromFile;
            characterFromFile = getc(file);

            if(characterFromFile != ']') {

                stringFromFile[i++] = characterFromFile;
                stringFromFile[i] = '\0';
            }

            if(characterFromFile == '|')
                isHyper = 1;

        } while(characterFromFile != ']');

        if(!isHyper) {

            for(i=0;i<16;i++) {

                if(strcasecmp(stringFromFile, colors[i]) == 0) {
                    setColor(i);
                    break;
                } else if(strcasecmp(stringFromFile, endColors[i]) == 0) {
                    setColor(WHITE);
                    break;
                }
            }

        // hyperLink
        } else {

            setColor(GREEN);
            // dikey cubuk ile parcalama
            token = strtok(stringFromFile, "|");
            // label
            // DONE: 3 - hyper linklerdeki labellerin komple ekrana basilmasi durumu
            printf("%s", token);
            // kordinat baslangici
            Links[lastLinkKey].start.X = printPosition.X;
            Links[lastLinkKey].start.Y = printPosition.Y;
            // kordinat guncellemesi
            updatePrintPosition(strlen(token), 0);
            // kordinat bitisi
            Links[lastLinkKey].finish.X = printPosition.X;
            Links[lastLinkKey].finish.Y = printPosition.Y;
            // tekrar parcalama
            token = strtok(NULL, "|");
            Links[lastLinkKey].fileName = trimWhiteSpace(token);

            // DONE: label uzunlugu kadar baslagic ve bitis kordinatlarini Links[lastLinkKey].start .finish e eklicez

            lastLinkKey++;
            setColor(WHITE);
        }

        prevCharacterFromFile = characterFromFile;
        characterFromFile = getc(file);
    }
}

char *trimWhiteSpace(char *str) {

  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

void buildNewStart(char *currentPath) {

    int i = 0;
    char path[128];

    strcpy(startNew, "start \0");

    for(i;currentPath[i] != '\0';i++) {
        path[i] = (currentPath[i] == '\\') ? '/' : currentPath[i];
    }

    path[i++] = ' ';
    path[i++] = '\0';

    strcat(startNew, path);
}

void printText(int color, char *text) {

    int length = textLength(text);

    setColor(color);
    printf("%s", text);

    updatePrintPosition(length, 0);
    setColor(WHITE);
}

int textLength(char *string) {

    int i = 0;

    for(i; string[i] != '\0'; ++i);

    return i;
}

void newLine() {
    updatePrintPosition(1, 1);
}

void updatePrintPosition(int length, int newLine) {

    if(newLine) {
        printPosition.X = 0;
        printPosition.Y += length;
    } else {
        printPosition.X += length;
    }
}

void movement() {

    updateCursor();
    character = _getch();

    switch(character) {
    case 'a':
        left();
        break;
    case 'd':
        right();
        break;
    case 'w':
        up();
        break;
    case 's':
        down();
        break;
    case '\r':
        if(isHyperPosition()) {
            strcat(startNew, lastFile);
            system(startNew);
        }
        break;
    }

    movement();
}

int isHyperPosition() {

    int i = 0;

    for(i;i<lastLinkKey;i++) {

        if(searchLink(Links[i])) {
            lastFile = Links[i].fileName;
            return 1;
        }
    }

    return 0;
}

int searchLink(struct hyperLink thisLink) {

    // DONE : burada matematiksel islemler olacak 2 kordinat arasinda current var mi yok mu tarzinda
    if(currentPosition.Y*80+currentPosition.X >= thisLink.start.Y*80+thisLink.start.X && currentPosition.Y*80+currentPosition.X < thisLink.finish.Y*80+thisLink.finish.X)
        return 1;

    return 0;
}

void updateCursor() {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), currentPosition);
}

void left() {
    // yatayda eksiye dusmesin diye
    if(currentPosition.X != 0)
        currentPosition.X--;
}

void right() {
    currentPosition.X++;
}

void up() {
    // dikeyde eksiye dusmesin diye
    if(currentPosition.Y != 0)
        currentPosition.Y--;
}

void down() {
    currentPosition.Y++;
}

void setColor(int ForgC) {
     WORD wColor;
     //This handle is needed to get the current background attribute

     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     //csbi is used for wAttributes word

     if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
          //To mask out all but the background attribute, and to add the color
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
     }
     return;
}

int wherex(void) {
  return currentPosition.X;
}

int wherey(void) {
  return currentPosition.Y;
}
