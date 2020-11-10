#include <uint128.h>

uint128::uint128(const std::string& sz) : hi(0), lo(0) {
    // do we have at least one character?
    if (!sz.empty()) {
        // make some reasonable assumptions
        int radix = 10;
        bool minus = false;

        std::string::const_iterator i = sz.begin();

        // check for minus sign, i suppose technically this should only apply
        // to base 10, but who says that -0x1 should be invalid?
        if (*i == '-') {
            ++i;
            minus = true;
        }

        // check if there is radix changing prefix (0 or 0x)
        if (i != sz.end()) {
            if (*i == '0') {
                radix = 8;
                ++i;
                if (i != sz.end()) {
                    if (*i == 'x') {
                        radix = 16;
                        ++i;
                    }
                }
            }

            while (i != sz.end()) {
                unsigned int n = 0;
                const char ch = *i;

                if (ch >= 'A' && ch <= 'Z') {
                    if (((ch - 'A') + 10) < radix) {
                        n = (ch - 'A') + 10;
                    } else {
                        break;
                    }
                } else if (ch >= 'a' && ch <= 'z') {
                    if (((ch - 'a') + 10) < radix) {
                        n = (ch - 'a') + 10;
                    } else {
                        break;
                    }
                } else if (ch >= '0' && ch <= '9') {
                    if ((ch - '0') < radix) {
                        n = (ch - '0');
                    } else {
                        break;
                    }
                } else {
                    /* completely invalid character */
                    break;
                }

                (*this) *= radix;
                (*this) += n;

                ++i;
            }
        }

        if (minus) {
            *this = -*this;
        }
    }
}

uint128& uint128::operator<<=(const uint128& rhs) {
    if (rhs >= 128) {
        hi = 0;
        lo = 0;
    } else {
        unsigned int n = rhs.to_integer();
        const unsigned int halfsize = 128 / 2;

        if (n >= halfsize) {
            n -= halfsize;
            hi = lo;
            lo = 0;
        }

        if (n != 0) {
            // shift high half
            hi <<= n;

            const uint64_t mask(~(uint64_t(-1) >> n));

            // and add them to high half
            hi |= (lo & mask) >> (halfsize - n);

            // and finally shift also low half
            lo <<= n;
        }
    }

    return *this;
}

uint128& uint128::operator>>=(const uint128& rhs) {
    if (rhs >= 128) {
        hi = 0;
        lo = 0;
    } else {
        unsigned int n = rhs.to_integer();
        const unsigned int halfsize = 128 / 2;

        if (n >= halfsize) {
            n -= halfsize;
            lo = hi;
            hi = 0;
        }

        if (n != 0) {
            // shift low half
            lo >>= n;

            // get lower N bits of high half
            const uint64_t mask(~(uint64_t(-1) << n));

            // and add them to low qword
            lo |= (hi & mask) << (halfsize - n);

            // and finally shift also high half
            hi >>= n;
        }
    }
    return *this;
}

uint128& uint128::operator*=(const uint128& b) {
    uint64_t a0 = (uint32_t)(this->lo);
    uint64_t a1 = (uint32_t)(this->lo >> 0x20);
    uint64_t a2 = (uint32_t)(this->hi);
    uint64_t a3 = (uint32_t)(this->hi >> 0x20);

    uint64_t b0 = (uint32_t)(b.lo);
    uint64_t b1 = (uint32_t)(b.lo >> 0x20);
    uint64_t b2 = (uint32_t)(b.hi);
    uint64_t b3 = (uint32_t)(b.hi >> 0x20);

    this->hi = 0;
    this->lo = a3 * b0;
    (*this) += a2 * b1;
    (*this) += a1 * b2;
    (*this) += a0 * b3;
    (*this) <<= 0x20;
    (*this) += a2 * b0;
    (*this) += a1 * b1;
    (*this) += a0 * b2;
    (*this) <<= 0x20;
    (*this) += a1 * b0;
    (*this) += a0 * b1;
    (*this) <<= 0x20;
    (*this) += a0 * b0;

    return *this;
}