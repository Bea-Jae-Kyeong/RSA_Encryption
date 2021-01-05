#define _CRT_SECURE_NO_WARNINGS
#include "xxhash.h"
#include <iostream>
#include "time.h"
using namespace std;

// GCD�� ���ϴ� �Լ�
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

// Modular Exponentiation�� ���� �Լ�. a^m mod n���� ǥ���� �� �ִ�.
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

// Miller-Rabin �Ҽ�������
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
		if (gcd(a, n) != 1)				// GCD�� 1�� �ƴϸ� �Ҽ��� �ƴϴ�.
			return false;
		b = expo(a, m, n);
		if (b != 1 && b != (n - 1))
		{
			for (int j = 0; j < k - 1; j++)
			{
				b = (b * b) % n;
				if (b == (n - 1))       // �Ҽ��� ���
					break;
			}

			if (b != (n - 1))
			{
				return false;			// �Ҽ��� �ƴ� ���
			}
		}
	}
	return true;		// �Ҽ��� ������
}

// Ȯ��� ��Ŭ���� ȣ���� �Լ�
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

// �߱����� ������ ����
unsigned __int64 crt(unsigned __int64 c, unsigned __int64 p, unsigned __int64 q, unsigned __int64 r, unsigned __int64 d, unsigned __int64 n)
{
	unsigned __int64 m = 0;
	unsigned __int64 dr, dpq, mr, mpq, r1;
	dr = d % (r - 1);
	dpq = d % ((p - 1)*(q - 1));

	mr = expo(c, dr, r);
	mpq = expo(c, dpq, p*q);

	r1 = extended(r, p*q);		// r�� ����

	m = mr + r * ((r1*(mpq - mr)) % n);
	return m;
}

int main() {
	char buf[65];
	char dbuf[65];
	unsigned __int64 p, q, r, e, d, N, phi = 0;
	unsigned __int64 message;
	srand(time(NULL));

	// p, q, r�� ¦���̰ų� Miller-Rabin �Ҽ��������� ���� �Ҽ��� �ƴϸ� �ٽ� ������ ���� �̰� �Ѵ�
	do {
		p = rand() % 1022 + 2;	// p, q, r�� 2���� 1023������ ������ ���� ������
		q = rand() % 1022 + 2;
		r = rand() % 1022 + 2;
	} while (p % 2 == 0 || q % 2 == 0 || r % 2 == 0 || MillerRabin(p) == false || MillerRabin(q) == false || MillerRabin(r) == false);
	N = p * q * r;
	phi = (p - 1)*(q - 1)*(r - 1);	// ���Ϸ��� totient

	// ����Ű e�� ¦���̰ų� GCD�� 1�� �ƴϸ� �ٽ� ������ ���� �̰� �Ѵ�
	do {
		e = rand() % (phi - 3) + 2;
	} while (e % 2 == 0 || gcd(e, phi) != 1);

	d = extended(e, phi);		// ����Ű e�� ������ ���Ű�� �Ѵ�

	cout << "p = " << p << endl << "q = " << q << endl << "r = " << r << endl
		<< "N = " << N << endl;
	cout << "phi = " << phi << endl << "e = " << e << endl << "d = " << d << endl << endl;
	cout << "Message Input : ";
	cin >> message;				// �޽��� �Է�
	cout << "Message : " << message << endl << endl;		// �Էµ� �޽���

	// �޽��� ��ȣȭ
	unsigned __int64 cipher;
	cipher = expo(message, e, N);
	sprintf(buf, "%I64u", message);
	unsigned __int64 hash = XXH64(buf, sizeof(buf) - 1, 0);
	hash = hash % N;			// hash ���� N���� �۰� �Ѵ�
	cout << "**Encryption" << endl << "cipher : " << cipher << endl << endl;
	cout << "**Generate signature" << endl << "message's hash value : " << hash << endl;

	// ���� ����
	unsigned __int64 signature;
	signature = expo(hash, d, N);			// hash ���� ��ȣȭ�Ѵ�
	cout << "generated signature : " << signature << endl << endl << endl;

	// �޽��� ��ȣȭ
	unsigned __int64 dec_cipher;
	dec_cipher = crt(cipher, p, q, r, d, N) % N;
	cout << "**Decryption" << endl << "decrypted cipher : " << dec_cipher << endl << endl;

	// ���� ����
	sprintf(dbuf, "%I64u", dec_cipher);		// ��ȣȭ�� �޽����� hash ���� ���Ѵ�
	unsigned __int64 dhash = XXH64(dbuf, sizeof(dbuf) - 1, 0);
	dhash = dhash % N;

	cout << "**Verify signature" << endl << "received signature value : " << signature << endl
		<< "decrypted message's hash value : " << dhash << endl;

	// signature�� ��ȣȭ �Ѵ�
	unsigned __int64 dsignature; 
	dsignature = expo(signature, e, N);
	cout << "verify value from signature : " << dsignature << endl;
	
	// ��ȣȭ�� signature ���� ��ȣȭ�� �޽����� hash ���� ���Ѵ�
	if (dsignature == dhash) {
		cout << "Signature valid!" << endl;	// ��ġ�ϸ� ���� ��ȿ�ϴ�
	}
	else {
		cout << "Signature not valid!" << endl;	// ��ġ���� ������ ���� ��ȿ���� �ʴ�
	}

	system("pause");
	return 0;
}