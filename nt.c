#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_INPUT_LEN 64
#define MAX_BIT_WIDTH 32

/* The nt_info struct helps keep track of the conversion process and
   support a normalize representation.
*/
struct nt_info {
    char input[MAX_INPUT_LEN];
    int width;
    int print_len;
    int r0;
    int r1;
    unsigned int value;
    bool has_r_flag;
    bool has_b_flag;
};

/* Initialize an nt_info struct to default and known initial values. */
void nt_info_init(struct nt_info *nti)
{
    nti->input[0] = '\0';
    nti->width = MAX_BIT_WIDTH;
    nti->print_len = MAX_BIT_WIDTH;
    nti->r0 = 0;
    nti->r1 = 0;
    nti->value = 0;
    nti->has_r_flag = false;
    nti->has_b_flag = false; 

    return;
}

/* Print an in_info struct for debugging purposes. */
void nt_info_print(struct nt_info *nti)
{
    printf("input = %s\n", nti->input);
    printf("width = %d\n", nti->width);
    printf("print length = %d\n", nti->print_len);
    printf("r0    = %d\n", nti->r0);
    printf("r1    = %d\n", nti->r1);
    printf("value = %u\n", nti->value);

    return;
}

/* A helpful usage message. */
void print_usage(void)
{
    printf("usage: nt [-b width] [-r start,end] value\n");
    printf("  width is 4, 8, 16, 32 (default)\n");
    printf("  start is 0 to 31, end is 0 to 31\n");
    printf("  value can be binary, hexadecimal, unsigned or signed decimal.\n");

    return;
}

/* A helper method to check there is a "-b"  */
bool has_b_flag(int argc, char **argv, struct nt_info *nti)
{
    int i; 
    bool has_b = false;

    for( i = 0; i < argc; i++){
        if(strcmp(argv[i], "-b") == 0){
            has_b = true;
            nti->has_b_flag = true; 
        }
    }

    return has_b;
}

/* A helper method to check there is a "-r"  */
bool has_r_flag(int argc, char **argv, struct nt_info *nti)
{
    int i;
    bool has_r = false;

    for(i =0; i < argc; i++){
        if(strcmp(argv[i], "-r") == 0){
            has_r = true;
            nti->has_r_flag = true; 
        }
    }

    return has_r;
}

/* Set the value behine "-b" */
void set_b_value(int argc, char **argv, struct nt_info *nti)
{
    int i,j,b_value;
    for( i = 0; i < argc; i++){
        if(strcmp(argv[i], "-b") == 0){
            j = i;
            break;
        }
    } 
    b_value = atoi(argv[j+1]); 
    nti->print_len = b_value;

    return;
}

/* Set the value behine "-r" */
void set_r_value(int argc, char **argv, struct nt_info *nti)
{
    int i,j;
    for(i =0; i < argc; i++){
        if(strcmp(argv[i], "-r") == 0){
            j = i;
            break;
        }
    }
    
    int d = strlen(argv[j+1]);
    if(d == 3) {
      nti->r0 = atoi(&argv[j+1][0]);
      nti->r1 = atoi(&argv[j+1][2]);
    }
    else {
      nti->r0 = atoi(&argv[j+1][0]);
      nti->r1 = atoi(&argv[j+1][3]);
    }
  
    return;
}

/* Parse command line arguments. */
void parse_args(int argc, char **argv, struct nt_info *nti)
{

    if (argc < 2) {
        printf("Insufficient arguments.\n");
        print_usage();
        exit(-1);
    }

    if(has_b_flag(argc,argv,nti)){
        set_b_value(argc,argv,nti);

    } 

    if(has_r_flag(argc,argv,nti)){
            set_r_value(argc,argv,nti);
    } 

    else {
        nti->r0 = 0; 
        nti->r1 = nti->print_len ;
    }
    strcpy(nti->input, argv[argc - 1]);

    return;
}

/* Addition method for binary numbers.  */
void add_function(int *decimal,struct nt_info *nti)
{
    int i,j;
    int one[nti->print_len]; 
    int curry_carry = 0;
    int next_carry = 0;

    /* Initialize the array for "1". */
    for (i = 0; i < nti->print_len; i++) {
        if(i == 0) {
            one[i] = 1;
        } 

        else {
            one[i] = 0; 
        }

    }

    /* Adding. Three circumstances will be considered - '1+1',"0+0",and '1+0'. */
    for( i = 0; i < nti->print_len; i++){
        curry_carry = next_carry;

        // when '1+1' or '0+0'
        if(decimal[i] == one[i]){

            decimal[i] = 0;
            if(one[i] == 1){
                next_carry = 1;
            }
            if(decimal[i] == curry_carry){
                decimal[i] = 0;
            } else {
                decimal[i] = 1;
                next_carry = 0;
            }
        } 

        // when '1 + 0'
        else {           
            decimal[i] = 1;
            if(decimal[i] == curry_carry){
                decimal[i] = 0;
                next_carry = 1;
            } 

            else {
                decimal[i] = 1;
                next_carry = 0;
            }
        }
    }
}

/* Inverting '1' to '0' or '0' to '1'  */
void invert_function(int *decimal, struct nt_info *nti)
{
    int i;

    for(i = 0; i < nti->width; i++) {
        if(decimal[i] == 0) {
            decimal[i] = 1;
        } 

        else {
            decimal[i] = 0; 
        }
    }

}

/* Convert a decimal number to boinary number. */
void get_binary(int *decimal, unsigned int input, int size)
{
    int k = 0; 
    while(k < size){
        if((input & 0b1) == 1){
            decimal[k] = 1;
        } 

        else {
            decimal[k] = 0; 
        }

        input = input >> 1;  
        k += 1;  
    }
}

/* Check if a input is a decimial number.*/
bool is_decimal(char *s)
{
    return ((s[0] >= '0' && s[0] <= '9') || s[0] == '-');

}

/* Convert decimal input to a signed long long number */
signed long long converter(char *input){
    int i,j = 0;
    signed long long max = 0;
    int len = strlen(input);

    if(input[0] == '-') {
        j = 1;
    }

    for(i = j; i < len ; i++ ){
        max += (input[i] - 48) * pow(10, len - i - 1);

    }

    return max;

}

/* Check decimal input to see if it's out of range. */
void check_decimal(struct nt_info *nti)
{
    bool check = true;;
    signed long long positive_end = 4294967295;
    signed long long negative_end = -2147483648;
    signed long long result = converter(nti->input);

    if(nti->input[0] == '-'){
        if(result > (negative_end * -1)){
            check = false;
        }
    } 

    else {
        if(result > positive_end){
            check = false;
        }
    }

    if(!check){
        printf("Out of range! \n");
        exit(-1);
    }
}

/* Validating each position of a decimal input. */
void validate_decimal(struct nt_info *nti)
{
    int i;
    char c;
    bool valid = true;

    /* Call function. */
    check_decimal(nti);

    for(i =1; i < nti->width; i++){

        c = nti->input[i];
        if (c == '\0') {
            break;
        }

        if (!(c >='0' && c <= '9')){
            valid = false;
            break;
        }
    }

    if (!valid) {
        printf("Invalid decimal input: %s\n", nti->input);
        print_usage();
        exit(-1);
    }
        
    return;
}

/* Recalculate value when there is a range limit. */
void bit_teddling_decimal(int *s,struct nt_info *nti) {
    unsigned int v = 0;
    int i;
    int c;
    
    for (i = nti->r0; i <= nti->r1; i++) {
        c = s[i];
        v += c * (pow(2, i - nti->r0));
    }

    nti->value = v;
}

/* Convert a negative number to an unsigned number.*/
void convert_negative_decimal(struct nt_info *nti) 
{
    unsigned sum = 0;
    int i,len;
    unsigned int number;
    int decimal[nti->width]; 
    
    number = (unsigned int) converter(nti->input); 

    /* Convert decimal number to binary number */
    get_binary(decimal, number, nti->width);

    /* call inverting function */
    invert_function(decimal,nti);

    /* call adding function */
    add_function(decimal, nti);

    if(nti->has_r_flag) {
        bit_teddling_decimal(decimal,nti);

    } else {
        for(i = 0; i < nti->print_len ; i++){
            sum += (decimal[i] * pow(2, i));
        }    

        nti->value = sum; 
    }
}

/* Normalize a decimal number(negative or positive). */
void normalize_decimal(struct nt_info *nti) {

    /*If this is a positive number. */
    if(nti->input[0] != '-'){
        if(!nti->has_r_flag){
	  nti->value = (unsigned int) converter(nti->input);
        } 

        else {

            int s[nti->width];
            get_binary(s,(unsigned int)converter(nti->input),nti->width);
            bit_teddling_decimal(s,nti);
        }
        
    } 
    /* If this is a negative number. */
    else {
      
      convert_negative_decimal(nti);
      
    }
} 

/* Check if a input is a hexadicimal number.*/
bool is_hexa(char *s)
{
    return (s[0] == '0' && s[1] == 'x');
}

/* Validating a hexadecimal number. */
void validate_hexa(struct nt_info *nti)
{
    int i,j; 
    bool valid = false;
    int len;
    char c;
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                    '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    len = strlen(nti->input);

    if (len < 2) {
        valid = false;
    } 

    else {
        for (i = 2; i < nti->width; i++) {
            c = nti->input[i];
            if (c == '\0') {
                break;
            }

            for(j = 0; j < 16; j++){
                if( c == hex[j]){
                    valid = true;
                    break;
                }
            }
        }

        if (!valid) {
            printf("Invalid hexadecimal input: %s\n", nti->input);
            print_usage();
            exit(-1);
        }

        return;
    }
}

/* Normalize a hexadecimal number. */
void normalize_hexa(struct nt_info *nti)
{
    int i,j;
    int len = strlen(nti->input);
    int bit[nti->width];
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                    '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char c;
    unsigned sum = 0; 

    for(i = 2; i < len; i++ ){
        for(j = 0; j < 16; j++){
            if(nti->input[i] == hex[j]){
                sum += j * pow(16, len - i - 1);
            }
        }
    }
    // Checking if there is "-r". 
    if(nti->has_r_flag){
        get_binary(bit, sum, nti->width);
        bit_teddling_decimal(bit, nti);
    } 

    else {
       nti->value = sum;   
    }
}

/* Check binary number input */
bool is_binary(char *s)
{
    return (s[0] == '0' && s[1] == 'b');
}

/* Validate a binary string value. */
void validate_binary(struct nt_info *nti)
{
    int i;
    bool valid = true;
    int len;
    char c;

    len = strlen(nti->input);

    if (len < 3 || len > nti->width + 2) {
        valid = false;
    } else {
        for (i = 2; i < nti->width; i++) {
            c = nti->input[i+2];

            if (c == '\0') {
                break;
            }

            if (!(c == '0' || c == '1')) {
                valid = false;
                break;
            }
        }
    }

    if (!valid) {
        printf("Invalid binary input: %s\n", nti->input);
        print_usage();
        exit(-1);
    }

    return;
}

/* Convert a valid binary string into an unsigned int value. */
void normalize_binary(struct nt_info *nti)
{
    unsigned int v = 0;
    int start = 2; 
    int i;
    char c;
    int b;
    int len = strlen(nti->input);
   
    if(nti->has_r_flag) {
        start = len - 1 - nti->r1;
        len = len - nti->r0;
      
    }
  
    for (i = start; i < len; i++) {
        v = v << 1;
        c = nti->input[i];

        if (c == '0') {
            b = 0;
        } 
        else if (c == '1') {
            b = 1;
        }

        v = v | b;
    }

    nti->value = v;
   
}

/* Convert all valid input representations to the normalized form. */
void normalize_input(struct nt_info *nti)
{

    if (is_binary(nti->input)) {
        validate_binary(nti);
        normalize_binary(nti);
    }

    if (is_hexa(nti->input)) {
        validate_hexa(nti);
        normalize_hexa(nti);
    }

    else {
        if (is_decimal(nti->input) && !is_binary(nti->input)) {
            validate_decimal(nti);
            normalize_decimal(nti);
        }        
    }

    return;
}

/* Print binary value with space. */
void print_binary_value_in_number(struct nt_info *nti){
    int i,b;

    for (i = nti->print_len - 1; i >= 0; i--) {
        b = (nti->value >> i) & 0b1;

        printf("%d", b);
        if(i % 4 == 0 && i != 0){
            printf(" ");
        }
    }
    printf(" (base 2)\n");

    return;
}

/* Printing a binary number starting with '0b'. */
void print_binary_value_c_literal(struct nt_info *nti) {
    int i;
    int b;

    printf("0b");
    for (i = nti->print_len - 1; i >= 0; i--) {
        b = (nti->value >> i) & 0b1;

        printf("%d", b);
    }
    printf(" (base 2)\n");

    return;
}

/* Printing a hexadecimal number. */
void print_hexa(struct nt_info *nti)
{
    int i,j = 0;
    unsigned int quotient,temp;
    char hexNum[nti->width/4];
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                    '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    for(i = 0; i < nti->print_len/4; i++) {
        hexNum[i] = '0';
    }
    quotient = nti->value;
    
    while(quotient != 0){
        temp = quotient %16;

        hexNum[j++] = hex[temp];
        quotient /= 16;
    }
    printf("%s","0x");
    for(i = nti->print_len/4 - 1; i >= 0; i--) {
        printf("%c", hexNum[i]);
    }
    printf(" (base 16)\n");

    return;
}

/* Printing a base 10 unsigned number.*/
void print_unsigned_value(struct nt_info *nti)
{
    printf("%u (base 10 unsigned)\n", nti->value);
}

/* Printing a base 10 signed number.  */ 
void print_signed_value(struct nt_info *nti)
{
    int i,j,b;
    signed sum = 0;
    int arr[nti->width]; 

    for (i = nti->print_len - 1; i >= 0; i--) {
        b = (nti->value >> i) & 0b1;
        if(i == nti->print_len - 1){
            j = b;
        }
        arr[i] = b;
    }

    if (arr[nti->print_len - 1] == 1) {
        invert_function(arr,nti);
    }

    for(i = 0; i < nti->print_len; i++){
        sum += arr[i] * pow(2, i);
    }

    if(j == 1){
        printf("%c", '-');
        sum += 1;

    }
    printf("%u (base 10 signed)\n", sum);

    return;
}

/* One function to pull out all printing function. */
void print_conversions(struct nt_info *nti)
{
    print_binary_value_in_number(nti);
    print_binary_value_c_literal(nti);
    print_hexa(nti);
    print_unsigned_value(nti);
    print_signed_value(nti);

    return;
}

/* Main function. */
int main(int argc, char **argv)
{
    struct nt_info nti;

    nt_info_init(&nti);

    parse_args(argc, argv, &nti);

    normalize_input(&nti);

    print_conversions(&nti);

    return 0;
}