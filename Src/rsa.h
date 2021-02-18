

// Generates a new random key
RSAPrimitive generate_numbers(long l);

// Main encryption functions
ZZ RSA_Encrypt(ZZ plaintext,RSAPubkey rp);
ZZ RSA_Decrypt(ZZ ciphertext, RSASeckey rs);

// Digital Signatures

ZZ RSA_SigEncrypt(ZZ plaintext, RSASeckey rs);
ZZ RSA_SigDecrypt(ZZ ciphertext, RSAPubkey rp);

int RSA_GenerateKeyPair(char *filename1,char *filename2,char *owner_name,char *owner_email,unsigned int modulus_length);
RSAPubkey RSA_GetPublicKeyFromFile(char *filename);
RSASeckey RSA_GetSecretKeyFromFile(char *filename);
