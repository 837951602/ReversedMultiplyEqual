#include <stdio.h>
#include <algorithm>
#include <thread>
typedef unsigned long long u64;
typedef __uint128_t u128;
const int N = 11;
const int BYTEWIDTH = 5;
const u64 MEMLEN = 1 << 27;
constexpr u64 pow10(int k) {
	return k?10*pow10(k-1):1;
}
template<int k=N>
u64 reverse(u64 x, u64 t = 0) {
	return reverse<k-1>(x/10, t*10+x%10);
}
template<>
u64 reverse<0>(u64 x, u64 t) {
	return t;
}
struct Mem {
	u64 x;
	u128 m;
	void operator=(u64 t) {
		x = t;
		m = (u128)t * reverse(t);
	}
	bool try_apply(u64 t) {
		u64 u = reverse(t);
		if (u>t) return false;
		x = t;
		m = (u128)t * u;
		return true;
	}
	void write(FILE* fp) {
		fwrite(&x, BYTEWIDTH, 1, fp);
	}
	void read(FILE* fp) {
		u64 t = 0;
		fread(&t, BYTEWIDTH, 1, fp);
		*this = t;
	}
	bool operator<(const Mem& o) {
		return m < o.m;
	}
} *Storage = new Mem[MEMLEN], *StorPtr = Storage;

thread_local int packLen = 0;
void pack() {
	printf("Segment %d ready, time %ld\n", packLen, clock());
	std::sort(Storage, StorPtr);
	printf("Segment %d sortted, time %ld\n", packLen, clock());
	char str[64];
	sprintf(str, "Segment %d.bin", packLen++);
	FILE* fp = fopen(str, "wb");
	for (Mem* p = Storage; p != StorPtr; ++p) {
		p->write(fp);
	}
	fclose(fp);
	StorPtr = Storage;
	printf("Segment %d saved, time %ld\n", packLen, clock());
}

int main() {
	for (u64 i = pow10(N-1); i<pow10(N); ++i) {
		if (i%10 && StorPtr -> try_apply(i)) {
			++StorPtr;
			if (StorPtr == Storage + MEMLEN) {
				pack();
			}
		}
	}
	pack();
}
