#define _CRT_SECURE_NO_WARNINGS
#include "xxhash.h"
#include <iostream>
#include "time.h"
using namespace std;

// GCD를 구하는 함수
unsigned __int64 gcd(unsigned __int64 a, unsigned __int64 n)
{
	unsigned __int64 temp;
	while (n > 0)
	{
		temp = a % n;
		a = n;
		n = temp;
	}
	return a;
}

// Modular Exponentiation을 위한 함수. a^m mod n으로 표현할 수 있다.
unsigned __int64 expo(unsigned __int64 a, unsigned __int64 m, unsigned __int64 n)
{
	unsigned __int64 b = 1;

	while (m > 0)
	{
		while (m % 2 == 0)
		{
			m = m / 2;
			a = (a*a) % n;
		}
		m--;

		b = (b*a) % n;
	}

	return b;
}

// Miller-Rabin 소수판정법
bool MillerRabin(unsigned __int64 n) {
	unsigned __int64 a, b;
	unsigned __int64 m = n - 1;
	unsigned __int64 k = 0;
	unsigned __int64 test = 0;
	while (m % 2 == 0)
	{
		m = m / 2;
		k++;
	}
	for (int i = 0; i < 20; i++) {
		a = rand() % (n - 3) + 2;
		if (gcd(a, n) != 1)				// GCD가 1이 아니면 소수가 아니다.
			return false;
		b = expo(a, m, n);
		if (b != 1 && b != (n - 1))
		{
			for (int j = 0; j < k - 1; j++)
			{
				b = (b * b) % n;
				if (b == (n - 1))       // 소수일 경우
					break;
			}

			if (b != (n - 1))
			{
				return false;			// 소수가 아닐 경우
			}
		}
	}
	return true;		// 소수로 판정됨
}

// 확장된 유클리드 호제법 함수
unsigned __int64 extended(unsigned __int64 num, unsigned __int64 mod)
{
	unsigned __int64 b1 = 0, b2 = 1;
	unsigned __int64 a1 = 1, a2 = 0;
	unsigned __int64 q = 0;
	unsigned __int64 t, t1, t2;
	unsigned __int64 m = mod;

	while (mod != 0)
	{
		q = num / mod;

		t = mod;
		mod = num % mod;
		num = t;

		t1 = b2 - q * b1;
		b2 = b1;
		b1 = t1;

		t2 = a2 - q * a1;
		a2 = a1;
		a1 = t2;
	}

	b2 = (b2 + m) % m;

	return b2;
}

// 중국인의 나머지 정리
unsigned __int64 crt(unsigned __int64 c, unsigned __int64 p, unsigned __int64 q, unsigned __int64 r, unsigned __int64 d, unsigned __int64 n)
{
	unsigned __int64 m = 0;
	unsigned __int64 dr, dpq, mr, mpq, r1;
	dr = d % (r - 1);
	dpq = d % ((p - 1)*(q - 1));

	mr = expo(c, dr, r);
	mpq = expo(c, dpq, p*q);

	r1 = extended(r, p*q);		// r의 역원

	m = mr + r * ((r1*(mpq - mr)) % n);
	return m;
}

int main() {
	char buf[65];
	char dbuf[65];
	unsigned __int64 p, q, r, e, d, N, phi = 0;
	unsigned __int64 message;
	srand(time(NULL));

	// p, q, r가 짝수이거나 Miller-Rabin 소수판정법에 의해 소수가 아니면 다시 랜덤한 값을 뽑게 한다
	do {
		p = rand() % 1022 + 2;	// p, q, r은 2부터 1023사이의 랜덤한 값을 가진다
		q = rand() % 1022 + 2;
		r = rand() % 1022 + 2;
	} while (p % 2 == 0 || q % 2 == 0 || r % 2 == 0 || MillerRabin(p) == false || MillerRabin(q) == false || MillerRabin(r) == false);
	N = p * q * r;
	phi = (p - 1)*(q - 1)*(r - 1);	// 오일러의 totient

	// 공개키 e가 짝수이거나 GCD가 1이 아니면 다시 랜덤한 값을 뽑게 한다
	do {
		e = rand() % (phi - 3) + 2;
	} while (e % 2 == 0 || gcd(e, phi) != 1);

	d = extended(e, phi);		// 공개키 e의 역원을 비밀키로 한다

	cout << "p = " << p << endl << "q = " << q << endl << "r = " << r << endl
		<< "N = " << N << endl;
	cout << "phi = " << phi << endl << "e = " << e << endl << "d = " << d << endl << endl;
	cout << "Message Input : ";
	cin >> message;				// 메시지 입력
	cout << "Message : " << message << endl << endl;		// 입력된 메시지

	// 메시지 암호화
	unsigned __int64 cipher;
	cipher = expo(message, e, N);
	sprintf(buf, "%I64u", message);
	unsigned __int64 hash = XXH64(buf, sizeof(buf) - 1, 0);
	hash = hash % N;			// hash 값을 N보다 작게 한다
	cout << "**Encryption" << endl << "cipher : " << cipher << endl << endl;
	cout << "**Generate signature" << endl << "message's hash value : " << hash << endl;

	// 전자 서명
	unsigned __int64 signature;
	signature = expo(hash, d, N);			// hash 값을 암호화한다
	cout << "generated signature : " << signature << endl << endl << endl;

	// 메시지 복호화
	unsigned __int64 dec_cipher;
	dec_cipher = crt(cipher, p, q, r, d, N) % N;
	cout << "**Decryption" << endl << "decrypted cipher : " << dec_cipher << endl << endl;

	// 서명 검증
	sprintf(dbuf, "%I64u", dec_cipher);		// 복호화된 메시지의 hash 값을 구한다
	unsigned __int64 dhash = XXH64(dbuf, sizeof(dbuf) - 1, 0);
	dhash = dhash % N;

	cout << "**Verify signature" << endl << "received signature value : " << signature << endl
		<< "decrypted message's hash value : " << dhash << endl;

	// signature을 복호화 한다
	unsigned __int64 dsignature; 
	dsignature = expo(signature, e, N);
	cout << "verify value from signature : " << dsignature << endl;
	
	// 복호화된 signature 값과 복호화된 메시지의 hash 값을 비교한다
	if (dsignature == dhash) {
		cout << "Signature valid!" << endl;	// 일치하면 값이 유효하다
	}
	else {
		cout << "Signature not valid!" << endl;	// 일치하지 않으면 값이 유효하지 않다
	}

	system("pause");
	return 0;
}