#include "def.h"
#include "graphics.h"

/* Main program */
int main(void)
{
    bool result;

    /* Set screen colors */
    (void) textcolor(COLOR_YELLOW);
    (void) bordercolor(COLOR_BLUE);
    (void) bgcolor(COLOR_BLACK);

    /* Clear the screen, put cursor in upper left corner */
    clrscr();

    /* Hide cursor */
    mouse_hide();

    printf("***  press a key to load characters  ***\r\n");
    cgetc();
    printf("***       loading characters         ***\r\n\r\n");

    /* Load custom characters */
    result = LoadCustomChars();

    if (result)
    {
        /* Wait for the user to press a key */
        printf("***        characters loaded         ***\r\n");
        printf("***     press a key to set chars     ***\r\n\r\n");
        cgetc();

        /* Set custom characters */
        SetCustomChars();

        /* Wait for the user to press a key */
        clrscr();
        printf("***              done                ***\r\n");
        printf("***    press a key to end program    ***\r\n\r\n");
        cgetc();

        /* Set original characters */
        /* SetDefaultChars(); */

        /* Clear the screen again */
        clrscr();

        /* Print all character codes on screen (debug) */
        /* PrintCustomCharsCode(); */
    }

    /* Set screen colors */
    (void) textcolor(COLOR_WHITE);
    (void) bordercolor(COLOR_LIGHTBLUE);
    (void) bgcolor(COLOR_BLUE);

    /* Show cursor */
    mouse_show();
   
    return EXIT_SUCCESS;
}

