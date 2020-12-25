/*****************************************************************************************
File Name     : main.c
Author        : Dingjiang Zhou
Purpose       : joystick demo in pure C code
*****************************************************************************************/
// http://archives.seul.org/linuxgames/Aug-1999/msg00107.html

#include <cstdlib>
#include <fcntl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define JOY_DEV "/dev/input/js0"

int main(int argc, char* argv[]) {
    int             js_f;
    int*            axis      = NULL;
    int             n_axes    = 0;
    int             n_buttons = 0;
    char*           button    = NULL;
    char            js_name[80];
    struct js_event js;

    // open joystick
    if ((js_f = open(JOY_DEV, O_RDONLY)) == -1) {
        printf("Couldn't open joystick.\n");
        return -1;
    }

    // fetch some information from the joystick
    ioctl(js_f, JSIOCGAXES, &n_axes);
    ioctl(js_f, JSIOCGBUTTONS, &n_buttons);
    ioctl(js_f, JSIOCGNAME(80), &js_name);

    // assign memory for axes and buttons
    axis   = ( int* )calloc(n_axes, sizeof(int));
    button = ( char* )calloc(n_buttons, sizeof(char));

    printf("Joystick detected: %s\n", js_name);
    if ((n_axes == 6) && (n_buttons == 12)) {
        printf("Joystick in D mode: ");
    }
    else if ((n_axes == 8) && (n_buttons == 11)) {
        printf("Joystick in X mode: ");
    }
    printf("%d axes, %d buttons\r\n", n_axes, n_buttons);

    // use non-blocking mode
    fcntl(js_f, F_SETFL, O_NONBLOCK);

    if (n_axes == 6) {
        printf("     0      1      2      3      4      5  | ");
    }
    else if (n_axes == 8) {
        printf("     0      1      2      3      4      5      6      7  | ");
    }
    if (n_buttons == 11) {
        printf("  0   1   2   3   4   5   6   7   8   9  10 \n");
    }
    else if (n_buttons == 12) {
        printf("  0   1   2   3   4   5   6   7   8   9  10  11\n");
    }

    while (1) {
        ssize_t bb = read(js_f, &js, sizeof(struct js_event));
        // reset
        for (int i = 0; i < n_axes; i++) {
            axis[i] = 0;
        }
        for (int i = 0; i < n_buttons; i++) {
            button[i] = 0;
        }
        switch (js.type) {
        case JS_EVENT_AXIS:
            axis[js.number] = js.value;
            break;
        case JS_EVENT_BUTTON:
            button[js.number] = js.value;
            break;
        }
        // print result ----------------
        for (int i = 0; i < n_axes; i++) {
            if (axis[i] < 0) {
                printf("-%5d ", -axis[i]);
            }
            else {
                printf(" %5d ", axis[i]);
            }
        }
        printf(" | ");
        for (int i = 0; i < n_buttons; i++) {
            printf("%3d ", button[i]);
        }
        printf("  \r");
        fflush(stdout);
        usleep(100);
    }

    close(js_f);
    return 0;
}
