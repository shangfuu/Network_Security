SSL = openssl
pKey = CA-pKey.pem
CERT = CA-cert.pem

NAME = B10615045

.PHONY: gen clean
gen: 
	# Generate RSA private key for CA using 2048 bits.
	$(SSL) genrsa -des3 -out $(pKey) 2048
	# Generate certificate of CA
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=$(NAME)/OU=$(NAME)/CN=my.domain" -days 365 -out $(CERT)

clean:
	@echo "Cleaning pem files..."
	-rm *.pem
