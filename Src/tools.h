/*********************************************************/
/*TOOLS.H - J. RABAIOTTI 001192249 2002-03		  		   */
/*FINAL YEAR PROJECT					  				            */
/*This file contains headers for general functions       */
/*Version 2.0 - Revised and Updated 2004-2005			   */
/*Copyright (c) J.Rabaiotti 2005						         */
/*********************************************************/


#include <NTL/ZZ.h>
#include <string>

#include "md5.h"
#include "defs.h"

using namespace std;


/*****************DATA STRUCTURES**********************/

// RSA Public Key structure
typedef struct {
	unsigned int id;
	unsigned int key_length;
    char owner_name[40];
    char owner_email[40];
    ZZ n;
	ZZ e;
	}RSAPubkey;

// RSA Secret Key structure
typedef struct {
	unsigned int id;
	unsigned int key_length;
    char owner_name[40];
    char owner_email[40];
    ZZ n;
	ZZ d;
	}RSASeckey;

// This structure is used in the RSA key generation process
typedef struct {
	ZZ n;
	ZZ e;
	ZZ d;
	unsigned int NumBits;
	}RSAPrimitive;

// This structure is the file header of an RSA key
// (the 3 characters at the beginning are an ID)
typedef struct {
      char t[3]; // file identifier ("KEY" for all key files)
      unsigned int id;
      char owner_name[40];
      char owner_email[40];
      unsigned int key_type;
      unsigned int key_length;
      unsigned int length_of_n;
      unsigned int length_of_e;
      unsigned int length_of_d;
      }RSAKeyHeader;


// This is an AES key structure. This also represents
// the structure of an AES key file.
typedef struct{
      char t[3]; // file identifier ("KEY" for all key files)
	  unsigned int id;
	  char owner_id[40];
	  unsigned int algorithm;
	  unsigned int size;
	  char key[32];
	  }SymKey;


// This is a file header (used for ciphertext files).
typedef struct{
	  char id[3]; // This is set to "CIP" for ciphertext files
	  unsigned int file_type;
      unsigned int encryption_key_ID;
      unsigned long number_of_blocks;
	  unsigned long size;
	  unsigned long crc;
	  char MD5SUM[16];
	  }file_header;

// This is a wrapper header, used to store the
// original filename of an encrypted file.
typedef struct{
	  char filename[255];
  	  long original_size;
	  unsigned int crc;
   	  char MD5[16];
	  }WRAPPER;


// This is a header for composite files, and it defines
// the size of the encrypted session key and the size of the
// ciphertext.
typedef struct{
      char t[3];
      unsigned int id;
	  unsigned int sym_alg_ID;
	  unsigned long key_size;
      unsigned long data_size;
      unsigned long key_crc;
      unsigned long data_crc;
      }composite_header;

typedef struct{
		char digest[16];
      }MD5STRUCT;

typedef struct{
		char t[3];
      char signing_key_name[40];
      unsigned long sig_size;
      unsigned long sig_crc;
      char signature[1000];
      }DIGITAL_SIGNATURE;


// This data type is used by the CRC function
typedef unsigned long int UNS_32_BITS;

/***COUNTING DIGITS***/
int digitcount(ZZ input); // Get number of digits in a ZZ

void PrintMD5(MD5STRUCT md);
/***CONVERSION BETWEEN ZZ AND BYTES, ZZ AND STRINGS, ETC***/

// Conversion between ZZ objects and character strings
char *to_char(ZZ input);
string to_string(ZZ& input);

// Converts a character to and from a string of its ASCII value
string char2ascii(char x);
char ascii2char(const string& x);

// Converts string objects two and from raw data
string bytes_to_string(char *data, int size);
string encode_ascii(const string& inputstring);
string encode_binary(const string& inputstring);
string decode_ascii(const string& asciistring);

// Special functions for key files
char *key_to_bytes(ZZ input);
ZZ key_from_bytes(char *input, size_t size);

/*** BLOCK FUNCTIONS ***/

// Add leading zeros to a block (a string object)
string AddLeadingZeros(string& input, int number_of_zeros);

// Pad a block to a given length
string MakeOutputBlock(const string& input, const int blocksize);

// Add leading zeros to align a block (in the form of a string object)
// by making sure its length is a multiple of 3.
string align(string& input);
string convert(string& input);

/*** FUNCTIONS FOR READING AND WRITING KEYS FROM FILES ***/

// RSA keys
RSAPubkey ReadPublicKey(BYTE *input);
RSAPubkey ReadPublicKey(FILE *input);
int WritePublicKey(FILE *output,RSAPubkey rk);
RSASeckey ReadSecretKey(FILE *input);
int WriteSecretKey(FILE *output,RSASeckey rs);

/*** TEST AND VERIFICATION ***/

// Check a given file name is valid
bool CheckFileName(char *fname);

// Check if a file exists
bool FileExists(char *fname);

// Check if a file conforms to various types
// i.e. is it encoded, or ciphertext, or a key
bool IsKey(char *fname);

// Error detection functions
unsigned long MemCRC(const char *buf, int length);
//MD5STRUCT GetMD5(char *filename);
MD5STRUCT GetBufferMD5(char *buffer, int length);
string GetMD5String(char *buffer, int length);
bool CompareMD5(MD5STRUCT a,MD5STRUCT b); // Compare 2 MD5 digests

/*** WRAPPING AND UNWRAPPING ***/
// These functions take a file's name as input, and write its data
// to an output file (tmpname) together with a header which specifies
// the original file's name. Unwrap removes the header and produces
// the original file as output.
int wrap(char *filename, char *filetitle, char *tmpname);
int unwrap(char *in_name,bool flag);
// 'flag' tells the function whether or not to ask the user before
//  unwrapping, and hence overwriting, a file with a name identical to a
//  file which already exists.

/*** ERRORS AND MESSAGES - DECODING ***/

// Produces a string corresponding to an error message.
string decode_error_message(unsigned int num);

// Produces a C string corresponding to a file type code
char *decode_file_type(unsigned int file_type);

// Produces a C string corresponding to an algorithm code
char *decode_algorithm(unsigned int algorithm);
