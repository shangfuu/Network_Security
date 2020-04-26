# Openssl to write a TLS server
## How to Run this code
Complie all subdirectory and generate CA, certificate and private key.
```
make
```
Clean all Pem, Object, CSR, SRL in the project
```
make clean
```

## How to create a self-signed SSL Certificate using OpenSSL
1. 先幫自己的 CA 生一組 key & certificate
```
# Generate RSA private key for CA using 2048 bits.
openssl genrsa -des3 -out ca.key 2048
```
```
# Generate certificate of CA
openssl req -x509 -new -nodes -key ca.key -subj "/C=TW/ST=Taiwan/L=Taipei City/O=B10615045/OU=NTUST/CN=localhost" -days 365 -out ca.crt
```
2. 幫自己的主機生一組 key & CSR
```
# Create RSA key pair for server with 2048 bits.
openssl genrsa -out host.key 2048
# Generate CSR for server using private key.
openssl req -new -key host.key -subj "/C=TW/ST=Taiwan/L=Taipei City/O=MyOrg/OU=MyUnit/CN=my.domain" -out host.csr
```
3. 用自己的主機簽 CSR 讓他變成 certificate
```
# Self-sign the CSR using CA certificate.
openssl x509 -req -days 365 -in host.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out host.crt
```

## HOWTO: Use Openssl C Library
### Installing Openssl Library
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
### Compiling your C Program with the Openssl library
While linking the program you need to provide the ssl and crypto library names. Following command should do it:
```
gcc yourprogram.c -lssl -lcrypto
```
### A few pointers on the do_crypt function
If you are going to use the do_crypt function for decrypting a text encrypted using electronic code book (ECB) mode, you should remove the following assert line since there is no Initialization Vector for ECB.
```
OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);
```

## Reference
https://developer.ibm.com/technologies/security/tutorials/l-openssl/
https://knowledge.broadcom.com/external/article/166370/how-to-create-a-selfsigned-ssl-certifica.html

