# HOWTO: Use Openssl C Library
## Installing Openssl Library
```
sudo apt-get install libssl-dev
```
For example, you will want to include the following header files:
```
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
```
## Compiling your C Program with the Openssl library
While linking the program you need to provide the ssl and crypto library names. Following command should do it:
```
gcc yourprogram.c -lssl -lcrypto
```
## A few pointers on the do_crypt function
If you are going to use the do_crypt function for decrypting a text encrypted using electronic code book (ECB) mode, you should remove the following assert line since there is no Initialization Vector for ECB.
```
OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);
```
## How to create a self-signed SSL Certificate using OpenSSL
### Step 1: Generate a Private Key 
### Step 2: Generate a CSR (Certificate Signing Request) 
### Step 3: Generating a Self-Signed Certificate 
### Step 4: Convert the CRT to PEM format

## Reference
https://developer.ibm.com/technologies/security/tutorials/l-openssl/
https://knowledge.broadcom.com/external/article/166370/how-to-create-a-selfsigned-ssl-certifica.html

