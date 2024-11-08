#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <time.h> 

void arith_op_any_base(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute);
    
void add_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values);

void sub_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values);

void mult_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values);


void arith_op_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute);

void add_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len);

void sub_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len);

void mult_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len);


void arith_op_any_base_V2(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute);

void add_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len);

void sub_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len);

void mult_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len);

const char help[] = "Arithmetic operations in any base:\n\n"
    "Required arguments:\tz1 z2\n"
    "Optional arguments:\t-h -V -B -a -b -o\n\n"
    "z1\tany whole number (including zero and negative numbers)\n"
    "z2\tany whole number (including zero and negative numbers)\n"
    "  \tz1 and z2 are positional arguments --> no declaration of these options needed, just put in the numbers\n"
    "  \tIf you want to enter a negative number, put a double minus sign in front of it, e.g., -5 will become -- -5\n"
    "  \tWhen you use -- then you are not allowed to use any declaration of an option after that.\n"
    "  \tThis is allowed: -b 7 3 -- -5\n"
    "  \tThis is not allowed: 3 -- -5 -b 7\n\n"
    "-h\tdisplay a guide that explains how to use this program\n\n"
    "-V\tchoose the implementation version that should be used for performing the calculation\n"
    "  \tpossible implementation versions: 0, 1, 2\n"
    "  \timplementation version 0: optimized implementation using lookup table\n"
    "  \timplementation version 1: unoptimized implementation using the original representation of z1 and z2\n"
    "  \timplementation version 2: unoptimized implementation using the binary representation of z1 and z2\n"
    "  \tdefault implementation version: 0\n"
    "  \texample: -V 2\n\n"
    "-B\tchoose the number of iterations for the performance test\n"
    "  \tthe number of iterations has to be a whole number greater than 0\n"
    "  \texample: -B 384\n\n"
    "-a\tspecify the alphabet you want to use for the representation of z1 and z2\n"
    "  \tif the base value is greater than 10 an alphabet needs to be specified, if the base value is 10 or less then no alphabet must be specified\n"
    "  \tall characters from the ASCII table are allowed except the minus sign --> 127 possible characters\n"
    "  \tthe length of the alphabet has to match the value of the provided basis\n"
    "  \tno duplicates allowed: every character in the alphabet has to be unique\n"
    "  \tif the absolute base value is 10 or less, then the alphabet is being auto-generated: starting from 0 and going up to 9\n"
    "  \texample: -a abcdefghijklmnop123456789\n\n"
    "-b\tspecify the base you want to use for the calculation\n"
    "  \tthe base has to be a whole number (including negative numbers)\n"
    "  \tthe absolute value of the base has to be between 2 and 255\n"
    "  \tthe base value and the length of the alphabet must match\n"
    "  \tdefault base value: 10\n"
    "  \texample: -b 7\n\n"
    "-o\tchoose the operation you want to use for z1 and z2\n"
    "  \tpossible operations: + (addition), - (subtraction), * (multiplication)\n"
    "  \tif you choose * then you need to write a backslash before it: -o \\*\n"
    "  \tdefault operation: +\n"
    "  \texample: -o -\n"
    ;

int main(int argc, char **argv) {

    // First two arguments always have to be the two numbers to operate on
    char* z1 = NULL;
    char* z2 = NULL;

    char* strtol_endptr;
    char implementation_to_use = 0; // default implementation, -V
    bool has_performance_parameter = false; // -B
    int iterations;
    int base = 10; // default base, -b
    bool has_alphabet_parameter = false; // -a
    char* alphabet;
    size_t alphabet_len;
    char operation = '+'; // default operation, -o

    // --help option
    static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'}
    };
    
    char opt;
    while((opt = getopt_long(argc, argv, "V:B:b:a:o:h", long_options, NULL)) != -1) {
        switch(opt) {
            case 'V':
                implementation_to_use = strtol(optarg, &strtol_endptr, 10);
                if(strtol_endptr != NULL && *strtol_endptr != 0) {
                    fprintf(stderr, "Implementation-Value is not a valid integer.\n");
                    return 1;
                } else if (errno == ERANGE) {
                    perror("Implementation-value caused an overflow. Please pick a whole number between 0 and 2.\n");
                    return 1;
                }
                break;
            case 'B':
                has_performance_parameter = true;
                iterations = strtol(optarg, &strtol_endptr, 10);
                if (iterations<1){
                    fprintf(stderr, "Iterations-Value has to be greater than 0.\n");
                    return 1;
                }
                if(strtol_endptr != NULL && *strtol_endptr != 0) {
                    fprintf(stderr, "Iterations-Value is not a valid integer.\n");
                    return 1;
                } else if (errno == ERANGE) {
                    perror("Iterations-value caused an overflow. Please pick a whole number that is less or equal to 2,147,483,647.\n");
                    return 1;
                }
                break;
            case 'b':
                base = strtol(optarg, &strtol_endptr, 10);
                
                if(strtol_endptr != NULL && *strtol_endptr != 0) {
                    fprintf(stderr, "Base-Value is not a valid number.\n");
                    return 1;
                } else if (errno == ERANGE) {
                    perror("Base-value caused an overflow. The base needs to be between -127 and 127 and cannot be -1, 0 or 1.\n");
                    return 1;
                }
                break;
            case 'a':
                has_alphabet_parameter = true;
                alphabet = optarg;
                alphabet_len = strlen(alphabet);
                break;
            case 'o':
                if(strlen(optarg) > 1) {
                    fprintf(stderr, "Operator can only be +, - or * and not a string.\n");
                    return 1;
                }
                operation = *optarg;
                break;
            case 'h':
                printf(help);
                return 0;
            case '?':
                // error: -- sign probably not used
                fprintf(stderr, "Something went wrong. Please use only the specified operators mentioned in the help section.\n" 
                    "If you want to use negative numbers. Use double minus signs (--) in front of the negative number.");
                return 1;
        }
    }

    // If argc is less than 3 then z1 and z2 are unset
    // if(argc < 3) {
    //     fprintf(stderr, usage, argv[0]);
    //     return 1;
    // }

    // Check for inputs to be in correct ranges
    if (implementation_to_use < 0 || 2 < implementation_to_use){ // check if it's 0 or 1 or 2, otherwise it's invalid
        fprintf(stderr, "The specified implementation version does not exist. You can choose from the following versions: 0, 1 and 2. Default version is 0.\n");
        return 1;
    }
    
    // Check for valid base
    size_t abs_base = abs(base);
    if (abs_base <= 1 || 127 < abs_base) {
        fprintf(stderr, "The base needs to be between -127 and 127 and cannot be -1, 0 or 1. Default base value is 10.\n");
        return 1;
    }
    
    // Check for valid operation
    if (operation != '+' && operation != '-' && operation != '*') {
        fprintf(stderr, "This application only supports addition (+), subtraction (-) and multiplication (*). Default operation is (+).\n");
        return 1;
    }

    // Check for valid alphabet
    if(has_alphabet_parameter) {
        if(abs_base <= 10) {
            fprintf(stderr, "Custom alphabet not allowed when base parameter is between -10 and -2 or between 2 and 10.\n");
            return 1;
        } else {
            // Check for unallowed alphabet characters
            for(size_t i = 0; i < alphabet_len; i++) {
                if(alphabet[i] == '-') {
                    fprintf(stderr, "Alphabet may not contain the '-' (hyphen) character.");
                    return 1;
                } else if (alphabet[i] < 0 || 127 < alphabet[i]) {
                    fprintf(stderr, "Alphabet may not contain characters outside the ascii-128 table.");
                    return 1;
                }
            }
            
            // Check for duplicates
            for(size_t i = 0; i < alphabet_len - 1; i++) {
                for(size_t k = i + 1; k < alphabet_len; k++) {
                    if(alphabet[i] == alphabet[k]) {
                        errno = EINVAL;
                        fprintf(stderr, "Alphabet may not contain the same character twice. Character that appeared twice: %c\n", alphabet[i]);
                        return 1;
                    }
                }
            }
            
            // Absolute value of base is > 10 and the alphabet has been set
            size_t strlen_alphabet = strlen(alphabet);
            if(alphabet_len != abs_base) {
                fprintf(stderr, "The alphabet (currently %ld characters long) needs to be the exactly as long as the absolute value of the base (%ld).\n", strlen_alphabet, abs_base);
                return 1;
            }
        }
    // Generate alphabet if possible
    } else {
        if(abs_base > 10) {
            fprintf(stderr, "Alphabet required when base parameter is greater than 10 or less than -10.\n");
            return 1;
        } else { // If base is <= 10, then we just create the alphabet ourselves
            alphabet_len = abs_base;
            alphabet = malloc((alphabet_len + 1) * sizeof(char)); // All base numbers + '\0'
            if(!alphabet) {
                fprintf(stderr, "Couldn't allocate memory for alphabet.\n");
                return 1;
            }

            for(unsigned char i = 0; i < alphabet_len; i++) {
                alphabet[i] = '0' + i;
            }
        }
    }

    // This handles input numbers after the options
    for(int index = optind; index < argc; index++) {
        if(!z1) {
            z1 = argv[index];
        }
        else if(!z2) {
            z2 = argv[index];
        }
        else {
            fprintf(stderr, "Too many arguments.\n");
            fprintf(stderr, "%s\n", argv[index]);
            printf("%s, %s\n", z1, z2);
            return 1;
        }
    }
    
    // None or only one number was provided
    if(!z1 || !z2) {
        fprintf(stderr, "Missing input strings for at least one of the two required numbers.\n");
        return 1;
    }

    // Disallow minus signs on negative bases
    if(base < 0) {
        if(z1[0] == '-' || z2[0] == '-') {
            errno = EINVAL;
            perror("When using negative bases, '-' are not allowed to denote negative numbers.");
            return 1;
        }
    }
    
    // multiple steps at once:
    // 1. create lookup table for the value of a character in the alphabet: the character determines the index, the value in the table is the position of the character in the alphabet
    // 2. check if z1 or z2 are negative numbers
    // 3. remove leading zeros for both numbers
    // 4. check if z1 and z2 only use valid characters from the alphabet
    // 5. while performing step 4, check which number is bigger
    // 6. while performing step 4, determine the length of both numbers

    // step 1: create lookup table for the value of a character in the alphabet: the character determines the index, the value in the table is the position of the character in the alphabet
    char lookup[255] = {-1};  // fill the array with -1
    for (int i = 0; alphabet[i] != 0; i++) {
        lookup[alphabet[i]] = i;
    }
    
    // step 2: check if z1 or z2 are negative numbers
    bool z1_signed = z1[0] == '-';
    bool z2_signed = z2[0] == '-';
    
    // skip the minus sign
    z1 += z1_signed;
    z2 += z2_signed; 
    
    // edge case: z1 is just -
    if (z1[0] == 0) {
        fprintf(stderr, "Z1 is just a minus sign. Please provide an actual number.\n");
        return 1;
    }
    
    // edge case: z2 is just -
    if (z2[0] == 0) {
        fprintf(stderr, "Z2 is just a minus sign. Please provide an actual number.\n");
        return 1;
    }
    
    // step 3: remove leading zeros from z1
    char z1_current_character;
    
    // check if first character of z1 is not null and in alphabet and represents the value zero. If so ---> remove this leading zero by increasing the pointer of z1
    while ((z1_current_character = z1[0]) != 0) {
        if (strchr(alphabet, z1_current_character) == NULL) { // check if z1_current_character is in alphabet
            fprintf(stderr, "Character %c of Z1 is not in the alphabet: %s\n", z1_current_character, alphabet);
            return 1;
        }
        
        if (lookup[z1_current_character] == 0) {
            z1++;
        } else {
            break;
        }
    }
    
    // edge case: z1 is zero
    if (z1[0] == 0) {
        z1--;
    }
    

    // step 3: remove leading zeros from z2
    char z2_current_character;
    
    // check if first character of z2 is not null and in alphabet and represents the value zero. If so ---> remove this leading zero by increasing the pointer of z2
    while ((z2_current_character = z2[0]) != 0) {
        if (strchr(alphabet, z2_current_character) == NULL) { // check if z2_current_character is in alphabet
            fprintf(stderr, "Character %c of Z2 is not in the alphabet: %s\n", z2_current_character, alphabet);
            return 1;
        }
        
        if (lookup[z2_current_character] == 0) {
            z2++;
        } else {
            break;
        }
    }
    
    // edge case: z2 is zero
    if (z2[0] == 0) {
        z2--;
    }

    // step 4: check if z1 and z2 only use valid characters from the alphabet + check which number is bigger + determine the length of the numbers
    int z1_is_largest_absolute = 0; // 0 = z1 equals z2, -1 = z2 is largest, 1 = z1 is largest based on the absolute values of z1 compared to z2
    size_t z1_len = 0, z2_len = 0; // not counting the optional minus sign
    int z1_current_index = 0, z2_current_index = 0;

    // run until both numbers are at the end, using bitwise or is important here to always update the z2_current_character
    while ((z1_current_character = z1[z1_current_index]) != 0 | (z2_current_character = z2[z2_current_index]) != 0) {
        if (z1_current_character != 0) {
            if (strchr(alphabet, z1_current_character) != NULL) { // check if z1_current_character is in alphabet
                z1_current_index++;
                z1_len++;
            } else {
                fprintf(stderr, "Character %c of Z1 is not in the alphabet: %s\n", z1_current_character, alphabet);
                return 1;
            }
        }
        
        if (z2_current_character != 0) {
            if (strchr(alphabet, z2_current_character) != NULL) { // check if z2_current_character is in alphabet
                z2_current_index++;
                z2_len++;
            } else {
                fprintf(stderr, "Character %c of Z2 is not in the alphabet: %s\n", z2_current_character, alphabet);
                return 1;
            }
        }

        if (z1_is_largest_absolute == 0 && lookup[z1_current_character] != lookup[z2_current_character]) {
            if (lookup[z1_current_character] > lookup[z2_current_character]) {
                z1_is_largest_absolute = 1;
            } else {
                z1_is_largest_absolute = -1;
            }
        }
    }
    
    // if one number has more digits than the other then this number is larger (based on the absolute values)
    if (z1_len > z2_len) {
        z1_is_largest_absolute = 1;
    } else if (z1_len < z2_len) {
        z1_is_largest_absolute = -1;
    }
    
    // calculate necessary result_len (the actual result can use less space in some scenarios)
    size_t result_len;
    bool minus_sign_necessary_for_result;
    switch (operation) {
        case '+':
            minus_sign_necessary_for_result = (z1_signed && z2_signed) || (z1_signed && z1_is_largest_absolute > 0) || (z2_signed && z1_is_largest_absolute < 0);
            result_len = z1_len >= z2_len ? z1_len + 1 + minus_sign_necessary_for_result : z2_len + 1 + minus_sign_necessary_for_result;
            break;
        case '-': 
            minus_sign_necessary_for_result = (z1_signed && !z2_signed) || (z1_signed && z1_is_largest_absolute > 0) || (!z2_signed && z1_is_largest_absolute < 0);
            result_len = z1_is_largest_absolute > 0 ? z1_len + minus_sign_necessary_for_result : z2_len + minus_sign_necessary_for_result; 
            break;
        case '*': 
            minus_sign_necessary_for_result = (z1_signed && !z2_signed) || (!z1_signed && z2_signed);
            result_len = z1_len + z2_len + minus_sign_necessary_for_result; 
            break;
    }
    
    if (base < 0) {
        result_len *= abs_base * 10;
    }

    // print out the used parameters for the calculation
    printf("Base: %d\n", base);
    printf("Alphabet: %s\n", alphabet);
    printf("Operation: %c\n", operation);
    
    if (z1_signed) {
        printf("Z1: -%s\n", z1);
    } else {
        printf("Z1: %s\n", z1);
    }
    
    if (z2_signed) {
        printf("Z2: -%s\n", z2);
    } else {
        printf("Z2: %s\n", z2);
    }
    
    // allocate memory for result string
    char* result = calloc((result_len + 1), sizeof(char));
    if (!result) {
        fprintf(stderr, "Couldn't allocate memory for the result.\n");
        return 1;
    }
    
    char* result_start = result;

    if (has_performance_parameter){ // User wants to print runtime 
        switch(implementation_to_use){
            case 0:;
                struct timespec start1;
                clock_gettime(CLOCK_MONOTONIC, &start1);
                for (int i = 0; i<iterations;i++){
                    arith_op_any_base(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                }
                struct timespec end1; 
                clock_gettime(CLOCK_MONOTONIC, &end1);
                double time1 = end1.tv_sec - start1.tv_sec + 1e-9 * (end1.tv_nsec - start1.tv_nsec);
                double avg1 = time1/iterations;
                printf("Implementation: Lookup table version\n");
                printf("Iterations: %d\n", iterations);
                printf("Runtime: %lf\n", time1);
                printf("Average runtime: %lf\n", avg1);
                break;
            case 1:;
                struct timespec start;
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (int i = 0; i < iterations; i++){
                    arith_op_any_base_V1(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                }
                struct timespec end; 
                clock_gettime(CLOCK_MONOTONIC, &end);
                double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
                double avg = time/iterations;
                printf("Implementation: Basic version\n");
                printf("Iterations: %d\n", iterations);
                printf("Runtime: %lf\n", time);
                printf("Average runtime: %lf\n", avg);
                break;
            case 2:; 
                struct timespec start2;
                clock_gettime(CLOCK_MONOTONIC, &start2);
                for (int i = 0; i < iterations; i++){
                    arith_op_any_base_V2(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                }
                struct timespec end2; 
                clock_gettime(CLOCK_MONOTONIC, &end2);
                double time2 = end2.tv_sec - start2.tv_sec + 1e-9 * (end2.tv_nsec - start2.tv_nsec);
                double avg2 = time2/iterations;
                printf("Implementation: Convert-to-Binary version\n");
                printf("Iterations: %d\n", iterations);
                printf("Runtime: %lf\n", time2);
                printf("Average runtime: %lf\n", avg2);
                break;
        }
    } else { 
        switch(implementation_to_use){
            case 0:
                arith_op_any_base(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                break;
            case 1:
                arith_op_any_base_V1(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                break;
            case 2:
                arith_op_any_base_V2(base, alphabet, z1, z1_len, z1_signed, z2, z2_len, z2_signed, operation, result, result_len, z1_is_largest_absolute);
                break;
        }

        for(int i = 0; i < result_len + 1; i++) {
            printf("%c", result[i]);
        }
        printf("\n");
    }
    
    
    // free arrays
    free(result);
    
    if (abs_base <= 10) {
        free(alphabet);
    }
    return 0;
}

void arith_op_any_base(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute) {
    
    char lookup_table_alphabet_values[255] = {-1};  // fill the array with -1
    for (int i = 0; alph[i] != 0; i++) {
        lookup_table_alphabet_values[alph[i]] = i;
    }
    
    bool is_result_negative = false;
    
    switch (op) {
        case '+':
            if (z1_signed && z2_signed) {
                add_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                is_result_negative = true;
            } else if (!z1_signed && !z2_signed) {
                add_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
            } else if (z1_signed && !z2_signed) {
                
                // z1 == z2 && z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base(base, alph, z2, z2_len, z1, z1_len, result, result_len, lookup_table_alphabet_values);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                        is_result_negative = true;
                    } else {
                        sub_any_base(base, alph, z2, z2_len, z1, z1_len, result, result_len, lookup_table_alphabet_values);
                    }
                }
            } else {
                // z1 == z2 && !z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                    } else {
                        sub_any_base(base, alph, z2, z2_len, z1, z1_len, result, result_len, lookup_table_alphabet_values);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '-':
            if ((z1_signed && !z2_signed) || (!z1_signed && z2_signed)) {
                add_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                is_result_negative = true;
            } else if (z1_signed && z2_signed) {
                // z1 == z2 && z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (z1_is_largest_absolute > 0) {
                    sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                    is_result_negative = true;
                } else {
                    sub_any_base(base, alph, z2, z2_len, z1, z1_len, result, result_len, lookup_table_alphabet_values);
                }
            } else {
                // z1 == z2 && !z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                } 

                
                if (base < -1) {
                    sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
                    } else {
                        sub_any_base(base, alph, z2, z2_len, z1, z1_len, result, result_len, lookup_table_alphabet_values);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '*':
            mult_any_base(base, alph, z1, z1_len, z2, z2_len, result, result_len, lookup_table_alphabet_values);
            is_result_negative = (z1_signed && !z2_signed) || (!z1_signed && z2_signed);
            break;
        default:
            break;
    }
    
    
    // remove leading zero, edge case: result = 0
    bool is_result_zero = false;
    for (int i = 0; i < result_len; i++) {
        if (result[i] != 0 && result[i] != alph[0]) {
            break;
        }
        
        if (result[i] == alph[0] && result[i + 1] != 0) {
            result[i] = 0;
        }
        
        if (i == result_len - 1 && result[i] == alph[0]) {
            is_result_zero = true;
        }
    }
    
    // add minus sign if result is negative
    if (base > 1 && is_result_negative && !is_result_zero) {
        for (int i = 0; i < result_len; i++) {
            if (result[i] != 0) {
                result[i - 1] = '-';
                break;
            }
        }
    }
}

void add_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values) {
    // Swap so that the longer string is always in z1.
    if(z2_len > z1_len) {
        const char* temp = z1;
        size_t temp_len = z1_len;
        z1 = z2;
        z1_len = z2_len;
        z2 = temp;
        z2_len = temp_len;
    }

    char carry = 0;
    size_t result_index = result_len-1; 
    for(size_t i = 0; i < z1_len; i++) {
        // Go over the string from right to left
        char z1_char = z1[z1_len - i - 1];
        char z2_char = i >= z2_len ? z2_char = alph[0] : z2[z2_len - i - 1];
        
        // Optimization: Convert characters into their respective binary representations using the lookup table
        char z1_value = lookup_table_alphabet_values[z1_char];
        char z2_value = lookup_table_alphabet_values[z2_char];
        
        short addition = z1_value + z2_value + carry;

        // If carry is -1 and both values are 0, we need to "borrow" from the previous character
        if(carry == -1 && z1_value == 0 && z2_value == 0) {
            addition = abs(base)-1;
            carry = 1;
        } else {
            if(addition >= abs(base)) {
                addition -= abs(base);
                carry = (base > 0) ? 1 : -1;
            } else {
                carry = 0;
            }
        }

        // Convert back into alphabet character
        result[result_index--] = alph[addition];
    }
    // If we have read all characters and still have a carry, we need apply it.
    if(carry != 0) {
        if(carry == -1) {
            // Only do "borrow" when adding
            result[result_index--] = alph[abs(base)-1];
            result[result_index--] = alph[1];
        } else {
            result[result_index--] = alph[carry];
        }
    }
}

void sub_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values) {
    
    char carry = 0;
    bool char_sign = 1;
    size_t result_index = result_len-1; 
    size_t max_length = z1_len > z2_len ? z1_len : z2_len;
    for(size_t i = 0; i < max_length; i++) {
        char z1_char = i >= z1_len ? alph[0] : z1[z1_len - i - 1];
        char z2_char = i >= z2_len ? alph[0] : z2[z2_len - i - 1];

        // Optimization: Convert characters into their respective binary representations using the lookup table
        char z1_value = lookup_table_alphabet_values[z1_char];
        char z2_value = lookup_table_alphabet_values[z2_char];

        short subtraction = z1_value - z2_value - carry;

        if(base < 0 && carry == 1 && z1_value == 0 && z2_value == 0) {
            subtraction = abs(base)-1;
            carry = -1;
        } else {
            carry = 0;
            if(subtraction < 0) {
                subtraction += abs(base);
                carry = (base > 0) ? 1 : -1;
            } else if (subtraction >= abs(base)) {
                subtraction -= abs(base);
                carry = 1;
            }
        }
        result[result_index--] = alph[subtraction];
    }
    
    // If we have read all characters and still have a carry, we need to apply it.
    // Should only happen when base is < 0
    if(carry != 0) {
        if(carry == 1) {
            result[result_index--] = alph[abs(base)-1];
            result[result_index--] = alph[1];
        } else {
            result[result_index--] = alph[-carry];
        }
    }
}

void mult_any_base(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len, char* lookup_table_alphabet_values) {
    char carry_sign = base >= 0 ? 1 : -1;
    char mult_carry = 0;
    char add_carry = 0;
    size_t result_index_inner;
    for(size_t i = 0; i < z1_len; i++) {
        char z1_char = z1[z1_len - i - 1];
        char z1_val = lookup_table_alphabet_values[z1_char];
        
        result_index_inner = result_len - i - 1;
        size_t k = 0;
        for(; k < z2_len; k++) {
            char z2_char = z2[z2_len - k - 1];
            char z2_val = lookup_table_alphabet_values[z2_char];
            
            short multiplication = z1_val * z2_val + mult_carry;

            mult_carry = 0;
            
            if(multiplication >= abs(base)) {
                mult_carry = (multiplication/abs(base)) * carry_sign;
                multiplication = (multiplication % abs(base));
            }

            short addition = multiplication + result[result_index_inner] + add_carry;

            add_carry = 0;
            if(addition >= abs(base)) {
                add_carry = addition/abs(base) * carry_sign;
                addition = (addition % abs(base));
            }

            result[result_index_inner] = addition;
            result_index_inner--;
        }

        if(mult_carry != 0) {
            if(mult_carry < 0) {
                result[result_index_inner--] = abs(base)+mult_carry;
                result[result_index_inner] = 1;
            } else {
                result[result_index_inner] += mult_carry;
            }
            
            mult_carry = 0;
        }
        if(add_carry != 0) {
            if(add_carry == -1) {
                if(i == z1_len-1 && k == z2_len-1) {
                    result[result_index_inner--] = abs(base)-1;
                    result[result_index_inner] = 1;
                } else {
                    result[result_index_inner] -= 1;
                }

            } else {
                result[result_index_inner] += add_carry;
            }
            add_carry = 0;
        }
    }

    // Values are still in binary so we convert them now
    for(size_t i = 0; i < result_len; i++) {
        char val = result[i];
        result[i] = alph[val];
    }
}

void arith_op_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute) {
    
    bool is_result_negative = false;
    
    switch (op) {
        case '+':
            if (z1_signed && z2_signed) {
                add_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                is_result_negative = true;
            } else if (!z1_signed && !z2_signed) {
                add_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
            } else if (z1_signed && !z2_signed) {
                
                // z1 == z2 && z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base_V1(base, alph, z2, z2_len, z1, z1_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                        is_result_negative = true;
                    } else {
                        sub_any_base_V1(base, alph, z2, z2_len, z1, z1_len, result, result_len);
                    }
                }
            } else {
                // z1 == z2 && !z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                    } else {
                        sub_any_base_V1(base, alph, z2, z2_len, z1, z1_len, result, result_len);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '-':
            if ((z1_signed && !z2_signed) || (!z1_signed && z2_signed)) {
                add_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                is_result_negative = true;
            } else if (z1_signed && z2_signed) {
                // z1 == z2 && z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (z1_is_largest_absolute > 0) {
                    sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                    is_result_negative = true;
                } else {
                    sub_any_base_V1(base, alph, z2, z2_len, z1, z1_len, result, result_len);
                }
            } else {
                // z1 == z2 && !z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                } 

                
                if (base < -1) {
                    sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
                    } else {
                        sub_any_base_V1(base, alph, z2, z2_len, z1, z1_len, result, result_len);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '*':
            mult_any_base_V1(base, alph, z1, z1_len, z2, z2_len, result, result_len);
            is_result_negative = (z1_signed && !z2_signed) || (!z1_signed && z2_signed);
            break;
        default:
            break;
    }
    
    
    // remove leading zero, edge case: result = 0
    bool is_result_zero = false;
    for (int i = 0; i < result_len; i++) {
        if (result[i] != 0 && result[i] != alph[0]) {
            break;
        }
        
        if (result[i] == alph[0] && result[i + 1] != 0) {
            result[i] = 0;
        }
        
        if (i == result_len - 1 && result[i] == alph[0]) {
            is_result_zero = true;
        }
    }
    
    // add minus sign if result is negative
    if (base > 1 && is_result_negative && !is_result_zero) {
        for (int i = 0; i < result_len; i++) {
            if (result[i] != 0) {
                result[i - 1] = '-';
                break;
            }
        }
    }
}


void add_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len) {
    // Swap so that the longer string is always in z1.
    if(z2_len > z1_len) {
        const char* temp = z1;
        size_t temp_len = z1_len;
        z1 = z2;
        z1_len = z2_len;
        z2 = temp;
        z2_len = temp_len;
    }

    char carry = 0;
    size_t result_index = result_len-1; 
    for(size_t i = 0; i < z1_len; i++) {
        // Go over the string from right to left
        char z1_char = z1[z1_len - i - 1];
        char z2_char = i >= z2_len ? z2_char = alph[0] : z2[z2_len - i - 1];
        char z1_value = 0;
        char z2_value = 0;
        bool z1_set = false;
        bool z2_set = false;
        
        // Convert characters into their respective binary representations
        for(char a = 0; a < abs(base); a++) {
            if(alph[a] == z1_char) {
                z1_value = a;
                z1_set = true;
            }
            if(alph[a] == z2_char) {
                z2_value = a;
                z2_set = true;
            }
            if(z1_set && z2_set) break;
        }
        
        short addition = z1_value + z2_value + carry;

        // If carry is -1 and both values are 0, we need to "borrow" from the previous character
        if(carry == -1 && z1_value == 0 && z2_value == 0) {
            addition = abs(base)-1;
            carry = 1;
        } else {
            if(addition >= abs(base)) {
                addition -= abs(base);
                carry = (base > 0) ? 1 : -1;
            } else {
                carry = 0;
            }
        }

        // Convert back into alphabet character
        result[result_index--] = alph[addition];
    }
    // If we have read all characters and still have a carry, we need apply it.
    if(carry != 0) {
        if(carry == -1) {
            // Only do "borrow" when adding
            result[result_index--] = alph[abs(base)-1];
            result[result_index--] = alph[1];
        } else {
            result[result_index--] = alph[carry];
        }
    }
}


void sub_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len) {
    
    char carry = 0;
    bool char_sign = 1;
    size_t result_index = result_len-1; 
    size_t max_length = z1_len > z2_len ? z1_len : z2_len;
    for(size_t i = 0; i < max_length; i++) {
        char z1_char = i >= z1_len ? alph[0] : z1[z1_len - i - 1];
        char z2_char = i >= z2_len ? alph[0] : z2[z2_len - i - 1];
        char z2_value = 0;
        char z1_value = 0;
        bool z1_set = false;
        bool z2_set = false;
        
        // Convert characters into their respective binary representations
        for(char a = 0; a < abs(base); a++) {
            if(alph[a] == z1_char) {
                z1_value = a;
                z1_set = true;
            }
            if(alph[a] == z2_char) {
                z2_value = a;
                z2_set = true;
            }
            if(z1_set && z2_set) break;
        }

        short subtraction = z1_value - z2_value - carry;

        if(base < 0 && carry == 1 && z1_value == 0 && z2_value == 0) {
            subtraction = abs(base)-1;
            carry = -1;
        } else {
            carry = 0;
            if(subtraction < 0) {
                subtraction += abs(base);
                carry = (base > 0) ? 1 : -1;
            } else if (subtraction >= abs(base)) {
                subtraction -= abs(base);
                carry = 1;
            }
        }
        result[result_index--] = alph[subtraction];
    }
    
    // If we have read all characters and still have a carry, we need to apply it.
    // Should only happen when base is < 0
    if(carry != 0) {
        if(carry == 1) {
            result[result_index--] = alph[abs(base)-1];
            result[result_index--] = alph[1];
        } else {
            result[result_index--] = alph[-carry];
        }
    }
}


void mult_any_base_V1(int base, const char* alph, const char* z1, size_t z1_len, const char* z2, size_t z2_len, char* result, size_t result_len) {
    char carry_sign = base >= 0 ? 1 : -1;
    char mult_carry = 0;
    char add_carry = 0;
    size_t result_index_inner;
    for(size_t i = 0; i < z1_len; i++) {
        char z1_char = z1[z1_len - i - 1];
        char z1_val;
        for(char a = 0; a < abs(base); a++) {
            if(alph[a] == z1_char) {
                z1_val = a;
                break;
            }
        }
        
        result_index_inner = result_len - i - 1;
        size_t k = 0;
        for(; k < z2_len; k++) {
            char z2_char = z2[z2_len - k - 1];
            char z2_val;
            for(char a = 0; a < abs(base); a++) {
                if(alph[a] == z2_char) {
                    z2_val = a;
                    break;
                }
            }
            
            short multiplication = z1_val * z2_val + mult_carry;

            mult_carry = 0;
            
            if(multiplication >= abs(base)) {
                mult_carry = (multiplication/abs(base)) * carry_sign;
                multiplication = (multiplication % abs(base));
            }

            short addition = multiplication + result[result_index_inner] + add_carry;

            add_carry = 0;
            if(addition >= abs(base)) {
                add_carry = addition/abs(base) * carry_sign;
                addition = (addition % abs(base));
            }

            result[result_index_inner] = addition;
            result_index_inner--;
        }

        if(mult_carry != 0) {
            if(mult_carry < 0) {
                result[result_index_inner--] = abs(base)+mult_carry;
                result[result_index_inner] = 1;
            } else {
                result[result_index_inner] += mult_carry;
            }
            
            mult_carry = 0;
        }
        if(add_carry != 0) {
            if(add_carry == -1) {
                if(i == z1_len-1 && k == z2_len-1) {
                    result[result_index_inner--] = abs(base)-1;
                    result[result_index_inner] = 1;
                } else {
                    result[result_index_inner] -= 1;
                }

            } else {
                result[result_index_inner] += add_carry;
            }
            add_carry = 0;
        }
    }

    // Values are still in binary so we convert them now
    for(size_t i = 0; i < result_len; i++) {
        char val = result[i];
        result[i] = alph[val];
    }
}





void arith_op_any_base_V2(int base, const char* alph, const char* z1, size_t z1_len, bool z1_signed, const char* z2, size_t z2_len, bool z2_signed, char op, char* result, size_t result_len, int z1_is_largest_absolute) {
    char* z1_vals = malloc(z1_len);
    if(!z1_vals) {
        fprintf(stderr, "Couldn't allocate array\n");
        exit(1);
    }
    
    char* z2_vals = malloc(z2_len);
    if(!z2_vals) {
        fprintf(stderr, "Couldn't allocate array\n");
        exit(1);
    }
    
    // Convert to binary
    for(size_t i = 0; i < z1_len; i++) {
        for(char a = 0; a < abs(base); a++) {
            if(z1[i] == alph[a]) {
                z1_vals[i] = a;
                break;
            }
        }
    }
    
    for(size_t i = 0; i < z2_len; i++) {
        for(char a = 0; a < abs(base); a++) {
            if(z2[i] == alph[a]) {
                z2_vals[i] = a;
                break;
            }
        }
    }
    
    bool is_result_negative = false;
    
    switch (op) {
        case '+':
            if (z1_signed && z2_signed) {
                add_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                is_result_negative = true;
            } else if (!z1_signed && !z2_signed) {
                add_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
            } else if (z1_signed && !z2_signed) {
                
                // z1 == z2 && z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base_V2(base, alph, z2_vals, z2_len, z1_vals, z1_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                        is_result_negative = true;
                    } else {
                        sub_any_base_V2(base, alph, z2_vals, z2_len, z1_vals, z1_len, result, result_len);
                    }
                }
            } else {
                // z1 == z2 && !z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (base < -1) {
                    sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                    } else {
                        sub_any_base_V2(base, alph, z2_vals, z2_len, z1_vals, z1_len, result, result_len);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '-':
            if ((z1_signed && !z2_signed) || (!z1_signed && z2_signed)) {
                add_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                is_result_negative = true;
            } else if (z1_signed && z2_signed) {
                // z1 == z2 && z1_signed && z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                }
                
                if (z1_is_largest_absolute > 0) {
                    sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                    is_result_negative = true;
                } else {
                    sub_any_base_V2(base, alph, z2_vals, z2_len, z1_vals, z1_len, result, result_len);
                }
            } else {
                // z1 == z2 && !z1_signed && !z2_signed --> result is 0 
                if (z1_is_largest_absolute == 0) {
                    result[result_len - 1] = alph[0];
                    break;
                } 

                
                if (base < -1) {
                    sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                } else {
                    if (z1_is_largest_absolute > 0) {
                        sub_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
                    } else {
                        sub_any_base_V2(base, alph, z2_vals, z2_len, z1_vals, z1_len, result, result_len);
                        is_result_negative = true;
                    }
                }
            }
            break;
        case '*':
            mult_any_base_V2(base, alph, z1_vals, z1_len, z2_vals, z2_len, result, result_len);
            is_result_negative = (z1_signed && !z2_signed) || (!z1_signed && z2_signed);
            break;
        default:
            break;
    }
    
    // remove leading zero, edge case: result = 0
    bool is_result_zero = false;
    for (int i = 0; i < result_len; i++) {
        if (result[i] != 0 && result[i] != alph[0]) {
            break;
        }
        
        if (result[i] == alph[0] && result[i + 1] != 0) {
            result[i] = 0;
        }
        
        if (i == result_len - 1 && result[i] == alph[0]) {
            is_result_zero = true;
        }
    }
    
    // add minus sign if result is negative
    if (base > 1 && is_result_negative && !is_result_zero) {
        for (int i = 0; i < result_len; i++) {
            if (result[i] != 0) {
                result[i - 1] = '-';
                break;
            }
        }
    }
    
    free(z1_vals);
    free(z2_vals);
}


void add_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len) {
    if(z2_len > z1_len) {
        const char* temp = z1_vals;
        size_t temp_len = z1_len;
        z1_vals = z2_vals;
        z1_len = z2_len;
        z2_vals = temp;
        z2_len = temp_len;
    }

    char carry = 0;
    size_t result_index = result_len-1; 
    for(size_t i = 0; i < z1_len; i++) {
        char z1_value = z1_vals[z1_len - i - 1];
        char z2_value = i >= z2_len ? 0 : z2_vals[z2_len - i - 1];

        short addition;
        
        // If carry is -1 and both values are 0, we need to "borrow" from the previous character
        if(carry == -1 && z1_value == 0 && z2_value == 0) {
            addition = abs(base)-1;
            carry = 1;
        } else {
            // Normal addition
            addition = z1_value + z2_value + carry;
            if(addition >= abs(base)) {
                addition -= abs(base);
                carry = (base > 0) ? 1 : -1;
            } else {
                carry = 0;
            }
        }

        // Convert back into alphabet character
        result[result_index--] = alph[addition];
    }

    // If we have read all characters and still have a carry, we need apply it.
    if(carry != 0) {
        if(carry == -1) {
            result[result_index--] = alph[abs(base)-1];
            result[result_index] = alph[1];
        } else {
            result[result_index] = alph[carry];
        }
    }
}


void sub_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len) {
    char carry = 0;
    bool char_sign = 1;
    size_t result_index = result_len-1; 
    size_t max_length = z1_len > z2_len ? z1_len : z2_len;
    for(size_t i = 0; i < max_length; i++) {
        char z1_value = i >= z1_len ? 0 : z1_vals[z1_len - i - 1];
        char z2_value = i >= z2_len ? 0 : z2_vals[z2_len - i - 1];

        short subtraction = z1_value - z2_value - carry;
        
        // Normal subtraction
        if(base < 0 && carry == 1 && z1_value == 0 && z2_value == 0) {
            subtraction = abs(base)-1;
            carry = -1;
        } else {
            carry = 0;
            if(subtraction < 0) {
                subtraction += abs(base);
                carry = (base > 0) ? 1 : -1;
            } else if (subtraction >= abs(base)) {
                subtraction -= abs(base);
                carry = 1;
            }
        }
        
        result[result_index--] = alph[subtraction];
    }

    // If we have read all characters and still have a carry, we need apply it.
    if(carry != 0) {
        if(carry == 1) {
            result[result_index--] = alph[abs(base)-1];
            result[result_index] = alph[1];
        } else {
            result[result_index] = alph[-carry];
        }
    }
}


void mult_any_base_V2(int base, const char* alph, const char* z1_vals, size_t z1_len, const char* z2_vals, size_t z2_len, char* result, size_t result_len) {
    char carry_sign = base >= 0 ? 1 : -1;
    char mult_carry = 0;
    char add_carry = 0;
    size_t result_index_inner;
    for(size_t i = 0; i < z1_len; i++) {
        char z1_val = z1_vals[z1_len - i - 1];
        result_index_inner = result_len - i - 1;
        size_t j = 0;
        for(; j < z2_len; j++) {
            char z2_val = z2_vals[z2_len - j - 1];

            short multiplication = z1_val * z2_val + mult_carry;
            mult_carry = 0;
            
            if(multiplication >= abs(base)) {
                mult_carry = (multiplication/abs(base)) * carry_sign;
                multiplication = (multiplication % abs(base));
            }

            short addition = multiplication + result[result_index_inner] + add_carry;
            add_carry = 0;
            
            if(addition >= abs(base)) {
                add_carry = addition/abs(base) * carry_sign;
                addition = (addition % abs(base));
            }

            result[result_index_inner] = addition;
            result_index_inner--;
        }

        if(mult_carry != 0) {
            if(mult_carry < 0) {
                result[result_index_inner--] = abs(base)+mult_carry;
                result[result_index_inner] = 1;
            } else {
                result[result_index_inner] += mult_carry;
            }
            
            mult_carry = 0;
        }
        if(add_carry != 0) {
            if(add_carry == -1) {
                if(i == z1_len-1 && j == z2_len-1) {
                    result[result_index_inner--] = abs(base)-1;
                    result[result_index_inner] = 1;
                } else {
                    result[result_index_inner] -= 1;
                }

            } else {
                result[result_index_inner] += add_carry;
            }
            add_carry = 0;
        }
    }

    // Values are still in binary so we convert them now
    for(size_t i = 0; i < result_len; i++) {
        char val = result[i];
        result[i] = alph[val];
    }
}