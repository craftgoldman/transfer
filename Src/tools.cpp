/***********************************************************************/
/* WinRSA v2.0 Copyright (c) J. RABAIOTTI 2005								  */
/* Arbitrary-length RSA implementation with additional functionality   */
/* This source file: general routines								   		  */
/* Revised and Additional Error Checking Implemented 24th June 2005	  */
/***********************************************************************/

#include "stdafx.h"
#include <string>
#include <NTL/zz.h>
#include "md5.h"
#include "tools.h"


// COUNTS THE DIGITS IN A ZZ ARBITRARY-LENGTH INTEGER
int digitcount(ZZ input)
{
	int count = 0;
	while(!IsZero(input))
	{
		input /= 10;
		count++;
	}
	return count;
}

// compare 2 RSA Data Security Inc. Message Digests
bool CompareMD5(MD5STRUCT a, MD5STRUCT b)
{
	bool x = TRUE;
   for(int i=0;i<16;i++)
   {
   	if(a.digest[i] != b.digest[i])
      {
      	x = FALSE;
         break;
      }
   }
   return x;
}

void PrintMD5(MD5STRUCT md)
{
	char buf[10];
   string str;
   int i;
   for(i=0;i<16;i++)
   {
   	sprintf(buf,"%02X",(unsigned char)md.digest[i]);
      str += buf;
      memset(buf,0,10);
   }
   //MessageBox(TopmostWindow(),str.c_str(),"MD5 Digest",MB_OK);
}

/*************************CONVERSION ROUTINES******************************/

string to_string(ZZ& input)
// Converts a ZZ to a string object
{
   // initialise two strings
   string one = "";
   string two = "";
   int i,j;
   // First, grab digits and concatenate them on to the string
   while(input>0)
   {
      j = input % 10; // Get digit from number
      j += 48; 		  // ASCII conversion
      one += (char)j; // Cast it to a char and concatenate onto the string
      input /= 10;    // Shorten the number (to get next digit)
   }
   // Now reverse the string
   for(i=(one.size() - 1);i>=0;i--)
   {
   	two += one[i];
   }
   return two;
}

string char2ascii(char x)
// convert a character to a string of its digits, i.e. "A" becomes "065"
{
	string buffer = "";
   int i1, i2, i3,newx;
   newx = (int)x;
   // adjust sign
   if(newx<0) newx = 256 + newx;
   i1 = newx % 10;
   newx /= 10;
   i2 = newx % 10;
   newx /= 10;
   i3 = newx % 10;
   buffer += (char)(i3 + 48);
   buffer += (char)(i2 + 48);
   buffer += (char)(i1 + 48);
   return buffer;
}

char ascii2char(const string& x)
// the reverse of the above procedure
{
	char y = 0;
   y += ((x[0]-48) * 100);
   y += ((x[1]-48) * 10);
   y += (x[2]-48);
   return y;
}

string bytes_to_string(char* data, int size)
// convert a block of data to a string of digits
{
	string buffer = "";
   for(int i=0;i<size;i++)
   {
   	buffer += char2ascii(data[i]);
   }
   return buffer;
}

string encode_ascii(const string& inputstring)
// encode a string as a string of its ASCII digits
{
	string buffer = "";
   for(int i=0;i<inputstring.size();i++)
   {
   	buffer += char2ascii(inputstring[i]);
   }
   return buffer;
}

string decode_ascii(const string& asciistring)
// decode a string of ASCII digits to a string of the characters they represent
{
	string buffer1 = "";
   string buffer2 = "";
   int length = asciistring.size() / 3;
   char x = 0;
   for(int i=0;i<length;i++)
   {
      buffer1 = "";
   	buffer1 += asciistring[(i*3)];
      buffer1 += asciistring[(i*3)+1];
      buffer1 += asciistring[(i*3)+2];
      x = ascii2char(buffer1);
      buffer2 += x;
   }
   return buffer2;
}

/***************************RSA KEY I/O FUNCTIONS*****************************/
// patched in from old WinRSA
char *key_to_bytes(ZZ input)
// converts a ZZ large integer to a block of bytes
{
   int i,j,k,l;
   k = digitcount(input);
   l = k - 1;
   char *output = (char *)calloc(k,sizeof(char));
   for(i=0;i<k;i++)
   {
   	j = input % 10;
      output[l] = j;
      l--;
      input /= 10;
  	}
   return output;
}

ZZ key_from_bytes(char *input, size_t size)
// converts a block of bytes to a ZZ
{
   ZZ output;
   int i,j;
   char *buffer;
   char buf2[3];
   buffer = (char *)calloc(size,sizeof(char));
   for(i=0;i<size;i++)
   {
   	j = (int)input[i];
    sprintf(buf2,"%i",j);
    strcat(buffer,buf2);
   }
   output = to_ZZ(buffer);
   free(buffer);
   return output;
}

string AddLeadingZeros(string& input, int number_of_zeros)
{
   string zerostring = "";
   int j;
	if(number_of_zeros<1) return input;
   else
   {
   	for(j=0;j<number_of_zeros;j++)
      {
      	zerostring += "0";
      }
      return zerostring + input;
   }
}

string align(string& input)
{
	int i = input.size();
   i %= 3;
   if (i == 0) return input;
   if (i == 1) return "00" + input;
   if (i == 2) return "0" + input;
}

string MakeOutputBlock(const string& input, const int blocksize)
{
   string rs = "";
   int i;
   for(i=0;i<(blocksize-input.size());i++)
   {
   	rs = rs + "0";
   }
   rs = rs + input;
   return rs;
}

string convert(string& input)
{
	int i,j;
   string temp = "";
   temp = align(input);
   i = temp.size();
   j = MAX_BLOCK_SIZE * 3;
   if (i < j)
   {
      return AddLeadingZeros(temp,j - i);
   }
   else return temp;
}
// FUNCTIONS FOR READING AND WRITING RSA KEYS

RSAPubkey ReadPublicKey(BYTE *input)
{
	RSAPubkey rk;
	RSAKeyHeader header;
	unsigned int i,j;
	char *e_buf, *n_buf;
	ZZ e,n;
	if(input==NULL)
	{
		rk.id = GENERAL_ERROR;
		return rk;
	}
	// Read in the file header
	memcpy(&header,input,sizeof(RSAKeyHeader));
	input += sizeof(RSAKeyHeader);

	if(header.key_type == RSA_PUBLIC_KEY)
	{
		rk.id = header.id;
		rk.key_length = header.key_length;
		strcpy(rk.owner_name,header.owner_name);
		strcpy(rk.owner_email,header.owner_email);
		i = header.length_of_n;
		j = header.length_of_e;
		n_buf = (char *)calloc(i,1);
		e_buf = (char *)calloc(j,1);
		memcpy(n_buf,input,i);
		input += i;
		memcpy(e_buf,input,j);
		n = key_from_bytes(n_buf,i);
		e = key_from_bytes(e_buf,j);
		memset(n_buf,0,i);
		memset(e_buf,0,j);
		free(n_buf);
		free(e_buf);
		rk.e = e;
		rk.n = n;
		e = 0;
		n = 0;
		return rk;
	}
	else
	{
		rk.id = GENERAL_ERROR;
		return rk;
	}
}

RSAPubkey ReadPublicKey(FILE *input)
{
   RSAPubkey rk;
   RSAKeyHeader header;
   unsigned int i,j,fr1,read;
   char *e_buf, *n_buf;
   ZZ e,n;
   if(input==NULL)
   {
   	//MsgPrintf(TopmostWindow(),"ReadPublicKey():%s\n",decode_error_message(ERR_NO_SUCH_FILE).c_str());
      rk.id = GENERAL_ERROR;
      return rk;
   }
   // Read in the file header
   fr1 = fread(&header,sizeof(RSAKeyHeader),1,input);
   if(fr1 != 1)
   {
   	//MsgPrintf(TopmostWindow(),"ReadPublicKey() fread error:%s\n",decode_error_message(ERR_CANT_READ).c_str());
      rk.id = GENERAL_ERROR;
      return rk;
   }
   if(header.key_type == RSA_PUBLIC_KEY)
   {
      rk.id = header.id;
      rk.key_length = header.key_length;
      strcpy(rk.owner_name,header.owner_name);
      strcpy(rk.owner_email,header.owner_email);
      i = header.length_of_n;
      j = header.length_of_e;
      n_buf = (char *)calloc(i,1);
      e_buf = (char *)calloc(j,1);
      read = fread(n_buf,i,1,input);
      if(read != 1)
      {
   		//MsgPrintf(TopmostWindow(),"ReadPublicKey():unable to read modulus n(fread() returned %i): %s\n",read,decode_error_message(ERR_CANT_READ).c_str());
         free(n_buf);
         free(e_buf);
         rk.id = GENERAL_ERROR;
         return rk;
   	}
      read = fread(e_buf,j,1,input);
      if(read != 1)
      {
   		//MsgPrintf(TopmostWindow(),"ReadPublicKey(): unable to read e (fread returned %i):%s\n",read,decode_error_message(ERR_CANT_READ).c_str());
         memset(n_buf,0,i);
         free(n_buf);
         free(e_buf);
         rk.id = GENERAL_ERROR;
         return rk;
   	}
      n = key_from_bytes(n_buf,i);
      e = key_from_bytes(e_buf,j);
      memset(n_buf,0,i);
      memset(e_buf,0,j);
      free(n_buf);
      free(e_buf);
      rk.e = e;
      rk.n = n;
      e = 0;
      n = 0;
      return rk;
   }
   else
   {
    	//MsgPrintf(TopmostWindow(),"ReadPublicKey():%s\n",decode_error_message(ERR_WRONG_KEY_TYPE).c_str());
      rk.id = GENERAL_ERROR;
      return rk;
   }
}

int WritePublicKey(FILE *output,RSAPubkey rk)
{
   ZZ e,n;
   char *e_buf,*n_buf;
   unsigned int e_length, n_length;
   RSAKeyHeader header;
   if(output==NULL)
   {
   	//MsgPrintf(TopmostWindow(),"WritePublicKey():%s\n",decode_error_message(ERR_NO_SUCH_FILE).c_str());
    return -1;
   }
   strcpy(header.t,"KEY");
   header.id = rk.id;
   header.key_type = RSA_PUBLIC_KEY;
   header.key_length = rk.key_length;
   strcpy(header.owner_name,rk.owner_name);
   strcpy(header.owner_email,rk.owner_email);
   e = rk.e;
   n = rk.n;
   e_length = digitcount(e);
   n_length = digitcount(n);
   header.length_of_n = n_length;
   header.length_of_e = e_length;
   header.length_of_d = 0;
   // write prepared header to file
   if(fwrite(&header,sizeof(RSAKeyHeader),1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WritePublicKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
		memset(e_buf,0,e_length);
		memset(n_buf,0,n_length);
	   	memset(&header,0,sizeof(RSAKeyHeader));
   		e = 0;
  		n = 0;
  		e_length = 0;
   		n_length = 0;
		return -1;
   }
   // convert E and N to bytes
   e_buf = (char*)key_to_bytes(e);
   n_buf = (char*)key_to_bytes(n);
   // always write N before E (or) D
   if(fwrite(n_buf,n_length,1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WritePublicKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
		memset(e_buf,0,e_length);
		memset(n_buf,0,n_length);
   		memset(&header,0,sizeof(RSAKeyHeader));
   		e = 0;
   		n = 0;
   		e_length = 0;
   		n_length = 0;
		return -1;
   }
   if(fwrite(e_buf,e_length,1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WritePublicKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
		memset(e_buf,0,e_length);
		memset(n_buf,0,n_length);
   		memset(&header,0,sizeof(RSAKeyHeader));
   		e = 0;
   		n = 0;
   		e_length = 0;
   		n_length = 0;
		return -1;
   }
	memset(e_buf,0,e_length);
	memset(n_buf,0,n_length);
	memset(&header,0,sizeof(RSAKeyHeader));
	e = 0;
	n = 0;
	e_length = 0;
	n_length = 0;
	return 0;
}

RSASeckey ReadSecretKey(FILE *input)
{
	RSASeckey rs;
   RSAKeyHeader header;
   unsigned int i,j;
   char *d_buf, *n_buf;
   ZZ d,n;
   if(input==NULL)
   {
   	//MsgPrintf(TopmostWindow(),"ReadSecretKey():%s\n",decode_error_message(ERR_NO_SUCH_FILE).c_str());
      rs.id = GENERAL_ERROR;
		return rs;
   }
   // Read in the file header
   if(fread(&header,sizeof(RSAKeyHeader),1,input) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"ReadSecretKey():%s\n",decode_error_message(ERR_CANT_READ).c_str());
      rs.id = GENERAL_ERROR;
		return rs;
   }
   if(header.key_type == RSA_SECRET_KEY)
   {
      rs.id = header.id;
      rs.key_length = header.key_length;
      strcpy(rs.owner_name,header.owner_name);
      strcpy(rs.owner_email,header.owner_email);
   	i = header.length_of_n;
      j = header.length_of_d;
      n_buf = (char *)calloc(i,1);
      d_buf = (char *)calloc(j,1);
      if(fread(n_buf,i,1,input) != 1)
      {
      	//MsgPrintf(TopmostWindow(),"ReadSecretKey():%s\n",decode_error_message(ERR_CANT_READ).c_str());
         memset(&header,0,sizeof(RSAKeyHeader));
         memset(&rs,0,sizeof(RSASeckey));
         free(n_buf);
         free(d_buf);
         rs.id = GENERAL_ERROR;
			return rs;
      }
      if(fread(d_buf,j,1,input) != 1)
      {
      	//MsgPrintf(TopmostWindow(),"ReadSecretKey():%s\n",decode_error_message(ERR_CANT_READ).c_str());
         memset(&header,0,sizeof(RSAKeyHeader));
         memset(n_buf,0,i);
         free(n_buf);
         free(d_buf);
         memset(&rs,0,sizeof(RSASeckey));
         rs.id = GENERAL_ERROR;
			return rs;
      }
      n = key_from_bytes(n_buf,i);
      d = key_from_bytes(d_buf,j);
      // clear memory for security
      memset(n_buf,0,i);
      memset(d_buf,0,j);
      memset(&header,0,sizeof(RSAKeyHeader));
      free(n_buf);
      free(d_buf);
      rs.d = d;
      rs.n = n;
      d = 0;
      n = 0;
      return rs;
   }
   else
   {
     //MsgPrintf(TopmostWindow(),"ReadSecretKey():%s\n",decode_error_message(ERR_WRONG_KEY_TYPE).c_str());
     rs.id = GENERAL_ERROR;
     return rs;
   }
}

int WriteSecretKey(FILE *output,RSASeckey rs)
{
	ZZ d,n;
   char *d_buf,*n_buf;
   unsigned int d_length, n_length;
	RSAKeyHeader header;
   if(output==NULL)
   {
   	//MsgPrintf(TopmostWindow(),"WriteSecretKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
		return -1;
   }
   strcpy(header.t,"KEY");
   header.id = rs.id;
   header.key_type = RSA_SECRET_KEY;
   header.key_length = rs.key_length;
   strcpy(header.owner_name,rs.owner_name);
   strcpy(header.owner_email,rs.owner_email);
   d = rs.d;
   n = rs.n;
   d_length = digitcount(d);
   n_length = digitcount(n);
   header.length_of_n = n_length;
   header.length_of_e = 0;
   header.length_of_d = d_length;
   // write prepared header to file
   if(fwrite(&header,sizeof(RSAKeyHeader),1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WriteSecretKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
      d = 0;
      n = 0;
		return -1;
   }
   // convert E and N to bytes
   d_buf = (char*)key_to_bytes(d);
   n_buf = (char*)key_to_bytes(n);
   // always write N before E (or) D
   if(fwrite(n_buf,n_length,1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WriteSecretKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
      memset(d_buf,0,d_length);
	   memset(n_buf,0,n_length);
   	free(n_buf);
   	free(d_buf);
		return -1;
   }
   if(fwrite(d_buf,d_length,1,output) != 1)
   {
   	//MsgPrintf(TopmostWindow(),"WriteSecretKey():%s\n",decode_error_message(ERR_CANT_WRITE).c_str());
      memset(d_buf,0,d_length);
   	memset(n_buf,0,n_length);
   	free(n_buf);
   	free(d_buf);
		return -1;
   }
   memset(d_buf,0,d_length);
   memset(n_buf,0,n_length);
   free(n_buf);
   free(d_buf);
   return 0;
}

bool CheckFileName(char *fname)
// checks a filename for validity against certain simple parameters
// it must not be null, contain less than one character or more than 255
{
   if(fname==NULL) return false;
	if(strlen(fname)<1) return false;
   if(strlen(fname)>255) return false;
   else return true;
}

// attempts to open a file in order to determine whether it exists
bool FileExists(char *fname)
{
	CStdioFile test;

	if (!test.Open(fname,CFile::modeRead))
	{
		return false;
	}
	else
	{
		test.Close();
		return true;
	}
}

bool IsKey(char *fname)
{
   FILE *fp;
   if(!CheckFileName(fname))
   {
  	  	//MsgPrintf(TopmostWindow(),"IsKey():%s\n",decode_error_message(ERR_INVALID_FNAME).c_str());
   	return false;
   }
   fp = fopen(fname,"rb");
   if(fp == NULL)
   {
  	  	//MsgPrintf(TopmostWindow(),"IsKey():%s\n",decode_error_message(ERR_CANT_OPEN).c_str());
   	return false;
   }
   if(getc(fp) != 'K')
   {
      fclose(fp);
   	return false;
   }
   if(getc(fp) != 'E')
   {
      fclose(fp);
   	return false;
   }
   if(getc(fp) != 'Y')
   {
      fclose(fp);
   	return false;
   }
   fclose(fp);
   return true;
}

MD5STRUCT GetBufferMD5(char *buffer, int length)
{
   int q;
   MD5STRUCT md;
   MD5_CTX mdContext;
   MD5Init(&mdContext);
   MD5Update(&mdContext,(unsigned char *)buffer,length);
   MD5Final(&mdContext);
   for(q=0;q<16;q++)
      md.digest[q] = mdContext.digest[q];
   return md;
}

string GetMD5String(char *buffer, int length)
{
   int q;
   char md_buf[10];
   string md5string;
   MD5_CTX mdContext;
   MD5Init(&mdContext);
   MD5Update(&mdContext,(unsigned char *)buffer,length);
   MD5Final(&mdContext);
   for(q=0;q<16;q++)
   {
      sprintf(md_buf,"%02X",(unsigned char)mdContext.digest[q]);
      md5string += md_buf;
      memset(md_buf,0,10);
   }
   return md5string;
}

string decode_error_message(unsigned int num)
{
	switch(num)
   {
   	case  ERR_INVALID_FNAME:
      	return "Invalid filename.";
		case	ERR_INVALID_FILE_TYPE:
      	return "Invalid file type.";
		case	ERR_NO_SUCH_FILE:
      	return "File does not exist.";
		case	ERR_CANT_OPEN:
      	return "Cannot open file.";
		case	ERR_MISC:
      	return "Miscellaneous error.";
		case	ERR_WRONG_KEY_TYPE:
      	return "Wrong key type for specified ciphertext.";
		case	ERR_INCORRECT_KEY:
      	return "Incorrect key.";
		case	ERR_SIGNATURE_UNRECOGNISED:
      	return "Digital signature unrecognised.";
		case	ERR_BAD_MD5SUM:
      	return "MD5 checksums do not match.";
      case  ERR_BAD_CRC:
      	return "Bad CRC.";
		case	ERR_CANT_READ:
      	return "Unable to read from file.";
		case	ERR_CANT_WRITE:
      	return "Unable to write to file.";
		case	ERR_CANT_CREATE:
      	return "File creation error.";
      case  ERR_INVALID_KEY:
      	return "Invalid key or corrupt key file.";
      case ERR_EMPTY_FILE:
      	return "File contains no data.";
      case ERR_POSSIBLE_CORRUPT_WRAPPER:
      	return "Decryption was successful, but the plaintext contains errors.";
      case ERR_WRAPPING_ERROR:
      	return "File could not be wrapped.";
      case ERR_CANT_DO_SESSION_KEY:
      	return "Composite mode session key could not be created.";
      case ERR_ALG_NOT_SUPPORTED:
      	return "The composite mode does not support the selected algorithm.";
      case ERR_CORRUPT_CIPHERTEXT:
      	return "The ciphertext appears to be corrupt.";
      case ERR_BAD_SIGNATURE:
      	return "The digital signature is corrupt.";
      case ERR_TRUNCATION_ERROR:
      	return "File could not be truncated.";
      case ERR_KEY_ERROR:
      	return "An error occurred during key processing. The key may be corrupt or invalid.";
      case ERR_ENCRYPTION_ERROR:
      	return "An error occurred during encryption. Encryption was aborted.";
      case ERR_DECRYPTION_ERROR:
      	return "An error occurred during decryption. Decryption was aborted.";
      case ERR_SIGNING_ERROR:
      	return "An error occurred during signing. Signing was aborted.";
      case ERR_VERIFY_ERROR:
      	return "An error occurred during signature verification. Verification was aborted.";
      case ERR_COMPOSITE_ERROR:
      	return "An error occurred during composite encryption or decryption. Operation was aborted.";
      case ERR_COMPRESSION_ERROR:
      	return "An error occurred during compression.";
      case ERR_DECOMPRESSION_ERROR:
      	return "An error occurred during decompression.";
      case GENERAL_ERROR:
      	return "A general error occurred.";
      default:
      	return "Unknown error.";
   }
}

char *decode_file_type(unsigned int file_type)
{
	switch(file_type)
   {
   	case RSA_PUBLIC_KEY:
      	return "RSA Public Key";
      case RSA_SECRET_KEY:
      	return "RSA Secret Key";
      case AES_KEY:
      	return "AES (Rijndael) Key";
      case RSA_CIPHERTEXT:
      	return "RSA Ciphertext";
      case AES_CIPHERTEXT:
      	return "AES Ciphertext";
      case COMPOSITE_CIPHERTEXT:
      	return "Composite Ciphertext";
      case SIGNATURE_FILE:
      	return "Digital Signature";
      default:
      	return "Unsupported file type";
   }
}

char *decode_algorithm(unsigned int algorithm)
{
 	switch(algorithm)
   {
   	case ALG_RSA:
      	return "RSA";
      case ALG_AES:
      	return "AES";
      case ALG_COMPOSITE:
      	return "COMPOSITE";
      default:
      	return "Unknown";
   }
}


/* Copyright (C) 1986 Gary S. Brown.  You may use this program, or
   code or tables extracted from it, as desired without restriction.*/

 /* Modified by Jeremy Bruestle, 2001 */
 /* Need an unsigned type capable of holding 32 bits; */

static UNS_32_BITS crc_32_tab[] = {     /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

unsigned long CRC(FILE *buf)
{
  if(buf == NULL)
  {
   	//MsgPrintf(TopmostWindow(),"CRC():%s\n",decode_error_message(ERR_NO_SUCH_FILE).c_str());
      return -1;
  }
  unsigned long crc = 0xffffffff;
  while(!feof(buf))
  {
  	crc = UPDC32 (getc(buf), crc);
  }
  return (crc);
}

unsigned long MemCRC(const char *buf,int length)
{
  int i;
  unsigned long crc = 0xffffffff;
  for(i=0;i<length;i++)
  {
  	crc = UPDC32 (buf[i], crc);
  }
   return (crc);
}

