/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brian <brian@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 00:56:37 by brian             #+#    #+#             */
/*   Updated: 2025/06/18 01:14:49 by brian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"

const int Fixed::_fract_bits = 8;

Fixed::Fixed() : _fp_value(0) {
  std::cout << "Default Constructor called" << std::endl;
}

Fixed::Fixed(const Fixed &copy) {
  std::cout << "Copy Constructor called" << std::endl;
  *this = copy;
}

Fixed::~Fixed() { std::cout << "Deconstructor called" << std::endl; }

Fixed &Fixed::operator=(const Fixed &src) {
  std::cout << "Assignation operator called" << std::endl;
  if (this != &src)
    this->_fp_value = src.getRawBits();

  return *this;
}

int Fixed::getRawBits(void) const {
  std::cout << "getRawBits member function called" << std::endl;
  return (this->_fp_value);
}

void Fixed::setRawBits(int const raw) {
  std::cout << "setRawBits member function called" << std::endl;
  this->_fp_value = raw;
}