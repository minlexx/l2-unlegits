#ifndef BASE64_H
#define BASE64_H

typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

void base64_init_encodestate(base64_encodestate* state_in);
char base64_encode_value(char value_in);
int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in);
int base64_encode_blockend(char* code_out, base64_encodestate* state_in);

void base64_init_decodestate(base64_decodestate* state_in);
int base64_decode_value(char value_in);
int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);

void base64_encode_string( const char *str_in, int len_in, char *str_out );
void base64_decode_string( const char *str_in, int len_in, char *str_out );

#endif /* BASE64_H */

