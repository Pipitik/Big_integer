#ifndef BIG_INTEGER_BIG_INTEGER_H
#define BIG_INTEGER_BIG_INTEGER_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <stack>
#include <string>
#include <vector>

class Big_integer_overflow : public std::runtime_error
{
public:
	Big_integer_overflow() : std::runtime_error("Big integer overflow") {}
};

class Big_integer_division_by_zero : public std::runtime_error
{
public:
	Big_integer_division_by_zero() : std::runtime_error("Big integer division by zero") {}
};

// Big_integer является шаблонным классом.
// Шаблонный аргумент NDigits означает максимальное количество цифр в числе.
template <std::size_t NDigits = 40000>
class Big_integer
{
	friend std::istream& operator>>(std::istream& is, Big_integer& bigInt)
	{
		long long input_number;
		is >> input_number;
		bigInt = input_number;
		
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const Big_integer& bigInt)
	{
		if (bigInt.m_digits.empty()) {
			os << 0;
		}
		else {
			if (bigInt.is_negative()) {
				os << '-';
			}
			for (typename std::vector<digit_type>::size_type i = bigInt.m_digits.size(); i != 0; --i) {
				if (i != bigInt.m_digits.size() && bigInt.m_BASE / 10 > bigInt.m_digits[i - 1]) { // bigInt.m_BASE / 10 is number that mean whether to add the zeros in front
					std::size_t amount_digits_of_element = _count_digits(bigInt.m_digits[i - 1]);
					std::size_t amount_base_zeros = _count_zeros(bigInt.m_BASE);
					for (std::size_t zero_number = 0; zero_number != amount_base_zeros - amount_digits_of_element; ++zero_number) {
						os << 0;
					}
				}
				os << bigInt.m_digits[i - 1];
			}
		}

		return os;
	}

	friend Big_integer operator+(const Big_integer& lhs, const Big_integer& rhs)
	{
		Big_integer ret(lhs);
		ret += rhs;
		return ret;
	}

	friend Big_integer operator-(const Big_integer& lhs, const Big_integer& rhs)
	{
		Big_integer ret(lhs);
		ret -= rhs;
		return ret;	
	}

	friend Big_integer operator*(const Big_integer& lhs, const Big_integer& rhs)
	{
		Big_integer ret(lhs);
		ret *= rhs;
		return ret;
	}

	friend Big_integer operator/(const Big_integer& lhs, const Big_integer& rhs)
	{
		Big_integer ret(lhs);
		ret /= rhs;
		return ret;
	}

	friend bool operator==(const Big_integer& lhs, const Big_integer& rhs)
	{
		if (lhs.m_digits.size() != rhs.m_digits.size() || lhs.is_negative() != rhs.is_negative()) {
			return false;
		} 
	
		for (typename std::vector<digit_type>::size_type i = 0; i != lhs.m_digits.size(); ++i) {
			if (lhs.m_digits[i] != rhs.m_digits[i]) {
				return false;
			}
		}
	
		return true;
	}

	friend bool operator!=(const Big_integer& lhs, const Big_integer& rhs)
	{
		return !(lhs == rhs);
	}

	friend bool operator<(const Big_integer& lhs, const Big_integer& rhs)
	{
		bool are_positive = !lhs.is_negative() && !rhs.is_negative();
		bool are_negative = lhs.is_negative() && rhs.is_negative();
		bool lhs_less_rhs = lhs.m_digits.size() < rhs.m_digits.size();
		bool lhs_more_rhs = lhs.m_digits.size() > rhs.m_digits.size();
		if ((lhs_less_rhs && are_positive) || (lhs_more_rhs && are_negative) || (lhs.is_negative() && !rhs.is_negative())) {
			return true;
		} 
		else if (lhs.m_digits.size() == rhs.m_digits.size() && lhs.is_negative() == rhs.is_negative()) {
			for (typename std::vector<digit_type>::size_type i = lhs.m_digits.size(); i != 0; --i) {
				if (lhs.m_digits[i - 1] < rhs.m_digits[i - 1]) {
					return true;
				}
				else if (lhs.m_digits[i - 1] > rhs.m_digits[i - 1]) {
					return false;
				}
			}
		}
	
		return false;
	}

	friend bool operator>(const Big_integer& lhs, const Big_integer& rhs)
	{
		return rhs < lhs;
	}

	friend bool operator<=(const Big_integer& lhs, const Big_integer& rhs)
	{
		return (lhs < rhs) || (lhs == rhs);
	}

	friend bool operator>=(const Big_integer& lhs, const Big_integer& rhs)
	{
		return (rhs < lhs) || (lhs == rhs);
	}

public:
	using digit_type = std::uint16_t;
	using double_digit_type = std::uint32_t;

	Big_integer() : m_is_negative(false), m_BASE(10000) {}
	Big_integer(const Big_integer& bigInt) : m_digits(bigInt.m_digits), m_is_negative(bigInt.m_is_negative), m_BASE(bigInt.m_BASE){}
	Big_integer& operator=(const Big_integer& rhs)
	{
		if (this != &rhs) {
			m_digits = rhs.m_digits;
			m_is_negative = rhs.m_is_negative;
		}

		return *this;
	}

	Big_integer(long long number); 
	explicit Big_integer(const char* pch) : Big_integer(std::atoll(pch)) {}
	Big_integer(const std::string& number) : Big_integer(std::stoll(number)) {}
 
	bool is_negative() const { return m_is_negative; }

	Big_integer& operator+=(const Big_integer&);
	Big_integer& operator-=(const Big_integer&);
	Big_integer& operator*=(const Big_integer&);
	Big_integer& operator/=(const Big_integer&);

	Big_integer& operator++();
	Big_integer& operator--();
	Big_integer operator++(int);
	Big_integer operator--(int);

	Big_integer operator+() const;
	Big_integer operator-() const;

	explicit operator bool() const { return !m_digits.empty(); }

private:
	std::vector<digit_type> m_digits;
	bool m_is_negative;
	const digit_type m_BASE;
	
	static std::size_t _count_digits(std::size_t number)
	{
		std::size_t ret = 0;
		while (number != 0) {
			number /= 10;
			++ret;
		}

		return ret == 0 ? 1 : ret;
	}

	static std::size_t _count_zeros(std::size_t number)
	{
		std::size_t ret = 0;
		while (number % 10 == 0) {
			number /= 10;
			++ret;
		}

		return ret;
	}

	void _add_number(double_digit_type number, std::vector<digit_type>::size_type pos)
	{
		if (m_digits.size() > pos) {
			m_digits[pos] = number;
		}
		else {
			m_digits.push_back(number);
		}
	}

	void _delete_leading_zeros()
	{
		while (!m_digits.empty() && m_digits.back() == 0) {
			m_digits.pop_back();
		}
	}

	bool _are_there_too_many_digits_here(const Big_integer& bigInt)
	{
		if (!bigInt.m_digits.empty()) {
			std::vector<digit_type>::size_type amount_digits_in_front = _count_digits(bigInt.m_digits.back());
			return _count_digits(m_BASE - 1) * (bigInt.m_digits.size() - 1) + amount_digits_in_front > NDigits;
		}
		else {
			return false;
		}
	}

	Big_integer _abs(const Big_integer& bigInt)
	{
		return bigInt >= 0 ? bigInt : -bigInt;
	}
};

template <std::size_t NDigits>
Big_integer<NDigits>::Big_integer(long long number) : m_is_negative(false), m_BASE(10000)
{
	if (_count_digits(number < 0 ? -number : number) > NDigits) {
		throw Big_integer_overflow{};
	}

	if (number < 0) {
		m_is_negative = true;
		number *= -1;
	}

	while (number != 0) {
		m_digits.push_back(number % m_BASE);
		number /= m_BASE;
	}
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator+=(const Big_integer& rhs)
{
	if (_are_there_too_many_digits_here(rhs)) {
		throw Big_integer_overflow{};
	}

	if (m_digits.empty() || rhs.m_digits.empty()) {
		if (!rhs.m_digits.empty()) {
			*this = rhs;
		}
	}
	else {
		bool are_negative_or_positive = is_negative() == rhs.is_negative();
		if (are_negative_or_positive) {
			double_digit_type carry = 0;
			std::vector<digit_type>::size_type max_size = 
				std::max(m_digits.size(), rhs.m_digits.size());
			
			Big_integer<NDigits> copy(*this);
	
			for (std::vector<digit_type>::size_type i = 0; i != max_size; ++i) {
				double_digit_type temp_carry = carry;
				
				if (i < m_digits.size()) {
					temp_carry += m_digits[i];
				}
				if (i < rhs.m_digits.size()) {
					temp_carry += rhs.m_digits[i];
				}

				_add_number(temp_carry % m_BASE, i);
				carry = temp_carry / m_BASE;
			} 
			if (carry > 0) {
				m_digits.push_back(carry);
				if (_are_there_too_many_digits_here(*this)) {
					*this = copy;
					throw Big_integer_overflow{};
				}
			}
		}
		else {
			m_is_negative = !m_is_negative;
			*this -= rhs;
			m_is_negative = !m_is_negative;
		}
	}

	return *this; 
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator-=(const Big_integer& rhs)
{
	if (_are_there_too_many_digits_here(rhs)) {
		throw Big_integer_overflow{};
	}

	if (this == &rhs) {
		m_digits.clear();
	}
	else if (m_digits.empty() || rhs.m_digits.empty()) {
		if (!rhs.m_digits.empty()) {
			*this = rhs;
			m_is_negative = !m_is_negative;
		}
	}
	else {
		bool are_negative_or_positive = is_negative() == rhs.is_negative();
		if (are_negative_or_positive) {
			if (*this == rhs) {
				m_digits.clear();
				m_is_negative = false;
			}
			else if (*this < rhs) {
				*this = rhs - *this;
				m_is_negative = !m_is_negative;
			}
			else {
				for (std::vector<digit_type>::size_type i = 0; i != rhs.m_digits.size(); ++i) {
					digit_type borrowed = 0;

					if (m_digits[i] < rhs.m_digits[i]) {
						std::vector<digit_type>::size_type j = i + 1;
						while (j != m_digits.size() && m_digits[j] == 0) { 
							m_digits[j] = m_BASE - 1;
							++j;
						}
						--m_digits[j];
						borrowed = static_cast<digit_type>(m_BASE);
					}

					m_digits[i] += (borrowed - rhs.m_digits[i]);
				}

				_delete_leading_zeros();				
			}
		}
		else {
			try {
				m_is_negative = !m_is_negative;
				*this += rhs;
				m_is_negative = !m_is_negative;
			}
			catch (const Big_integer_overflow&) {
				m_is_negative = !m_is_negative;
				throw Big_integer_overflow{};
			}
		}
	}

	return *this;
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator*=(const Big_integer& rhs)
{
	if (this == &rhs) {
		*this = *this * rhs;
	}
	else if (m_digits.empty() || rhs.m_digits.empty()) {
		m_digits.clear();
		m_is_negative = false;
	}
	else {
		bool lhs_is_negative = m_is_negative != rhs.is_negative();

		m_is_negative = false;
		const Big_integer<NDigits> lhs(*this);
		m_digits.clear();
		
		for (std::vector<digit_type>::size_type i = 0; i != rhs.m_digits.size(); ++i) {
			Big_integer<NDigits> temp_bigInt;
			double_digit_type carry = 0;

			std::fill_n(std::back_inserter(temp_bigInt.m_digits), i, 0);

			for (std::vector<digit_type>::size_type j = 0; j != lhs.m_digits.size(); ++j) {
				double_digit_type temp_carry = carry + static_cast<double_digit_type>(rhs.m_digits[i]) * lhs.m_digits[j];
				temp_bigInt.m_digits.push_back(temp_carry % m_BASE);
				carry = temp_carry / m_BASE;
			}
			if (carry > 0) {
				temp_bigInt.m_digits.push_back(carry);
			}
		
			try {
				*this += temp_bigInt;
			}
			catch (const Big_integer_overflow&) {
				*this = lhs;
				throw Big_integer_overflow{};				
			}
		}

		m_is_negative = lhs_is_negative;
	}

	return *this;
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator/=(const Big_integer& rhs)
{
	if (rhs.m_digits.empty()) {
		throw Big_integer_division_by_zero{};
	}

	Big_integer<NDigits> abs_rhs = _abs(rhs);

	if (_abs(*this) < abs_rhs) {
		m_digits.clear();
		m_is_negative = false;
	}
	else {
		bool lhs_is_negative = m_is_negative != rhs.is_negative();

		m_is_negative = false;

		std::stack<digit_type> quotient;
		Big_integer<NDigits> number;
		for (std::vector<digit_type>::size_type i = m_digits.size(); i != 0; --i) {
			digit_type part_result = 0;

			for (std::size_t temp_base = m_BASE / 10; temp_base != 0; temp_base /= 10) {
				number *= 10;
				part_result *= 10;
				number += (m_digits[i - 1] / temp_base) % 10;
				digit_type digit = 0;

				while (number >= abs_rhs) {
					number -= abs_rhs;
					++digit;
				}

				part_result += digit;
			}

			quotient.push(part_result);
		}
			
		m_digits.clear();
		while (!quotient.empty()) {
			m_digits.push_back(quotient.top());
			quotient.pop();
		}
		m_is_negative = lhs_is_negative;
		_delete_leading_zeros();
	}

	return *this;
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator++()
{
	return *this += 1;
}

template <std::size_t NDigits>
Big_integer<NDigits>& Big_integer<NDigits>::operator--()
{
	return *this -= 1;
}

template <std::size_t NDigits>
Big_integer<NDigits> Big_integer<NDigits>::operator++(int)
{
	Big_integer<NDigits> ret(*this);
	*this += 1;
	return ret;
}

template <std::size_t NDigits>
Big_integer<NDigits> Big_integer<NDigits>::operator--(int)
{
	Big_integer<NDigits> ret(*this);
	*this -= 1;
	return ret;
}

template <std::size_t NDigits>
Big_integer<NDigits> Big_integer<NDigits>::operator+() const
{
	return *this;
}

template <std::size_t NDigits>
Big_integer<NDigits> Big_integer<NDigits>::operator-() const
{
	Big_integer<NDigits> ret(*this);
	ret.m_is_negative = !ret.m_is_negative;
	return ret;
}

#endif
