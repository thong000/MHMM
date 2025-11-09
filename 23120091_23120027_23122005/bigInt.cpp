#include "bigInt.h"
#include <algorithm>
#include <random>
#include <stdexcept>

const uint64_t BASE = (1ULL << 32);

void BigIntBinary::normalize() {
    // Xóa các "nhánh" 0 ở cuối vector
    while (!limbs.empty() && limbs.back() == 0) {
        limbs.pop_back();
    }
}

// --- Constructors ---
BigIntBinary::BigIntBinary(unsigned long long n) {
    limbs.clear();
    limbs.push_back((uint32_t)(n & 0xFFFFFFFF)); // Lấy 32 bit thấp
    if (n >> 32) {
        limbs.push_back((uint32_t)(n >> 32));      // Lấy 32 bit cao
    }
    normalize();
}

BigIntBinary::BigIntBinary(const BigIntBinary& other) {
    limbs = other.limbs;
}

// --- Phép toán cộng/nhân số nhỏ
void BigIntBinary::add_int(uint32_t n) {
    if (n == 0) return;
    if (limbs.empty()) limbs.push_back(0);

    uint64_t carry = n;
    for (size_t i = 0; i < limbs.size() && carry > 0; ++i) {
        uint64_t sum = (uint64_t)limbs[i] + carry;
        limbs[i] = (uint32_t)(sum & 0xFFFFFFFF); 
        carry = sum >> 32;            
    }
    if (carry) {
        limbs.push_back((uint32_t)carry);
    }
}

void BigIntBinary::multiply_by_int(uint32_t n) {
    if (n == 0) {
        limbs.clear();
        return;
    }
    uint64_t carry = 0;
    for (size_t i = 0; i < limbs.size(); ++i) {
        uint64_t product = (uint64_t)limbs[i] * n + carry;
        limbs[i] = (uint32_t)(product & 0xFFFFFFFF);
        carry = product >> 32;
    }
    if (carry) {
        limbs.push_back((uint32_t)carry);
    }
}

// --- String Constructor
BigIntBinary::BigIntBinary(const std::string& s) {
    limbs.clear(); // Bắt đầu từ 0
    for (char c : s) {
        if (!isdigit(c)) throw std::runtime_error("Invalid number string");
        multiply_by_int(10);
        add_int(c - '0');
    }
}

// --- Phép toán quan trọng ---
void BigIntBinary::divide_by_2() {
    bool carry = 0;
    // Dịch bit phải trên từng "nhánh", từ nhánh cao nhất xuống
    for (int i = limbs.size() - 1; i >= 0; --i) {
        bool next_carry = (limbs[i] & 1);
        limbs[i] >>= 1;
        if (carry) {
            limbs[i] |= (1U << 31);
        }
        carry = next_carry;
    }
    normalize();
}

bool BigIntBinary::is_odd() const {
    if (is_zero()) return false;
    return (limbs[0] & 1);
}

bool BigIntBinary::is_zero() const {
    return limbs.empty();
}


BigIntBinary& BigIntBinary::operator+=(const BigIntBinary& other) {
    size_t n = limbs.size();
    size_t m = other.limbs.size();

    if (m > n) limbs.resize(m, 0);

    uint64_t carry = 0;
    for (size_t i = 0; i < limbs.size(); ++i) {
        uint64_t sum = (uint64_t)limbs[i] + carry;
        if (i < m) {
            sum += other.limbs[i];
        }
        limbs[i] = (uint32_t)(sum & 0xFFFFFFFF);
        carry = sum >> 32;
    }
    if (carry) {
        limbs.push_back((uint32_t)carry);
    }
    return *this;
}

// --- Phép so sánh ---
bool operator<(const BigIntBinary& a, const BigIntBinary& b) {
    if (a.limbs.size() != b.limbs.size()) {
        return a.limbs.size() < b.limbs.size();
    }


    for (int i = a.limbs.size() - 1; i >= 0; --i) {
        if (a.limbs[i] != b.limbs[i]) {
            return a.limbs[i] < b.limbs[i];
        }
    }
    return false; // Chúng bằng nhau
}

bool operator==(const BigIntBinary& a, const BigIntBinary& b) {
    return a.limbs == b.limbs;
}


uint32_t BigIntBinary::divide_by_10_and_get_remainder() {
    uint64_t remainder = 0;
    for (int i = limbs.size() - 1; i >= 0; --i) {

        // Kết hợp số dư từ nhánh trước với nhánh hiện tại
        uint64_t current_value = (remainder << 32) + limbs[i];

        limbs[i] = (uint32_t)(current_value / 10);
        remainder = current_value % 10;
    }
    normalize();
    return (uint32_t)remainder;
}

std::ostream& operator<<(std::ostream& out, const BigIntBinary& a) {
    if (a.is_zero()) {
        return out << "0";
    }

    BigIntBinary temp = a;
    std::string s = "";

    // Liên tục chia cho 10 để lấy từng chữ số (cơ số 10)
    while (!temp.is_zero()) {
        s += (char)(temp.divide_by_10_and_get_remainder() + '0');
    }

    std::reverse(s.begin(), s.end());
    return out << s;
}


void BigIntBinary::shift_left_1_bit() {
    bool carry = 0;
    for (size_t i = 0; i < limbs.size(); ++i) {
        bool next_carry = (limbs[i] >> 31) & 1;
        limbs[i] <<= 1;
        if (carry) limbs[i] |= 1;
        carry = next_carry;
    }
    if (carry) limbs.push_back(1);
    normalize();
}

int BigIntBinary::num_bits() const {
    if (is_zero()) return 0;

    uint32_t last_limb = limbs.back();
    int bits_in_last = 32;

    for (int i = 31; i >= 0; i--) {
        if ((last_limb >> i) & 1) {
            bits_in_last = i + 1;
            break;
        }
    }

    return (limbs.size() - 1) * 32 + bits_in_last;
}

bool BigIntBinary::get_bit(int n) const {
    int limb_index = n / 32;
    int bit_index = n % 32;
    if (limb_index >= limbs.size()) return 0;
    return (limbs[limb_index] >> bit_index) & 1;
}

void BigIntBinary::set_bit(int n) {
    int limb_index = n / 32;
    int bit_index = n % 32;

    if (limb_index >= limbs.size()) {
        limbs.resize(limb_index + 1, 0);
    }
    limbs[limb_index] |= (1U << bit_index);
}


// --- Phép trừ ---
BigIntBinary& BigIntBinary::operator-=(const BigIntBinary& other) {
    if (*this < other) {
        throw std::runtime_error("Subtraction underflow (negative result not supported)");
    }

    int64_t borrow = 0;
    size_t n = limbs.size();
    size_t m = other.limbs.size();

    for (size_t i = 0; i < n; ++i) {
        int64_t diff = (int64_t)limbs[i] - borrow - (i < m ? other.limbs[i] : 0);

        if (diff < 0) {
            limbs[i] = (uint32_t)(diff + BASE);
            borrow = 1;
        }
        else {
            limbs[i] = (uint32_t)diff;
            borrow = 0;
        }
    }
    normalize();
    return *this;
}

// --- Phép nhân ---
BigIntBinary& BigIntBinary::operator*=(const BigIntBinary& other) {
    BigIntBinary result;
    size_t n = limbs.size();
    size_t m = other.limbs.size();
    if (n == 0 || m == 0) { *this = result; return *this; }

    result.limbs.resize(n + m, 0);

    for (size_t i = 0; i < n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < m; ++j) {

            uint64_t product = (uint64_t)limbs[i] * other.limbs[j]
                + result.limbs[i + j] + carry;

            result.limbs[i + j] = (uint32_t)(product & 0xFFFFFFFF);
            carry = product >> 32;
        }
        result.limbs[i + m] += (uint32_t)carry;
    }

    result.normalize();
    *this = result;
    return *this;
}

// --- Phép chia & Modulo  ---
void BigIntBinary::divide(const BigIntBinary& divisor, BigIntBinary& quotient, BigIntBinary& remainder) const {
    if (divisor.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    quotient = BigIntBinary(0);
    remainder = BigIntBinary(0);
    int bits = this->num_bits();

    for (int i = bits - 1; i >= 0; i--) {
        // 1. Dịch trái số dư (remainder = remainder * 2)
        remainder.shift_left_1_bit();

        // 2. "Hạ" bit tiếp theo của số bị chia vào
        if (this->get_bit(i)) {
            remainder.limbs[0] |= 1; // Đặt bit thấp nhất là 1
        }

        // 3. So sánh và trừ
        if (remainder >= divisor) {
            remainder -= divisor;
            quotient.set_bit(i); // Đặt bit tương ứng trong thương
        }
    }
}

BigIntBinary& BigIntBinary::operator/=(const BigIntBinary& other) {
    BigIntBinary quotient, remainder;
    this->divide(other, quotient, remainder);
    *this = quotient;
    return *this;
}

BigIntBinary& BigIntBinary::operator%=(const BigIntBinary& other) {
    BigIntBinary quotient, remainder;
    this->divide(other, quotient, remainder);
    *this = remainder;
    return *this;
}

// --- Các toán tử tiện ích ---
BigIntBinary operator+(const BigIntBinary& a, const BigIntBinary& b) {
    BigIntBinary temp = a; temp += b; return temp;
}
BigIntBinary operator-(const BigIntBinary& a, const BigIntBinary& b) {
    BigIntBinary temp = a; temp -= b; return temp;
}
BigIntBinary operator*(const BigIntBinary& a, const BigIntBinary& b) {
    BigIntBinary temp = a; temp *= b; return temp;
}
BigIntBinary operator/(const BigIntBinary& a, const BigIntBinary& b) {
    BigIntBinary temp = a; temp /= b; return temp;
}
BigIntBinary operator%(const BigIntBinary& a, const BigIntBinary& b) {
    BigIntBinary temp = a; temp %= b; return temp;
}

bool operator!=(const BigIntBinary& a, const BigIntBinary& b) {
    return !(a == b);
}
bool operator>(const BigIntBinary& a, const BigIntBinary& b) {
    return b < a;
}
bool operator<=(const BigIntBinary& a, const BigIntBinary& b) {
    return !(b < a);
}
bool operator>=(const BigIntBinary& a, const BigIntBinary& b) {
    return !(a < b);
}




BigIntBinary generate_private_key(const BigIntBinary& p) {

    static std::random_device rd;
    static std::mt19937_64 gen(rd());

    std::uniform_int_distribution<uint32_t> dis_32(0, 0xFFFFFFFF);


    BigIntBinary min(2);
    BigIntBinary max = p - min;

    // Lấy số bit tối đa của khóa
    int max_bits = max.num_bits();

    // Xử lý trường hợp p quá nhỏ (ví dụ p=3, phạm vi [2, 1] là không hợp lệ)
    if (max_bits < 2) {
        throw std::runtime_error("So nguyen to p qua nho de tao khoa rieng.");
    }

    BigIntBinary k;

    do {
        k = BigIntBinary(0); // Reset khóa k về 0

        uint32_t current_random_limb = 0;
        int bits_in_limb = 0;

        // Tạo một số ngẫu nhiên k có 'max_bits' bit
        for (int i = 0; i < max_bits; ++i) {

            // Nếu đã dùng hết bit trong "nhánh" 32-bit, tạo nhánh mới
            if (bits_in_limb == 0) {
                current_random_limb = dis_32(gen);
                bits_in_limb = 32;
            }

            // Lấy 1 bit ngẫu nhiên (bit cuối của 'current_random_limb')
            if (current_random_limb & 1) {
                k.set_bit(i); // Bật bit thứ 'i' của k lên
            }

            // Chuyển sang bit tiếp theo
            current_random_limb >>= 1;
            bits_in_limb--;
        }

        // Lặp lại nếu k nằm ngoài phạm vi [min, max]
    } while (k < min || k > max);

    // Trả về khóa hợp lệ
    return k;
}


BigIntBinary& BigIntBinary::operator=(const BigIntBinary& other) {
    limbs = other.limbs;
    return *this;
}