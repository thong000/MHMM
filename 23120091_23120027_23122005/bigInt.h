#ifndef BIGINT_BINARY_H
#define BIGINT_BINARY_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint> // Để dùng uint32_t và uint64_t

class BigIntBinary {
private:
    std::vector<uint32_t> limbs;

    // Xóa các số 0 thừa ở đầu
    void normalize();
    void add_int(uint32_t n);
    void multiply_by_int(uint32_t n);
    uint32_t divide_by_10_and_get_remainder();

public:
    // --- Constructors ---
    BigIntBinary(unsigned long long n = 0);
    BigIntBinary(const std::string& s);
    BigIntBinary(const BigIntBinary& other); // Copy constructor

    // --- Phép toán quan trọng ---

    void divide_by_2();
    bool is_odd() const;
    bool is_zero() const;


    // --- Phép so sánh ---
    friend bool operator<(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator==(const BigIntBinary& a, const BigIntBinary& b);

    // --- In số ---
    friend std::ostream& operator<<(std::ostream& out, const BigIntBinary& a);


    // --- Thao tác bit ---
    void shift_left_1_bit();
    int num_bits() const;
    bool get_bit(int n) const;
    void set_bit(int n);

    // --- Phép toán---
    // Phép gán
    BigIntBinary& operator=(const BigIntBinary& other);

    // Phép cộng
    BigIntBinary& operator+=(const BigIntBinary& other);
    friend BigIntBinary operator+(const BigIntBinary& a, const BigIntBinary& b);

    // Phép trừ
    BigIntBinary& operator-=(const BigIntBinary& other);
    friend BigIntBinary operator-(const BigIntBinary& a, const BigIntBinary& b);

    // Phép nhân
    BigIntBinary& operator*=(const BigIntBinary& other);
    friend BigIntBinary operator*(const BigIntBinary& a, const BigIntBinary& b);

    // Phép chia & Modulo 
    void divide(const BigIntBinary& divisor, BigIntBinary& quotient, BigIntBinary& remainder) const;
    BigIntBinary& operator/=(const BigIntBinary& other);
    friend BigIntBinary operator/(const BigIntBinary& a, const BigIntBinary& b);
    BigIntBinary& operator%=(const BigIntBinary& other);
    friend BigIntBinary operator%(const BigIntBinary& a, const BigIntBinary& b);

    // --- Phép so sánh ---
    friend bool operator<(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator==(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator!=(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator>(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator<=(const BigIntBinary& a, const BigIntBinary& b);
    friend bool operator>=(const BigIntBinary& a, const BigIntBinary& b);

};

BigIntBinary modular_exponentiation(BigIntBinary a, BigIntBinary b, BigIntBinary n); // a^b % n
BigIntBinary generate_private_key(const BigIntBinary& p);
BigIntBinary generate_safe_prime(int bit_size);

#endif
