/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 19:00:21 by welow             #+#    #+#             */
/*   Updated: 2025/03/04 21:06:09 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#ifndef CALL
#define CALL 0
#endif

#include <iostream>
#include <string>
#include "Brain.hpp"

#define BLUE_H "\033[1;44m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

class Animal
{
	protected:
		std::string _type;

	public:
		Animal(void);
		Animal(std::string type);
		Animal(const Animal &src);
		Animal &operator=(const Animal &src);
		virtual ~Animal(void);
		std::string getType(void) const;
		virtual void makeSound(void) const;
};

#endif
