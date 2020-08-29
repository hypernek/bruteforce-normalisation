// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vector>
#include <sstream>
#include <iostream>
#include "Nstdint.h"


void test()
{
	Nstdint::init({ 5, 4, 3, 3 });
	Nstdint::addElementsByIndex(10);
	Nstdint::testSequence();
	Nstdint::testNumbers();

	Nstdint::reset();

	Nstdint::init({ 2, 1 });
	Nstdint::testSequence();
	Nstdint::testNumbers();

	Nstdint::reset();
	Nstdint::init({ 4, 1 });
	Nstdint::testSequence();

	digittype * digits = new digittype[8]{ 4,0,6,0,4,2,6,1 };
	Nstdint w(8, digits);
	w.normalise();

	std::cout << w << " * 5 = " << w * 5 << std::endl;
}

bool check_Confluent(std::vector<digittype> coeffs)
{
	digittype a = coeffs[0];
	if (a < 1)
		return false;
	for (int i = 0; i < coeffs.size()-1; i++)
	{
		if (coeffs[i] != a)
			return false;
	}
	if (coeffs[coeffs.size() - 1] > a || coeffs[coeffs.size() - 1] < 1)
	{
		return false;
	}
	return true;
}

bool check_F(std::vector<digittype> coeffs)
{
	// check (F) property of basis, i. e.
	// check whether basis coefficients satisfy
	// t_1 >= t_2 >= ... >= t_m >= 1.
	if (coeffs[0] < 1)
		return false;
	for (int i = 0; i < coeffs.size() - 1; i++)
	{
		if (coeffs[i] < coeffs[i+1])
			return false;
	}
	if (coeffs[coeffs.size() - 1] < 1)
		return false;
	return true;
}

void normaliseInput(std::string input_word)
{
	// check if it is digits (0-9, A-Z, a-z allowed)
	for (int i = 0; i < input_word.length(); i++)
	{
		if (!isalnum(input_word[i]))
		{
			std::cout << "Invalid input, " << input_word[i] << " is not a digit" << std::endl;
			return;
		}
	}
	// proceed with normalisation
	digittype *digits = new digittype[input_word.length()];
	for (int i = 0; i < input_word.length(); i++)
	{
		digits[i] = Nstdint::charToDigit(input_word[i]);
		std::cout << "Next digit: " << input_word[i] << " = " << digits[i] << std::endl;
	}

	Nstdint x(input_word.length(), digits);
	x.normalise();
}

void acceptInput()
{
	while (true)
	{
		bool Basis_ready = false;
		std::vector<digittype> Basis_coeffs(0);
		while (!Basis_ready)
		{
			std::cout << "\nPlease input coefficients of basis, separated by comma: " << std::endl;
			std::string coeffs;
			std::getline(std::cin, coeffs);
			std::stringstream ss(coeffs);

			while (ss.good())
			{
				std::string coef;
				std::getline(ss, coef, ','); // split off next coefficient
				bool coef_good = true;
				std::cout << "coef: " << coef << std::endl;
				// check if it is really a number
				for (int i = 0; i < coef.length(); i++)
				{
					if (!isdigit(coef[i]))
					{
						std::cout << "Invalid input, " << coef[i] << " is not a digit" << std::endl;
						coef_good = false;
						break;
					}
				}

				if (coef_good)
				{
					digittype coefficient = std::stoi(coef);
					Basis_coeffs.push_back(coefficient);
				}
				else
				{
					break;
				}
			}
			if (!ss.good() && !Basis_coeffs.empty())
			{
				Basis_ready = true;
				std::cout << "Proceeding with coefficients: " << Basis_coeffs[0];
				for (int i = 1; i < Basis_coeffs.size(); i++)
				{
					std::cout << ", " << Basis_coeffs[i];
				}
				std::cout << std::endl;
			}
		}

		bool is_F = check_F(Basis_coeffs);
		if (!is_F)
		{
			std::cout << "Basis is NOT (F). Please enter an (F) basis." << std::endl;
			continue;
		}
		std::cout << "Basis satisfies the (F) property. Proceeding with construction of basis..." << std::endl;

		Nstdint::init(Basis_coeffs);

		while (true)
		{
			std::string input_w;
			std::cout << "\nEnter next word to normalise: (allowed digits: 0-9, A-Z, a-z), enter % to start over with a new basis" << std::endl;
			std::getline(std::cin, input_w);
			if (input_w == "%")
			{
				Nstdint::reset();
				break;
			}
			else
			{
				normaliseInput(input_w);
			}
		}
	}	
}

int main()
{
	//test();	
	acceptInput();
	
}


