prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/share/man

CFLAGS += -g --std=c99

.PHONY: all check clean

all: chars

check: chars
	./chars    < test-in.txt | diff -u test-out/standard.txt  -
	./chars -p < test-in.txt | diff -u test-out/printable.txt -
	./chars -s < test-in.txt | diff -u test-out/oneline.txt   -
	./chars -r < test-in.txt | diff -u test-out/raw.txt       -
	./chars -q < test-in.txt | diff -u test-out/quote.txt     -
	./chars -x < test-in.txt | diff -u test-out/hex.txt       -
	./chars -X < test-in.txt | diff -u test-out/hex-upper.txt -
	./chars -u < test-in.txt | diff -u test-out/decimal.txt   -

clean:
	rm -f chars
	rm -rf chars.dSYM

install: chars chars.1
	mkdir -p $(bindir) $(mandir)/man1
	install chars $(bindir)/chars
	install chars.1 $(mandir)/man1/chars.1

uninstall:
	rm -f $(bindir)/json-yaml
	rm -f $(mandir)/man1/json-yaml.1

# Run this to regenerate the test cases
test-out: chars
	mkdir -p test-out
	./chars test-in.txt -o test-out/standard.txt
	./chars test-in.txt -o test-out/printable.txt --printable
	./chars test-in.txt -o test-out/oneline.txt   --oneline
	./chars test-in.txt -o test-out/raw.txt       --raw
	./chars test-in.txt -o test-out/quote.txt     --quote
	./chars test-in.txt -o test-out/hex.txt       --hex
	./chars test-in.txt -o test-out/hex-upper.txt --Hex
	./chars test-in.txt -o test-out/decimal.txt   --decimal
