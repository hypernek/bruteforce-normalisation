#pragma once
#include <string>
#include <ostream>
#include <vector>
#include <initializer_list>

typedef int digittype;
typedef int sequencetype;

// Nstdint = integer in non-standard representation (B-representation)
class Nstdint
{
private:
	// DATA
	unsigned size;
	digittype * digits;

	// STATIC VARIABLES
	static std::vector<digittype> coefficients; // coefficients of reccurence
	static std::vector<sequencetype> sequence; // basis
	static int m; // degree of basis recurrence
	static digittype a; // largest digit
	static bool inited;

	// KOPÍROVÁNÍ, PŘEALOKACE, HLADOVÝ ZÁPIS, NORMALIZACE
	void deepcopy(digittype* destdigits) const;  // prekopiruje zapis
	void expand(digittype expanddigit = 0); // prealokuje cifry do pole o 1 vetsi (pouzivame pri preteceni)
	void construct_representation(sequencetype num); // sestroji pocatecni hladovy zapis	
	void carryNormalize(int i, int j);

public:
	// OBSLUHA POSLOUPNOSTI
	static void init(std::vector<digittype> coefs);
	static void init(std::initializer_list<digittype> coefs); // so that we can write coefficients into code
	static void reset();
	static int addElementsByIndex(int maxElementIndex); // dopocitej cleny podle radu
	static int addElementsByValue(sequencetype maxValue); // dopocitej cleny podle hodnoty
	
	// TESTY
	static void testSequence();
	static void testNumbers();

	// KONSTRUKTORY, DESTRUKTOR
	Nstdint() : size(1), digits(new digittype(0)) {};
	Nstdint(unsigned size, digittype* digits) : size(size), digits(digits) {};
	Nstdint(sequencetype num);
	~Nstdint() { size = 0; delete[] digits; };

	// COPY & MOVE
	Nstdint(const Nstdint& original) : size(0), digits(nullptr) { *this = original; };
	Nstdint(Nstdint&& original) : size(0), digits(nullptr) { *this = std::move(original); };
	Nstdint& operator=(const Nstdint& original);
	Nstdint& operator=(Nstdint&& original);
	
	// SROVNÁVACÍ OPERÁTORY
	bool operator==(const Nstdint &rhs) const;
	bool operator!=(const Nstdint &rhs) const { return !(*this == rhs); };
	bool operator>(const Nstdint &rhs) const;
	bool operator>=(const Nstdint &rhs) const;
	bool operator<(const Nstdint &rhs) const { return !(*this >= rhs); };
	bool operator<=(const Nstdint &rhs) const { return !(*this > rhs); };

	// UNÁRNÍ ARITMETICKÉ OPERACE
	Nstdint& operator+=(const Nstdint& rhs);
	Nstdint& operator++();
	Nstdint& operator*=(const unsigned& p);

	// NORMALIZACE
	void normalise();

	// KONVERZE
	int toInt() const;
	long long toLongLong() const;

	// VÝPIS
	static char digitToChar(digittype digit);
	static digittype charToDigit(char c);
	std::string toString() const;
	friend std::ostream& operator<<(std::ostream& out, const Nstdint& x);

	
};

// BINÁRNÍ ARITMETICKÉ OPERACE
Nstdint operator+(const Nstdint& a, const Nstdint& b);
//Nstdint& operator-(const Nstdint& a, const Nstdint& b);
Nstdint operator*(const Nstdint& a, const unsigned& p);
//Nstdint operator*(const unsigned& p, const Nstdint& a) { return a * p; };


//
