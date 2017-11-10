#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace std::chrono;


// -----------------------------------------------------------------------------
// POLYNOMIAL
// -----------------------------------------------------------------------------

class Polynomial {
public:
    Polynomial(float * constants, int degree) : constants(constants), degree(degree) {};

    Polynomial * trivialMultiplication(const Polynomial & rPolynom) ;

    Polynomial * karatsubaMultiplication(const Polynomial & rPolynom) ;

    friend ostream & operator << (ostream & os, const Polynomial & p);

    float getConstantAtIndex(int index) const { return constants[index]; }

    float * getConstants() const { return constants; }

    int getDegree() const { return degree; }

    int getNumConstants() const { return degree + 1; }
private:
    float * constants;
    int degree;

    float * karatsuba(float * left, float * right, int size);
    float * sumPolynomial(float * left, float * right, int size);
    float * fillConstants(float * constants, int size);
    int degreePowerOfTwo(int degree) const;
};


// -----------------------------------------------------------------------------
// POLYNOMIAL IMPL
// -----------------------------------------------------------------------------


Polynomial * Polynomial::trivialMultiplication(const Polynomial & rPolynom)  {

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    float * result = new float[this->getDegree() + rPolynom.getDegree() + 1];

    for(int i = 0; i < this->getNumConstants(); i++) {
        for(int j = 0; j < rPolynom.getNumConstants(); j++) {
            result[i + j] += constants[i] * rPolynom.getConstantAtIndex(j);
        }
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    printf("Trivial algorithm duration: %f seconds\n", time_span.count());

    return new Polynomial(result, this->getDegree() + rPolynom.getDegree());
}

Polynomial * Polynomial::karatsubaMultiplication(const Polynomial & rPolynom)  {

    if (this->getDegree() != rPolynom.getDegree()) {
        return nullptr;
    }

    int realSize = degreePowerOfTwo(this->getNumConstants());

    float * left;
    float * right;

    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    float * result;
    if (realSize != this->getNumConstants()) {
        left = fillConstants(this->constants, realSize);
        right = fillConstants(rPolynom.getConstants(), realSize);

        t1 = high_resolution_clock::now();
        result = karatsuba(left, right, realSize);
        t2 = high_resolution_clock::now();

        delete [] left;
        delete [] right;
    } else {
        t1 = high_resolution_clock::now();
        result = karatsuba(this->constants, rPolynom.getConstants(), this->getNumConstants());
        t2 = high_resolution_clock::now();
    }
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    printf("Kartasuba algorithm duration: %f seconds\n", time_span.count());

    return new Polynomial(result, realSize*2);
}

float * Polynomial::karatsuba(float * left, float * right, int size)  {

    int resultSize = size * 2 + 1;

    float * result = new float[resultSize];
    for (int i = 0; i < resultSize; i++) {
        result[i] = 0;
    }

    if (size == 1) {
        result[0] = left[0] * right[0];
        return result;
    }

    int halfSize = size/2;

    float * leftLowP = left;
    float * rightLowP = right;
    float * leftHighP = &left[halfSize];
    float * rightHighP = &right[halfSize];

    float * p0 = karatsuba(leftLowP, rightLowP, halfSize);
    //memory leak for sumPolynomial
    float * p1 = karatsuba(sumPolynomial(leftLowP, leftHighP, halfSize), sumPolynomial(rightLowP, rightHighP, halfSize), halfSize);
    float * p2 = karatsuba(leftHighP, rightHighP, halfSize);

    for(int i = 0; i < size; i++) {
        result[i] += p0[i];
        result[i + size] += p2[i];
        result[i + halfSize] += p1[i] - p2[i] - p0[i];
    }

    delete [] p0;
    delete [] p1;
    delete [] p2;

    return result;

}

float * Polynomial::sumPolynomial(float * left, float * right, int size)  {

    float * result = new float[size];
    for (int i = 0; i < size; i++) {
        result[i] = left[i] + right[i];
    }

    return result;
}

int Polynomial::degreePowerOfTwo(int degree) const {
    int power = 1;

    while(power < degree) {
        power *= 2;
    }
    return power;
}

float * Polynomial::fillConstants(float * constants, int size) {

    int rightDegree = degreePowerOfTwo(size-1);
    float * newConstants;

    if(rightDegree != size) {
        newConstants = new float[rightDegree];

        for(int i = 0; i < rightDegree; i++) {
            if(i < size) {
                newConstants[i] = constants[i];
            } else {
                newConstants[i] = 0;
            }
        }
    } else {
        newConstants = constants;
    }

    return newConstants;
}

ostream & operator << (ostream & os, const Polynomial & p) {

    for(int i = 0; i < p.getNumConstants(); i++) {
        if (i == 0) {
            os << p.constants[0] << " ";
        } else if (p.constants[i] > 1) {
            os << " + " << p.constants[i] << "x^" << i;
        } else if (p.constants[i] < -1) {
            os << " - " << abs(p.constants[i]) << "x^" << i;
        } else if (p.constants[i] == 1) {
            os << " + x^" << i;
        } else if (p.constants[i] == -1) {
            os << " - x^" << i;
        }
    }

    return os;
}


// -----------------------------------------------------------------------------
// POLYNOMIAL READ
// -----------------------------------------------------------------------------


Polynomial * readPolynomial() {

    int degree = 0;
    cout << "Degree of polynomial: ";
    scanf("%d", &degree);

    float * constants = new float[degree + 1];

    cout << "Ascending polynomial's constants: ";
    for(int i = 0; i < (degree + 1); i++) {
        int constant = 0;
        if(scanf("%d", &constant) == 1) {
            constants[i] = constant;
        } else {
            return nullptr;
        }
    }


    return new Polynomial(constants, degree);
}


Polynomial * getPolynomial(string coef, int degree) {

    float * constants = new float[degree + 1];

    string value = "";
    int indexCoef = 0;
    for(int i = 0; i <= coef.size(); i++) {
        if(coef[i] == ' ' || coef[i] == '\0') {
            constants[indexCoef] = stof(value);
            indexCoef++;
            value = "";
        } else if(isdigit(coef[i]) || coef[i] == '.' || coef[i] == '-') {
            value.push_back(coef[i]);
        } else {
            return nullptr;
        }
    }

//    for(int i = 0; i < degree + 1; i++) {
//        cout << constants[i] << " ";
//    }
//    cout << endl;

    return new Polynomial(constants, degree);
}


int getDegree(string degreeStr) {

    for(int i = 0; i < degreeStr.size(); i++) {
        if(!isdigit(degreeStr[i])) {
            return -1;
        }
    }

    return atoi(degreeStr.c_str());
}

Polynomial * readPolynomial(ifstream & fs) {
    string degree;
    string coef;

    getline(fs, degree);
    int numDegree = getDegree(degree);
    if (numDegree == -1) {
        return nullptr;
    }

    getline(fs, coef);
    Polynomial * polynomial = getPolynomial(coef, numDegree);
    if(polynomial == nullptr) {
        return nullptr;
    }

    return polynomial;
}


// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------


int main(int argc,  char **argv) {

    Polynomial * firstPolynomial;
    Polynomial * secondPolynomial;
    if(argc == 1) {
        firstPolynomial = readPolynomial();
        secondPolynomial = readPolynomial();
    } else {
        ifstream fs(argv[1], ios::in);
        if (fs.fail()) {
            cout << "Cannot read input file." << endl;
            return 1;
        }

        firstPolynomial = readPolynomial(fs);
        secondPolynomial = readPolynomial(fs);
    }

    if(firstPolynomial == nullptr || secondPolynomial == nullptr) {
        cout << "Doesn't conform to our polynomial pattern." << endl;
        return -1;
    }

    Polynomial * resultTrivial = firstPolynomial->trivialMultiplication(*secondPolynomial);
    Polynomial * resultKartasuba = firstPolynomial->karatsubaMultiplication(*secondPolynomial);

    cout << *resultTrivial << endl;
    cout << *resultKartasuba << endl;

    return 0;
}

