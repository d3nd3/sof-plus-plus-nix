
using namespace std;
uint8_t AddChar(uint8_t x) {
	if (x<26) return static_cast<uint8_t>('A' + x);
	else if (x<52) return static_cast<uint8_t>('a' + (x-26));
	else if (x<62) return static_cast<uint8_t>('0' + (x-52));
	else if (x==62) return '+';
	else if (x==63) return '/';
	else return 0; // default value
}


vector<uint8_t> ValidateKey(string handoff, string in_key) {
	int handofflen = handoff.length();
	int in_keylen = in_key.length();
	uint8_t a, b, c, d;
	a = b = c = d = 0;
	vector<uint8_t> table(256);
	for (int i = 0; i < 256; i++) {
		table[i] = i;
	}
	if (handofflen > 6) {
		cout << "invalid handoff\n";
		exit(1);
	}
	for (int i = 0; i < 256; i++) {
		a = (a + table[i] + handoff[i % handofflen]) & 255;
		b = table[a];
		table[a] = table[i];
		table[i] = b;
	}
	a = 0;
	vector<uint8_t> key;
	for (int i = 0; i < in_keylen; i++) {
		key.push_back(in_key[i]);
		a = (a + key[i] + 1) & 255;
		b = table[a];
		c = (c + b) & 255;
		d = table[c];
		table[c] = b;
		table[a] = d;
		key[i] ^= table[(b + d) & 255];
	}
	in_keylen /= 3;
	int i = 0;
	vector<uint8_t> out_key;
	while (in_keylen > 0) {
		in_keylen -= 1;
		b = key[i];
		i += 1;
		d = key[i];
		i += 1;
		uint8_t c = AddChar(b >> 2);
		if ( c )
			out_key.push_back(c);
		c = AddChar(((b & 3) << 4) | (d >> 4));
		if ( c ) 
			out_key.push_back(c);
		b = key[i];
		i += 1;
		c = AddChar(((d & 15) << 2) | (b >> 6));
		if ( c )
			out_key.push_back(c);
		c = AddChar(b & 63);
		if ( c )
			out_key.push_back(c);
	}
	return out_key;
}