#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include <memory>

using namespace std;


				class Intvec
{
	public:
	explicit Intvec(size_t num = 0)
	: m_size(num), m_data(new int[m_size])
	{
		log("constructor");
	}

	~Intvec()
	{
		log("destructor");
		if (m_data) {
			delete[] m_data;
			m_data = 0;
		}
	}

	Intvec(const Intvec& other)
	: m_size(other.m_size), m_data(new int[m_size])
	{
		log("copy constructor");
		for (size_t i = 0; i < m_size; ++i)
			m_data[i] = other.m_data[i];
	}

	Intvec& operator=(Intvec&& other)
	{
		log("move assignment operator");
		std::swap(m_size, other.m_size);
		std::swap(m_data, other.m_data);
		return *this;
	}

	Intvec& operator=(const Intvec& other)
	{
		log("copy assignment operator");
		Intvec tmp(other);
		std::swap(m_size, tmp.m_size);
		std::swap(m_data, tmp.m_data);
		return *this;
	}
	private:
	void log(const char* msg)
	{
		cout << "[" << this << "] " << msg << "\n";
	}

	size_t m_size;
	int* m_data;
};

int main(int argc, char *argv[]) {

	Intvec v1(20);
	Intvec v2;

	cout << "assigning lvalue...\n";
//    v2 = v1;
	v2 = Intvec(10);
	cout << "ended assigning lvalue...\n";
	return 0;
}