/************************************************************************/
/* J. RABAIOTTI 001192249 - Computer Science Final Year Project 2002-03 */
/* Arbitrary-length RSA implementation with additional functionality    */
/* This source file: RSA algorithm routines							   		*/
/* Additional error-checking added 27 June 2005									*/
/************************************************************************/

#include "stdafx.h"
#include <string>
#include <NTL/ZZ.h>
#include "tools.h"

// an implementation of Euclid's extended algorithm
ZZ euclid_extended_d(ZZ e, ZZ phi)
{
	ZZ t1,t2,t3,u1,u2,u3,v1,v2,v3,q,result;

   q = 0;
   u1 = 1;
   u2 = 0;
   u3 = phi;
   v1 = 0;
   v2 = 1;
   v3 = e;
   t1 = 0;
   t2 = 0;
   t3 = 0;
   result = 0;
	while(v3 != 0)
   {
   	q = u3/v3;
   	t1 = u1 - (q*v1);
      t2 = u2 - (q*v2);
      t3 = u3 - (q*v3);
      u1 = v1;
      u2 = v2;
      u3 = v3;
      v1 = t1;
      v2 = t2;
      v3 = t3;
   }
   if(u2<0)
   {
   	result = u2 + phi;
   }
   else
   {
   	result = u2;
   }
   return result;
}

// similarly, an implementation of Euclid's extended algorithm
ZZ euclid_extended_e(ZZ N)
{
   ZZ e,gcd,u1,u2,u3,t1,t2,t3,v1,v2,v3,u,v,q;
	e = RandomLen_ZZ(200);
   gcd = 0;
   while(gcd != 1)
   {
   	u = N;
   	v = e;
	   q = 0;
	   u1 = 1;
	   u2 = 0;
	   u3 = u;
	   v1 = 0;
	   v2 = 1;
	   v3 = v;
	   t1 = 0;
	   t2 = 0;
	   t3 = 0;
	   while(v3 != 0)
	   {
	   	q = u3/v3;
	      t1 = u1 - (q*v1);
	      t2 = u2 - (q*v2);
	      t3 = u3 - (q*v3);
	      u1 = v1;
	      u2 = v2;
	      u3 = v3;
	      v1 = t1;
	      v2 = t2;
	      v3 = t3;
	   }
	   gcd = u3;
      if(gcd !=1) e++;
   }
   return e;
}

RSAPrimitive generate_numbers(long l)
{
   ZZ p,q,n,e,d,test = to_ZZ(0);
   RSAPrimitive RS;
   srand(time(NULL));
   SetSeed(to_ZZ((int)rand()*50));
   GenPrime(p,l);
   GenPrime(q,l);
   n = p * q;
   test = (p-1)*(q-1);
   e = euclid_extended_e(test);
   d = euclid_extended_d(e,test);
   RS.n = n;
   RS.e = e;
   RS.d = d;
   n = 0;
   e = 0;
   d = 0;
   return RS;
}

ZZ RSA_Encrypt(ZZ plaintext,RSAPubkey rp)
{
	ZZ ciphertext;
   ZZ error = to_ZZ("-1");
   if(rp.id == GENERAL_ERROR)
   {
   	//MsgPrintf(NULL,"RSA_Encrypt():%s",decode_error_message(ERR_KEY_ERROR).c_str());
      return error;
   }
   PowerMod(ciphertext,plaintext,rp.e,rp.n);
   return ciphertext;
}

ZZ RSA_Decrypt(ZZ ciphertext, RSASeckey rs)
{
   ZZ plaintext,error = to_ZZ("-1");
   if(rs.id == GENERAL_ERROR)
   {
   	//MsgPrintf(NULL,"RSA_Decrypt():%s",decode_error_message(ERR_KEY_ERROR).c_str());
      return error;
   }
	PowerMod(plaintext,ciphertext,rs.d,rs.n);
   return plaintext;
}

ZZ RSA_SigEncrypt(ZZ plaintext, RSASeckey rs)
{
	ZZ ciphertext,error=to_ZZ(-1);
   if(rs.id == GENERAL_ERROR)
   {
   	//MsgPrintf(NULL,"RSA_SigEncrypt():%s",decode_error_message(ERR_KEY_ERROR).c_str());
      return error;
   }
   PowerMod(ciphertext,plaintext,rs.d,rs.n);
   return ciphertext;
}

ZZ RSA_SigDecrypt(ZZ ciphertext, RSAPubkey rp)
{
	ZZ plaintext,error=to_ZZ(-1);
   if(rp.id == GENERAL_ERROR)
   {
   	//MsgPrintf(NULL,"RSA_SigDecrypt():%s",decode_error_message(ERR_KEY_ERROR).c_str());
      return error;
   }
   PowerMod(plaintext,ciphertext,rp.e,rp.n);
   return plaintext;
}

int RSA_GenerateKeyPair(char *filename1,char *filename2,char *owner_name,char *owner_email,unsigned int modulus_length)
{
  int sum;
  for(int i=0;i<strlen(owner_name);i++)
  {
  	sum += owner_name[i];
  }
  FILE *pubfile, *secfile;
  RSAPrimitive rk;
  RSAPubkey rp;
  RSASeckey rs;
  //randomize;
  int id = rand() + sum;
  rk = generate_numbers(modulus_length);
  rp.n = rk.n;
  rp.e = rk.e;
  rs.n = rk.n;
  rs.d = rk.d;
  rp.id = id;
  rs.id = id;
  strcpy(rp.owner_name,owner_name);
  strcpy(rp.owner_email,owner_email);
  strcpy(rs.owner_name,owner_name);
  strcpy(rs.owner_email,owner_email);
  rk.n = 0;
  rk.e = 0;
  rk.d = 0;
  pubfile = fopen(filename1,"w+b");
  if(pubfile==NULL)
  {
   	rp.n = 0;
      rs.n = 0;
      rp.e = 0;
      rs.d = 0;
      return ERR_CANT_OPEN;
  }
  secfile = fopen(filename2,"w+b");
  if(secfile==NULL)
  {
      fclose(pubfile);
  		rp.n = 0;
      rs.n = 0;
      rp.e = 0;
      rs.d = 0;
      return ERR_CANT_OPEN;
  }
  if(WritePublicKey(pubfile,rp)!=0)
  {
      fclose(pubfile);
      fclose(secfile);
  		rp.n = 0;
      rs.n = 0;
      rp.e = 0;
      rs.d = 0;
      return ERR_CANT_WRITE;
  }
  if(WriteSecretKey(secfile,rs)!=0)
  {
     fclose(pubfile);
     fclose(secfile);
  	  rp.n = 0;
     rs.n = 0;
     rp.e = 0;
     rs.d = 0;
     return ERR_CANT_WRITE;
  }
  fclose(pubfile);
  fclose(secfile);
  rp.n = 0;
  rs.n = 0;
  rp.e = 0;
  rs.d = 0;
  //memset(&rp,0,sizeof(RSAPubkey));
  //memset(&rs,0,sizeof(RSASeckey));
  //memset(&rk,0,sizeof(RSAPrimitive));
  return 0;
}

RSAPubkey RSA_GetPublicKeyFromFile(char *filename)
{
   RSAPubkey rp;
   FILE *pubfile = fopen(filename,"rb");
   if(pubfile == NULL)
   {
    	//MsgPrintf(NULL,"RSA_GetPublicKeyFromFile():%s",decode_error_message(ERR_CANT_OPEN).c_str());
      rp.id = GENERAL_ERROR;
      return rp;
   }
   else
   {
   	rp = ReadPublicKey(pubfile);
   	fclose(pubfile);
   	return rp;
   }
}

RSASeckey RSA_GetSecretKeyFromFile(char *filename)
{
   FILE *secfile;
   RSASeckey rs;
   secfile = fopen(filename,"rb");
   if(secfile == NULL)
   {
    	//MsgPrintf(NULL,"RSA_GetSecretKeyFromFile():%s",decode_error_message(ERR_CANT_OPEN).c_str());
      rs.id = GENERAL_ERROR;
      return rs;
   }
   rs = ReadSecretKey(secfile);
   fclose(secfile);
   return rs;
}
