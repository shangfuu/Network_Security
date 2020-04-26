SSL = openssl
pKey = CA-pKey.pem
CERT = CA-cert.pem

.PHONY: gen clean
gen: 
	$(SSL) genrsa -des3 -out $(pKey) 2048
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=MyOrg/OU=MyUnit/CN=my.domain" -days 365 -out $(CERT)

clean:
	@echo "Cleaning pem files..."
	-rm *.pem
