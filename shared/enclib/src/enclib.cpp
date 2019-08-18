#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "d3des.h"
#include "../inc/enclib.h"

__byte g_key[8] = { 0 };

__byte mapstr(__byte ch) {
	if (ch >= '0' && ch <= '9') {
		return ch - 48;
	}

	if (ch >= 'A' && ch <= 'F') {
		return ch - 55;
	}

	if (ch >= 'a' && ch <= 'f') {
		return ch - 87;
	}

	return 0;
}


int str2bin(__byte* buf, int len, const __byte* str, int slen) {
	int spos = 0;

	int i = 0;
	for (i = 0; i < len; i++) {
		spos = i * 2;
		if (0 >= (slen - spos)) {
			break;
		}
		else if (1 == (slen - spos)) {
			buf[i] = mapstr(str[spos]);
		}
		else {
			buf[i] = ((mapstr(str[spos]) << 4) | (mapstr(str[spos + 1])));
		}
	}
	return i;
}

int SetEncKey(const __byte* key) {
	memcpy(g_key, key, 8);
	srand((unsigned)time(NULL));
	return 1;
}

int Encrypt(__byte* buf, int len, const __byte* pwd, int plen) {
	
	// Uses 4 noise bits.
	int size = plen + 4;
	int apd = size % 8;
	if (apd)
	{
		apd = 8 - apd;
		size += apd;
	}

	// Thanks to the addition of one byte, the sprintf will overflow.
	// Binary->string, doubles the length.
	size = size * 2 + 1;

	__byte* tmp = new __byte[size];
	memset(tmp, 0, size);
	memcpy((tmp + 4), pwd, plen);

	__byte* ptr = tmp;
	
	// 4 noise bits.
	tmp[0] = (__byte)rand() & 0xFF;
	tmp[1] = (__byte)rand() & 0xFF;
	tmp[2] = (__byte)rand() & 0xFF;
	tmp[3] = (__byte)rand() & 0xFF;

	apd = (int)size / 2;
	for (int i = 4; i < apd;) {
		ptr[i] = ptr[0] ^ ptr[i];
		++i;
		ptr[i] = ptr[1] ^ ptr[i];
		++i;
		ptr[i] = ptr[2] ^ ptr[i];
		++i;
		ptr[i] = ptr[3] ^ ptr[i];
		++i;
	}

	deskey((__byte*)g_key, EN0);

	__byte* eptr = buf;
	__byte* dptr = tmp;

	for (;;) {
		des(dptr, eptr);
		dptr += 8;
		eptr += 8;
		if ((dptr - tmp) >= size) {
			break;
		}
	}

	memset(tmp, 0, size);
	ptr = tmp;
	apd = (int)(size / 2);
	for (int j = 0; j < apd; j++) {

		// Overwrite one byte at a time because the string ends with \0
		_snprintf_s((char*)ptr, 3, _TRUNCATE, "%02x", buf[j]);
		ptr += 2;
	}
	memcpy(buf, tmp, size);
	delete[] tmp;
	return size;
}

int Decrypt(__byte* buf, int len, const __byte* enc, int elen) {
	int size = (int)(elen / 2);
	if (elen % 2) {
		size += 1;
	}

	int apd = size % 8;
	if (apd) {
		apd = 8 - apd;
		size += apd;
	}

	__byte* tmp = new __byte[size];
	memset(tmp, 0, size);
	int bsize = str2bin(tmp, size, enc, elen);

	deskey((__byte*)g_key, DE1);

	__byte* dptr = buf;
	__byte* eptr = tmp;

	for (;;) {
		des(eptr, dptr);
		dptr += 8;
		eptr += 8;
		if ((eptr - tmp) >= bsize) {
			break;
		}
	}

	bsize -= apd;
	memset(tmp, 0, size);
	dptr = tmp;
	for (int i = 4; i < bsize;) {
		*dptr = buf[0] ^ buf[i];
		++dptr; ++i;
		*dptr = buf[1] ^ buf[i];
		++dptr; ++i;
		*dptr = buf[2] ^ buf[i];
		++dptr; ++i;
		*dptr = buf[3] ^ buf[i];
		++dptr; ++i;
	}
	memcpy(buf, tmp, bsize);
	delete[] tmp;
	return bsize;
}
