





    #include <stdint.h>
    #include <efi.h>
    #include <efilib.h>
    #include "drivers.h"
    #include "inputboxes.h"

    int counter11 = 0;
    int enableDiagonal = 1;

    static char s[100000];
    static int tracker = 0;
    static int tr = 0;

    /*
     * Convert an integer to a null-terminated string.
     * Does not require sprintf() or snprintf().
     */
    static void int_to_string(int num, char *str)
    {
        int i = 0;
        int start = 0;
        unsigned int n;

        if (num < 0) {
            str[i++] = '-';
            start = i;
            n = (unsigned int)(-(num + 1)) + 1;
        } else {
            n = (unsigned int)num;
        }

        do {
            str[i++] = (char)((n % 10) + '0');
            n /= 10;
        } while (n > 0);

        str[i] = '\0';

        /* Reverse the digits without reversing the minus sign. */
        for (int j = start, k = i - 1; j < k; j++, k--) {
            char temp = str[j];
            str[j] = str[k];
            str[k] = temp;
        }
    }

    /*
     * Increment the counter and return its string representation
     * followed by a period.
     */
    static char *counter1(void)
    {
        static char str[16];

        if (counter11 < 2147483647) {
            counter11++;
        } else {
            counter11 = 0;
        }

        int_to_string(counter11, str);

        int len = 0;
        while (str[len] != '\0' && len < (int)sizeof(str) - 2) {
            len++;
        }

        str[len] = '.';
        str[len + 1] = '\0';

        return str;
    }

    static char *returnmessage(void)
    {
        if (enableDiagonal == 1) {
            return "[n] start a new line \n"
                   "[t] press the tab key [c] Clear the screen "
                   "[d] Diagonal mode [x] Hide menu. "
                   "Press any other key to exit inputboxes...";
        }

        return "[n] start a new line \n"
               "[h] Hide Diagonal Mode [t] press the tab key "
               "[c] Clear the screen [x] Hide menu. "
               "Press any other key to exit inputboxes...";
    }

    int inputboxes(
        uint32_t *framebuffer,
        uint32_t width,
        uint32_t height,
        uint32_t pitch
    )
    {
        char key;
        char key1;

        int count = 0;
        int x = Val + 4;
        int y = Val + 4;

        int breaksig = 0;
        int code = 0;

        int x1 = Val + 4;
        int y1 = Val + 4;

        char c1 = 0;

        while (1) {
            key = get_hardware_keypress();

            if (key != 0) {
                if (key == '\n' || key == '\r') {
                    draw_string(
                        returnmessage(),
                        100, 700,
                        textcolor,
                        framebuffer,
                        pitch
                    );

                    while (1) {
                        key1 = get_hardware_keypress();

                        if (key1 != 0) {
                            if (key1 == 'n' || key1 == 'N') {
                                breaksig = 2;

                                y += Val;
                                x = Val + 4;

                                uint32_t start_x = 100;
                                uint32_t start_y = 700;

                                uint32_t end_x = start_x + (71 * Val);
                                uint32_t end_y = start_y + Val + 4;

                                for (uint32_t py = start_y;
                                     py < end_y && py < height;
                                     py++) {
                                    for (uint32_t px = start_x;
                                         px < end_x && px < width;
                                         px++) {
                                        draw_pixel(
                                            px, py, background,
                                            framebuffer, pitch
                                        );
                                    }
                                }

                                break;

                            } else if (key1 == 't' || key1 == 'T') {
                                x += 4 * Val;
                                breaksig = 2;

                                uint32_t start_x = 100;
                                uint32_t start_y = 700;

                                uint32_t end_x = start_x + (71 * Val);
                                uint32_t end_y = start_y + Val + 4;

                                for (uint32_t py = start_y;
                                     py < end_y && py < height;
                                     py++) {
                                    for (uint32_t px = start_x;
                                         px < end_x && px < width;
                                         px++) {
                                        draw_pixel(
                                            px, py, background,
                                            framebuffer, pitch
                                        );
                                    }
                                }

                                break;

                            } else if (key1 == 'q' || key1 == 'Q')
                            {
                                    x = 0;
        y = 0;
        x1 = 0;
        y1 = 0;
        counter11 = 0;
      tracker = 0;
      for (int i = 0; i < 100000; i++)
{
    s[i] = 0;
}

                            } else if (key1 == 'd' || key1 == 'D') {
                                if (enableDiagonal == 1) {
                                    clear_screen(
                                        background,
                                        framebuffer,
                                        width,
                                        height,
                                        pitch
                                    );

                                    breaksig = 2;

                                    for (int i = 0; i < tracker; i++) {
                                        draw_char(
                                            s[i],
                                            x1, y1,
                                            textcolor,
                                            framebuffer,
                                            pitch
                                        );

                                        x1 += Val;
                                        y1 += Val;
                                    }

                                    x1 = Val + 4;
                                    y1 = Val + 4;

                                    break;
                                }

                            } else if (key1 == 'a' || key1 == 'A') {
                                char *counter_str = counter1();
                                 y += Val;

                                draw_string(
                                    counter_str,
                                    x+4, y,
                                    textcolor,
                                    framebuffer,
                                    pitch
                                );
                               x += Val + Val / 2;
                            } else if (key1 == 'x' || key1 == 'X') {
                                breaksig = 2;

                                uint32_t start_x = 100;
                                uint32_t start_y = 700;

                                uint32_t end_x = start_x + (71 * Val);
                                uint32_t end_y = start_y + Val + 4;

                                for (uint32_t py = start_y;
                                     py < end_y && py < height;
                                     py++) {
                                    for (uint32_t px = start_x;
                                         px < end_x && px < width;
                                         px++) {
                                        draw_pixel(
                                            px, py, background,
                                            framebuffer, pitch
                                        );
                                    }
                                }

                                break;

                            } else if (key1 == 'c' || key1 == 'C') {
                                breaksig = 2;

                                x = Val + 4;
                                y = Val + 4;

                                tracker = 0;
                                count = 0;
                                s[0] = '\0';

                                clear_screen(
                                    background,
                                    framebuffer,
                                    width,
                                    height,
                                    pitch
                                );

                                break;

                            } else if (key1 == 'r' || key1 == 'R') {
                                draw_char(
                                    c1,
                                    x, y,
                                    textcolor,
                                    framebuffer,
                                    pitch
                                );

                            } else if (key1 == 'h' || key1 == 'H') {
                                breaksig = 2;

                                if (enableDiagonal == 1) {
                                    enableDiagonal = 0;
                                } else {
                                    enableDiagonal = 1;
                                }

                                break;

                            } else {
                                breaksig = 1;
                                break;
                            }
                        }

                        for (volatile int i = 0; i < 50000; i++) {
                        }
                    }
                }

                if (breaksig == 1) {
                    break;
                }

                if (breaksig == 2) {
                    breaksig = 0;
                    continue;
                }

                if (key == '\t') {
                    x += 4 * Val;
                    continue;
                }

                c1 = key;

                draw_char(
                    key,
                    x, y,
                    textcolor,
                    framebuffer,
                    pitch
                );

                /* Store the character safely. */
                if (tracker < (int)sizeof(s) - 1) {
                    s[tracker++] = key;
                    s[tracker] = '\0';
                }

                count++;
                x += Val;
            }

            for (volatile int i = 0; i < 50000; i++) {
            }
        }

        clear_screen(
            background,
            framebuffer,
            width,
            height,
            pitch
        );
          return code;
    }
