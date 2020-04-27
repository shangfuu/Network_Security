SSL = openssl
pKey = CA-pKey.pem
CERT = CA-cert.pem

.PHONY: generateCA clean
generateCA: 
	# Generate RSA private key for CA using 2048 bits.
	$(SSL) genrsa -des3 -out $(pKey) 2048
	# Generate certificate of CA
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=CA" -days 365 -out $(CERT)

clean:
	@echo "Cleaning pem srl files..."
	-rm *.pem
	-rm *.srl
