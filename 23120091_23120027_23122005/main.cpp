#include "bigInt.h"
#include <iostream>

using namespace std;

int main() {
	// 1. Tạo số nguyên tố an toàn p và cơ số g
	BigIntBinary p;
	BigIntBinary g(2);
	int bit_size = 512;
	p = generate_safe_prime(bit_size);


	// 2. Sinh khóa riêng của Alice và Bob
	BigIntBinary alicePrivateKey = generate_private_key(p);
	BigIntBinary bobPrivateKey = generate_private_key(p);

	// 3. Tính giá trị công khai của Alice và Bob
	BigIntBinary A = modular_exponentiation(g, alicePrivateKey, p);
	BigIntBinary B = modular_exponentiation(g, bobPrivateKey, p);


	BigIntBinary aliceSharedSecret = modular_exponentiation(B, alicePrivateKey, p); // Alice tính s = B^a % p
	BigIntBinary bobSharedSecret = modular_exponentiation(A, bobPrivateKey, p); // Bob tính s = A^b % p

	std::cout << "Bí mật chung Alice nhận được: " << aliceSharedSecret << std::endl;
	std::cout << "Bí mật chung Bob nhận được: " << bobSharedSecret << std::endl;

	if (aliceSharedSecret == bobSharedSecret) {
		std::cout << "Thành công! Alice và Bob đã chia sẻ cùng một bí mật chung." << std::endl;
	} else {
		std::cout << "Thất bại! Bí mật chung của Alice và Bob không khớp." << std::endl;
	}

	return 0;
}