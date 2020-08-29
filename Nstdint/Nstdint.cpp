#pragma once
#include "Nstdint.h"
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>

// INICIALIZACE STATICKÝCH PROMĚNNÝCH
std::vector<digittype> Nstdint::coefficients = {};
std::vector<sequencetype> Nstdint::sequence = { (sequencetype)1 };
int Nstdint::m = 0;
digittype Nstdint::a = 0;
bool Nstdint::inited = false;

// KOPÍROVÁNÍ, PŘEALOKACE, HLADOVÝ ZÁPIS
void Nstdint::deepcopy(digittype * destdigits) const
{
	for (int i = 0; i < size; i++)
		destdigits[i] = digits[i];
}

void Nstdint::expand(digittype expanddigit)
{
	// expand only if needed
	if (digits[size - 1] == 0)
		return;
	digittype* newdigits = new digittype[size + 1];
	deepcopy(newdigits);
	newdigits[size] = expanddigit; // napiš na nové místo chtěnou cifru (default = 0)
	delete[] digits; // smaz stary zapis
	this->digits = newdigits;
	size += 1; // zvětši velikost
}

void Nstdint::construct_representation(sequencetype num)
{
	// dogeneruj cleny posloupnosti abychom mohli reprezentovat num
	addElementsByValue(num);
	// najdi nejvyssi nutny clen, abychom vedeli jak bude cislo dlouhe
	std::vector<sequencetype>::iterator it = std::lower_bound(sequence.begin(), sequence.end(), num); // pozice prvniho prvku v posloupnosti, ktery neni ostre mensi 
	int index = it - sequence.begin();
	// kvuli tomu, jak funguje std::lower_bound je nutna nasledujici podminka 
	// Pripad (#): it ukazuje na prvni (indexem) clen, ktery neni ostre mensi 
	// priklad: Bn = 3Bn-1 + Bn-2, num = 42. Pak
	//				index = 3
	//						\/
	//		1	4	13	43	142
	//		B0	B1	B2	B3	B4
	// nicmene pro reprezentovani num = 42 nam staci 3 mista, takze alokujeme new basistype[3]
	// pro naslednou konstrukci zapisu pak o jedna snizime kvuli indexovani od nuly: index--; => index = 2
	// Pripad (##): pro num = 43 bude it ukazovat na stejny clen, protoze 43 neni ostre mensi nez 43
	//				index = 3
	//						\/
	//		1	4	13	43	142
	//		B0	B1	B2	B3	B4
	// pro reprezentaci 43 musime ale alokovat 4 mista = index + 1 
	// a pro konstrukci tedy nechame index roven 3.
	if ((it != sequence.end() && num == *it) || num == 0)
	{
		// pripad kdy se num zrovna trefi do nejakeho clenu posloupnosti (##)
		// num = B_index
		digits = new digittype[index + 1];
		size = index + 1;
	}
	else
	{
		// pripad kdy num < B_index (#) - num se nerovna zadnemu clenu posloupnosti
		digits = new digittype[index];
		size = index;
		index--; // posunuti na prvek, ktery je prvni mensi nez num
	}
	// sestroj hladovy zapis
	for (; num > 0 || index >= 0; --index)
	{
		// a_i = floor( A / B_i )
		digits[index] = num / sequence[index];
		// A = A - a_iB_i
		num = num % sequence[index];
	}
}

// OBSLUHA POSLOUPNOSTI
void Nstdint::init(std::vector<digittype> coefs)
{
	if (!inited)
	{
		coefficients.insert(coefficients.end(), coefs.begin(), coefs.end()); // uloz koeficienty rekurence
		m = coefficients.size(); // uloz rad rekurence
		a = coefficients[0]; // uloz nejvetsi cifru abecedy ( d = t_1 )
		addElementsByIndex(m); // vygeneruj cleny posloupnosti na zacatku
		inited = true;
	}
	else
	{
		// ochrana abychom neinicializovali vickrat
		std::cout << "cannot init: sequence was already initialized " << std::endl;
	}
}


void Nstdint::init(std::initializer_list<digittype> coefs)
{
	std::vector<digittype> coefs_vect(0);
	coefficients.insert(coefs_vect.end(), coefs.begin(), coefs.end()); // uloz koeficienty rekurence
	Nstdint::init(coefs_vect);
}

void Nstdint::reset()
{
	if (inited)
	{
		coefficients.clear(); 
		sequence.clear();
		m = 0; 
		a = 0; 
		inited = false;
	}
	else
	{
		std::cout << "cannot reset: sequence wasn't initialized " << std::endl;
	}
}

int Nstdint::addElementsByIndex(int maxElementIndex)
{
	int maxindex = sequence.size() - 1;
	for (; maxindex < maxElementIndex; ++maxindex)
	{
		sequencetype novy = 0;
		// přičti jedna k členům na začátku (které mají index kratší než řád rekurence)
		if (maxindex < m)
			novy += 1;

		// generuj členy podle obecné rekurence
		for (int i = 0; i < m && i < sequence.size(); i++)
			novy += coefficients[i] * sequence[sequence.size() - 1 - i];

		sequence.push_back(novy);
	}
	return maxindex;
}

int Nstdint::addElementsByValue(sequencetype maxValue)
{
	// adds new elements if requested value is bigger than current highest sequence element
	// the actual threshold is a bit lower, but it's there to have some extra space
	// otherwise does nothing
	int maxindex = sequence.size() - 1;
	while (sequence[maxindex] <= maxValue)
	{
		maxindex = addElementsByIndex(maxindex + 1);
	}
	return maxindex;
}

// TESTY
void Nstdint::testSequence()
{
	using namespace std;
	if (!inited)
	{
		cout << "Cannot test: Basis wasn't initialized" << endl;
		return;
	}
	
	cout << "Basis coefficients: " << endl;
	for (auto t : coefficients)
		cout << t << ", ";
	cout << endl << endl;
	cout << "Generated elements of basis sequence: " << endl;
	for (auto b : sequence)
		cout << b << ", ";
	cout << endl << endl;
}

void Nstdint::testNumbers()
{
	using namespace std;
	if (inited)
	{
		cout << "Testing numbers" << endl;
		int test_values[] = { 1, 10, 1632, 44050 };
		std::vector<Nstdint> cisla;
		for (int i : test_values)
		{
			Nstdint cislo = Nstdint(i);
			cisla.push_back(cislo);
			cout << i << "DEC = " << cislo << endl;
		}

		cout << cisla[0] << " + " << cisla[0] << " = " << cisla[0] + cisla[0] << endl;

		cout << cisla[1] << " + " << cisla[0] << " = " << cisla[1] + cisla[0] << endl;
		cout << cisla[2] << " + " << cisla[1] << " = " << cisla[2] + cisla[1] << endl;
		Nstdint x = cisla[2] + cisla[2];
		cout << cisla[2] << " + " << cisla[2] << " = " << x << endl;
		Nstdint y = x + cisla[2];
		cout << x << " + " << cisla[2] << " = " << y << endl;
		Nstdint z = x + y;
		cout << x << " + " << y << " = " << z << endl;

		cout << "Normalising a random number " << endl;
		int GGG = 10;
		int* cifry = new int[GGG];
		for (int i = 0; i < GGG; i++)
		{
			cifry[i] = (i * 7) % 10;
		}
			
		Nstdint arbitrary = Nstdint(GGG, cifry);
		arbitrary.normalise();
	}
	else
	{
		cout << "Cannot test, basis wasn't initialized" << endl;
	}
}

// KONSTRUKTORY
Nstdint::Nstdint(sequencetype num)
{
	construct_representation(num);
}

// COPY & MOVE
Nstdint & Nstdint::operator=(const Nstdint & original)
{
	if (this != &original)
	{
		// free memory
		if (digits != nullptr)
			delete[] digits;
		size = original.size;
		digits = new digittype[size];
		original.deepcopy(digits);
	}
	return *this;
}

Nstdint & Nstdint::operator=(Nstdint && original)
{
	if (this != &original)
	{
		// free memory
		if (digits != nullptr)
			delete[] digits;
		//size = 0;
		// assume ownership
		size = original.size;
		digits = original.digits;
		// deassign (free) RHS
		original.size = 0;
		original.digits = nullptr;
	}
	return *this;
}

// SROVNÁVACÍ OPERÁTORY
bool Nstdint::operator==(const Nstdint & rhs) const
{
	if (size == rhs.size)
	{
		for (int i = 0; i < size; i++)
		{
			if (digits[i] != rhs.digits[i])
				return false;
		}
		return true;
	}
	return false;
}

bool Nstdint::operator>(const Nstdint & rhs) const
{
	if (size > rhs.size)
		return true;
	else
	{
		if (size == rhs.size)
		{
			for (int i = size; i >= 0; i--) // srovnej od nejvyznamnejsi k nejmene vyznamne
			{
				if (digits[i] > rhs.digits[i])
					return true;
				else
					if (digits[i] < rhs.digits[i])
						return false;
			}
			return false; // dosli jsme na konec, rovnost
		}
		return false; // size < rhs.size - rhs delsi
	}
}

bool Nstdint::operator>=(const Nstdint & rhs) const
{
	if (size > rhs.size)
		return true;
	else
	{
		if (size == rhs.size)
		{
			for (int i = size; i >= 0; i--) // srovnej od nejvyznamnejsi k nejmene vyznamne
			{
				if (digits[i] > rhs.digits[i])
					return true;
				else
					if (digits[i] < rhs.digits[i])
						return false;
			}
			return true; // dosli jsme na konec, rovnost
		}
		return false; // size < rhs.size - rhs delsi
	}
}

// UNÁRNÍ ARITMETICKÉ OPERACE
Nstdint & Nstdint::operator+=(const Nstdint & rhs)
{
	// sečti cifru po cifře a pak nech normalizovat
	int newsize = std::max(size, rhs.size);
	if (newsize > size)
	{
		digittype* newdigits = new digittype[newsize];
		int i = 0;
		for (; i < size; ++i)
			newdigits[i] = digits[i] + rhs.digits[i];
		for (; i < newsize; ++i)
			newdigits[i] = rhs.digits[i];
		delete[] digits;
		digits = newdigits;
		size = newsize;
	}
	else
	{
		int i = 0;
		for (; i < rhs.size; ++i)
			digits[i] = digits[i] + rhs.digits[i];
	}
	normalise();
	return *this;
}

Nstdint & Nstdint::operator++()
{
	digits[0] += 1;
	if(digits[0] > a) // šetříme
		normalise();
	return *this;
}

Nstdint & Nstdint::operator*=(const unsigned & p)
{
	// vynasob cifru po cifre a pak nech normalizovat
	for (int i = 0; i < size; i++)
	{
		digits[i] = digits[i] * p;
	}
	normalise();
	return *this;
}

// NORMALIZACE
void Nstdint::normalise()
{
	using namespace std;
	cout << "Normalising number " << toString() << endl;
	int i = size - 1;
	bool normal = false;
	while (!normal)
	{
		for (; i >= 0; i--)
		{
			if (digits[i] >= a) // d = t_0 = coefficients[0]
			{
				// narazili jsme na potencialne nevhodnou cifru, jdeme zkontrolovat max. m - 1 dalsich, ktere muze byt potreba opravit
				int j = i;
				bool localOK = false;
				for (; i - j <= m - 1 && j >= 0; j--)
				{
					if (digits[j] < coefficients[i - j])
					{
						// zápis je v pořádku, jdeme zase šoupat ičkem
						cout << "digit at position j = " << j << " satisfies the greedy representation with respect to i = " << i << ", moving on" << endl;
						localOK = true;
						i = j; // doposuň se
						break;
					}
					else
					{
						if (digits[j] > coefficients[i - j])
						{
							// narazili jsme na index r = j - i, r \e {0,1,2,...,m-1}
							// pro který platí a_(i+r) > t_r a nesplňuje tedy hladový zápis
							break;
						}
						else
						{
							if (i - j == m - 1 && digits[j] == coefficients[i - j])
								break;
							if (j == 0)
								localOK = true; // ukončení
							
						}
					}
				}
				if (localOK == false)
				{
					cout << "Problem at positions i = " << i << ", j = " << j << ", starting normalisation: " << endl;
					carryNormalize(i, j);
					cout << "Corrected to: " << toString() << endl;
					i = size;
				}
			}
		}
		normal = true;
		cout << endl << "Normal form: " << toString() << endl;
	}
}

void Nstdint::carryNormalize(int i, int j)
{
	using namespace std;
	cout << toString() << endl;
	if (i - j == m - 1)
	{
		// 1)
		// zkontrolovali jsme celkem m cifer, řetězec d_i d_i-1 ... d_i-m+1 >= t_1 t_2 ... t_K, můžeme tedy odečíst
		if (i == size - 1) // prodluž zápis pokud nutno, rovnou tam napiš 1
			expand(1);
		else
			digits[i + 1] += 1;
		for (int r = 0; r < m; r++)
			digits[i - r] -= coefficients[r];
		cout << toString() << endl;
	}
	else
	{
		if (i == size - 1) // prodluž zápis pokud nutno, rovnou tam napiš 1
			expand(1);
		else
			digits[i + 1] += 1;
		cout << toString() << endl;
		int r = 0;
		// tady se pohybujeme mezi i a j, můžeme odečítat
		for (; r < i - j; r++)
		{
			digits[i - r] -= coefficients[r];
			cout << toString() << endl;
		}
		// zde už jdeme za j, musíme opatrněji
		for (; r <= m - 1; r++)
		{
			if (i - r > 0 || r == m -1)
			{
				if (digits[i - r] >= coefficients[r])
				{
					// cifra větší než, koeficient, odečítáme
					// pokud ostře větší, můžeme posunout j, abychom 
					// jedničku odečetli z nejnižšího možného řádu
					if (digits[i - r] > coefficients[r])
					{
						j = i - r; // posuň j	
					}
					digits[i - r] -= coefficients[r];
					cout << toString() << endl;
				}
				else
				{
					// narazili jsme na cifru menší než koeficient, musíme si dopůjčit
					// půjčíme si z nejbližšího čísla, které bylo ostře větší než koeficient rekurence
					// (a tedy tam zbyla aspoň jednička)
					digits[j] -= 1;
					cout << toString() << endl;
					int s = 0;
					// dojeď do r
					for (; j - 1 - s < i - r; s++)
					{
						// dopřičti přes již navštívené cifry (mezi j a r)
						digits[j - 1 - s] += coefficients[s];
						cout << toString() << endl;
					}
					for (; r <= m - 1; s++, r++)
					{
						// přičti i odečítej
						digits[j - 1 - s] = digits[j - 1 - s] - coefficients[r] + coefficients[s];
						cout << toString() << endl;
						if (j - 1 - s == 0)
						{
							// - 1 + 1 = 0, nic nepřičítáme, až na případ r == m - 1
							// stačí takováto podmínka, případ s == r nenastane, vždy platí s < r
							if (r == m - 1) 
								digits[0] += 1;
							return;
						}
					}
					for (; s <= m - 1; s++)
					{
						// zde už jsme r "vyčerpali" - r == m - 1, 
						// už jen přičítáme
						digits[j - 1 - s] += coefficients[s];
						cout << toString() << endl;
						if (j - 1 - s == 0 && s < m - 1)
						{
							// pokud jsme dorazili do nejnižšího řádu a s < m - 1, nutno dopřičíst 1
							digits[0] += 1;
							cout << toString() << endl;
							return;
						}
					}
					return;
				}
			}
			else
			{
				// víme: i - r == 0 && r < m - 1
				if (digits[i - r] >= coefficients[r] + 1)
				{
					digits[i - r] = digits[i - r] - coefficients[r] - 1;
					cout << toString() << endl;
					return;
				}
				else
				{
					// na 0. řádu jsme narazili na cifru menší než koeficient + 1, musíme si dopůjčit
					// půjčíme si z nejbližšího čísla, které bylo ostře větší než koeficient rekurence
					// (a tedy tam zbyla aspoň jednička)
					digits[j] -= 1;
					cout << toString() << endl;
					int s = 0;
					// dojeď do r
					for (; j - 1 - s < i - r; s++)
					{
						// dopřičti přes již navštívené cifry (mezi j a r)
						digits[j - 1 - s] += coefficients[s];
						cout << toString() << endl;
					}
					//for (; r <= m - 1; s++, r++)
					//{
						// přičti i odečítej
						digits[j - 1 - s] = digits[j - 1 - s] - coefficients[r] + coefficients[s];
						cout << toString() << endl;
						if (j - 1 - s == 0)
						{
							// - 1 + 1 = 0, nic nepřičítáme
							return;
						}
					//}
					return;
				}
			}
			
		}
	}
}

// KONVERZE
int Nstdint::toInt() const
{
	int vysledek = 0;
	for (int i = 0; i < size; i++)
	{
		vysledek += digits[i] * sequence[i];
	}
	return vysledek;
}

long long Nstdint::toLongLong() const
{
	long long vysledek = 0;
	for (int i = 0; i < size; i++)
	{
		vysledek += digits[i] * sequence[i];
	}
	return vysledek;
}

// VÝPIS
char Nstdint::digitToChar(digittype digit)
{
	// konverze cislic pro citelnost (pro cifry < 62)
	// aby zabírala jenom jedno místo a nemátla při výpisu
	if (digit < 10)
		return '0' + digit;
	else if (digit < 36)
		return 'A' - 10 + digit;
	else
		return 'a' - 10 - 26 + digit;
}

digittype Nstdint::charToDigit(char c)
{
	// konverze cislic pro citelnost (pro cifry < 62)
	// aby zabírala jenom jedno místo a nemátla při výpisu
	if (isdigit(c))
		return digittype(c - '0');
	else if (isupper(c))
		return digittype(c - 'A' + 10);
	else if (islower(c))
		return digittype(c - 'a' + 10 + 26);
}

std::string Nstdint::toString() const
{
	// vypis ve spravnem poradi ( nejvyznamnejsi cifra prvni)
	std::stringstream sstream;
	for (int i = size - 1 ; i >= 0; i--) 
		sstream << digitToChar(digits[i]);
	return sstream.str();
}

std::ostream & operator<<(std::ostream & out, const Nstdint& x)
{
	out << x.toString();
	return out;
}

// BINÁRNÍ ARITMETICKÉ OPERACE
Nstdint operator+(const Nstdint & a, const Nstdint & b)
{
	Nstdint c = a;
	c += b;
	return c;
}

Nstdint operator*(const Nstdint & a, const unsigned & p)
{
	Nstdint c = a;
	c *= p;
	return c;
}

//void Nstdint::init(std::vector<digittype> coefs)
//{

//}

