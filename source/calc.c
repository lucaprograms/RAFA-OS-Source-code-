#include "calc.h"
#include "drivers.h"
#include <stdint.h>
#include <math.h>

double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mul(double a, double b) { return a * b; }
double divi(double a, double b) { return b == 0.0 ? 0.0 : a / b; }
int x;
int y;
double custom_sqrt(double num) {
    // Square root of a negative number is imaginary (not supported here)
    if (num < 0.0) return 0.0; 
    if (num == 0.0 || num == 1.0) return num;

    // Start with an initial guess (half of the number)
    double guess = num / 2.0;
    double last_guess = 0.0;

    // Run the Babylonian method loop until the guess is incredibly precise
    // (We look for a difference smaller than 0.00001)
    for (int i = 0; i < 100; i++) {
        last_guess = guess;
        guess = 0.5 * (guess + (num / guess));
        
        // Check if our precision is close enough to stop early
        double diff = guess - last_guess;
        if (diff < 0.0) diff = -diff; // Absolute value
        if (diff < 0.00001) break;
    }

    return guess;
}
double custom_pow(double base, double exp) {
    // Handle 0^0 or any base to power of 0
    if (exp == 0.0) return 1.0;
    if (base == 0.0) return 0.0;

    // Handle negative exponents (e.g., 5^-2 = 1 / 5^2)
    int is_negative = 0;
    if (exp < 0) {
        is_negative = 1;
        exp = -exp;
    }

    double result = 1.0;
    // Cast to integer loop since standard kernel calculators usually expect whole-number powers
    uint32_t num_loops = (uint32_t)exp; 
    for (uint32_t i = 0; i < num_loops; i++) {
        result *= base;
    }

    return is_negative ? (1.0 / result) : result;
}

double calculate(double a, double b, char op) {
    switch (op) {
        case '+': return add(a, b);
        case '-': return sub(a, b);
        case '*': return mul(a, b);
        case 'x': return mul(a, b);
        case '/': if (b == 0) { return NAN; } return divi(a, b);
        case '^': return custom_pow(a, b);
        case 'q': return custom_sqrt(a);
        case 'd': return mul(a, 2);
        case 'h': return divi(a, 2);
        case 'b': b = a; return custom_pow(2, a);
        case 't': return mul(a, 10);
        case 'l': return divi(10, a);
        default:  return 0.0;
    }
}

// Safer string conversion: passes the target buffer instead of relying entirely on static memory
// Safer string conversion: passes the target buffer instead of relying entirely on static memory
void double_to_string(double value, char* buf) {
    int i = 0;
    // value = NAN;
    // value = INFINITY;
    // 1. Intercept Infinity values before doing any math!
    if (isinf(value)) {
        if (value < 0) {
            buf[i++] = '-';
        }
        buf[i++] = 'I';
        buf[i++] = 'n';
        buf[i++] = 'f';
        buf[i++] = 'i';
        buf[i++] = 'n';
        buf[i++] = 'i';
        buf[i++] = 't';
        buf[i++] = 'y';
        
        buf[i] = '\0';
        return; // Exit early so we don't do crazy decimal parsing
    }

    // 2. Intercept NaN (Not a Number) values just in case
    if (isnan(value)) {
        buf[i++] = 'N';
        buf[i++] = 'o';
        buf[i++] = 't';
        buf[i++] = ' ';
        buf[i++] = 'a';
        buf[i++] = ' ';
        buf[i++] = 'v';
        buf[i++] = 'a';
        buf[i++] = 'l';
        buf[i++] = 'i'; 
        buf[i++] = 'd';
        buf[i++] = ' ';
        buf[i++] = 'v';
        buf[i++] = 'a'; 
        buf[i++] = 'l'; 
        buf[i++] = 'u';
        buf[i++] = 'e';
        buf[i++] = '!';
        buf[i] = '\0';
        return;
    }

    // --- Your original formatting math remains untouched below ---
    if (value < 0) {
        buf[i++] = '-';
        value = -value;
    }

    int int_part = (int)value;
    double frac = value - int_part;

    char tmp[16];
    int ti = 0;

    if (int_part == 0) {
        tmp[ti++] = '0';
    } else {
        while (int_part > 0 && ti < 15) {
            tmp[ti++] = '0' + (int_part % 10);
            int_part /= 10;
        }
    }

    while (ti > 0) {
        buf[i++] = tmp[--ti];
    }

    buf[i++] = '.';

    for (int d = 0; d < 3; d++) {
        frac *= 10.0;
        int digit = (int)frac;
        buf[i++] = '0' + digit;
        frac -= digit;
    }

    buf[i] = '\0';
}


int minimal_calc(uint32_t* fb, uint32_t pitch) {
    double a = 0.0;
    double b = 0.0;
    char op = 0;
    int entering_b = 0;

    int decimal_mode = 0;
    double decimal_div = 1.0;

    int show_result = 0;
    double last_result = 0.0;
    int needs_redraw = 1;

    while (1) {
        char c = get_hardware_keypress();

        if (c != 0) {
            needs_redraw = 1;

            if (show_result) {
                a = b = 0.0;
                op = 0;
                entering_b = 0;
                decimal_mode = 0;
                decimal_div = 1.0;
                show_result = 0;
            }

            if (c == 'c') {
                break; 
            }

            if (c == 27) { // ESC Key
                shutdown();
            }
            if (c == 'p')
            {
                        if (!entering_b) {
        a = PI;
    } else {
        b = PI;
    }
    decimal_mode = 0;
    decimal_div = 1.0;
            }
            if (c == 'i')
            {
                  if (!entering_b) {
        a = INFINITY;
    } else {
        b = INFINITY;
    }
            decimal_mode = 0;
    decimal_div = 1.0;
            }
            if (c == 's')
            {
                               if (!entering_b) {
        a = a - a * 2;
    } else {
        b = b - b * 2;
    }
            decimal_mode = 0;
    decimal_div = 1.0;
            }
            if (c == 'r')
            {
              if (!entering_b) {
                  a = -INFINITY;
                } else {
               b = -INFINITY;
               }
                decimal_mode = 0;
                decimal_div = 1.0;
            }
            if (c == 'n')
            {
              if (!entering_b) {
                  a = -a;
                } else {
               b = -b;
               }
                decimal_mode = 0;
                decimal_div = 1.0;
            }

            if (c == '\b') {
                // Simplified integer-only backspace fallback
                if (!decimal_mode) {
                    if (!entering_b) {
                        a = (int)a / 10;
                    } else {
                        b = (int)b / 10;
                    }
                }
                // Note: True float backspacing requires string tracking, 
                // but this prevents the crazy left-shifting decimal bug.
            }
            else if (c >= '0' && c <= '9') {
                int digit = c - '0';
                if (!entering_b) {
                    if (!decimal_mode) {
                        a = a * 10.0 + digit;
                    } else {
                        decimal_div *= 10.0;
                        a = a + (digit / decimal_div);
                    }
                } else {
                    if (!decimal_mode) {
                        b = b * 10.0 + digit;
                    } else {
                        decimal_div *= 10.0;
                        b = b + (digit / decimal_div);
                    }
                }
            }
            else if (c == '.') {
                if (!decimal_mode) { // Avoid double decimals
                    decimal_mode = 1;
                    decimal_div = 1.0;
                }
            }
            else if (
        c == '+' ||
        c == '-' || 
        c == '*' || 
        c == '/' || 
        c == '^' || 
        c == 'q' || 
        c == 'x' || 
        c == 'd' || 
        c == 'h' || 
        c == 'b' ||
        c == 't' ||
        c == 'l') {
                op = c;
                entering_b = 1;
                decimal_mode = 0; 
                decimal_div = 1.0;
            }
            else if (c == '=' || c == '\n' || c == '\r') {
                last_result = calculate(a, b, op);
                show_result = 1;
            }
        }

        if (needs_redraw) {
            clear_screen(background, fb, 800, 600, pitch);
            draw_string("Rafa Calculator Application", 10, 10, textcolor, fb, pitch);

            char str_buf[32];
            if (show_result) {
                draw_string("Result:", 10, 30, textcolor, fb, pitch);
                double_to_string(last_result, str_buf);
                draw_string(str_buf, 10, 50, textcolor, fb, pitch);
            } else {
                char buffer[64];
                int idx = 0;

                double_to_string(a, str_buf);
                char* s = str_buf;
                while (*s) buffer[idx++] = *s++;

                if (op) {
                    buffer[idx++] = ' ';
                    buffer[idx++] = op;
                    buffer[idx++] = ' ';
                }

                if (entering_b) {
                    double_to_string(b, str_buf);
                    s = str_buf;
                    while (*s) buffer[idx++] = *s++;
                }

                buffer[idx] = '\0';
                draw_string(buffer, 10, 30, textcolor, fb, pitch);
            }

            needs_redraw = 0;
        }
    }
    return 100;
}
