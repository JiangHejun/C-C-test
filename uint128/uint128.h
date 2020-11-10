#ifndef __UINT128_H__
#define __UINT128_H__

#include <stdint.h>

#include <string>

class uint128 {
  public:
    uint128() : hi(0), lo(0) {}
    uint128(uint32_t l) : hi(0), lo(l) {}
    uint128(int32_t l) : hi(-(l < 0)), lo(l) {}
    uint128(int64_t l) : hi(-(l < 0)), lo(l) {}
    uint128(uint64_t l) : hi(0), lo(l) {}
    uint128(const std::string& s);
    uint128(uint64_t _h, uint64_t _l) : hi(_h), lo(_l) {}

    bool operator==(const uint128& o) const { return hi == o.hi && lo == o.lo; }
    bool operator!=(const uint128& o) const { return hi != o.hi || lo != o.lo; }
    bool operator<(const uint128& o) const { return (hi == o.hi) ? lo < o.lo : hi < o.hi; }
    bool operator<(const int64_t& o) const { return *this < uint128(o); }
    bool operator!() const { return !(hi != 0 || lo != 0); }
    uint128 operator-() const { return ++uint128(~hi, ~lo); }
    uint128 operator~() const { return uint128(~hi, ~lo); }

    uint128& operator++() {
        hi += (++lo == 0);
        return *this;
    }
    uint128& operator--() {
        hi -= (lo-- == 0);
        return *this;
    }
    uint128 operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    uint128 operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    uint128& operator|=(const uint128& u) {
        hi |= u.hi;
        lo |= u.lo;
        return *this;
    }
    uint128& operator&=(const uint128& u) {
        hi &= u.hi;
        lo &= u.lo;
        return *this;
    }
    uint128& operator^=(const uint128& u) {
        hi ^= u.hi;
        lo ^= u.lo;
        return *this;
    }
    uint128& operator<<=(const uint128& u);
    uint128& operator>>=(const uint128& u);

    uint128& operator+=(const uint128& u) {
        const uint64_t old = lo;
        lo += u.lo;
        hi += u.hi + (lo < old);
        return *this;
    }
    uint128& operator-=(const uint128& u) { return *this += -u; }
    uint128& operator*=(const uint128& u);

    friend uint128 operator+(const uint128& l, const uint128& r) { return uint128(l) += r; }
    friend uint128 operator+(const uint128& l, const uint64_t& r) { return uint128(l) += uint128(r); }
    friend uint128 operator+(const uint128& l, const uint32_t& r) { return uint128(l) += uint128(r); }
    friend uint128 operator+(const uint128& l, const int32_t& r) { return uint128(l) += uint128(r); }
    friend uint128 operator+(const uint64_t& l, const uint128& r) { return uint128(l) += r; }
    friend uint128 operator-(const uint128& l, const uint128& r) { return uint128(l) -= r; }
    friend uint128 operator*(const uint128& l, const uint128& r) { return uint128(l) *= r; }
    friend uint128 operator*(const uint128& l, const uint64_t& r) { return uint128(l) *= uint128(r); }
    friend uint128 operator*(const uint128& l, const uint32_t& r) { return uint128(l) *= uint128(r); }
    friend uint128 operator|(const uint128& l, const uint128& r) { return uint128(l) = (r); }
    friend uint128 operator&(const uint128& l, const uint128& r) { return uint128(l) &= r; }
    friend uint128 operator&(const uint128& l, const uint64_t& r) { return uint128(l) &= uint128(r); }
    friend uint128 operator^(const uint128& l, const uint128& r) { return uint128(l) ^= r; }
    friend uint128 operator<<(const uint128& l, const uint128& r) { return uint128(l) <<= r; }
    friend uint128 operator>>(const uint128& l, const uint128& r) { return uint128(l) >>= r; }
    friend uint128 operator>>(const uint128& l, const int32_t& r) { return uint128(l) >>= uint128(r); }
    friend bool operator>(const uint128& l, const uint128& r) { return r < l; }
    friend bool operator>(const uint128& l, const int64_t& r) { return uint128(r) < l; }
    friend bool operator>(const int64_t& l, const uint128& r) { return r < uint128(l); }

    friend bool operator>=(const uint128& l, const uint128& r) { return l == r || l > r; }
    friend bool operator>=(const uint128& l, const int64_t& r) { return l >= uint128(r); }
    friend bool operator>=(const int64_t& l, const uint128& r) { return uint128(l) >= r; }
    friend bool operator<=(const uint128& l, const uint128& r) { return l == r || l < r; }
    friend bool operator<=(const uint128& l, const int64_t& r) { return l <= uint128(r); }
    friend bool operator<=(const int64_t& l, const uint128& r) { return uint128(l) <= r; }

    operator uint64_t() { return lo; }            //强制转换为uint64_t
    operator uint32_t() { return (uint32_t)lo; }  //强制转换为uint32_t
    operator int32_t() { return (int32_t)lo; }    //强制转换为int32_t

    uint32_t low_32_bits() const { return (uint32_t)lo; }
    uint64_t low_bits() const { return lo; }
    uint64_t high_bits() const { return hi; }

    uint64_t hi;
    uint64_t lo;
};

#endif