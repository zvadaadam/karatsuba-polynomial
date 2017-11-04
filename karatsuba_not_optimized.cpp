#include <iostream>
#include <cmath>

using namespace std;


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

    int degreePowerOfTwo() const;
private:
    float * constants;
    int degree;

    float * karatsuba(float * left, float * right, int size);
    float * sumPolynomial(float * left, float * right, int size);
};

Polynomial * Polynomial::trivialMultiplication(const Polynomial & rPolynom)  {

    float * result = new float[this->getDegree() + rPolynom.getDegree() + 1];

    for(int i = 0; i < this->getNumConstants(); i++) {
        for(int j = 0; j < rPolynom.getNumConstants(); j++) {
            result[i + j] += constants[i] * rPolynom.getConstantAtIndex(j);
        }
    }

    return new Polynomial(result, this->getDegree() + rPolynom.getDegree());
}

Polynomial * Polynomial::karatsubaMultiplication(const Polynomial & rPolynom)  {

    if (this->getDegree() != rPolynom.getDegree()) {
        return nullptr;
    }

    float * left = this->constants;
    float * right = rPolynom.getConstants();

    float * result = karatsuba(left, right, this->getNumConstants());

    for (int i = 0; i < this->getDegree()*2; i++) {
        cout << result[i] << " ";
    }
    cout << endl;

    return new Polynomial(result, this->getDegree()*2);
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
    float * p1 = karatsuba(sumPolynomial(leftLowP, leftHighP, halfSize), sumPolynomial(rightLowP, rightHighP, halfSize), halfSize);
    float * p2 = karatsuba(leftHighP, rightHighP, halfSize);

    for(int i = 0; i < size; i++) {
        result[i] += p0[i];
        result[i + size] += p2[i];
        result[i + halfSize] += p1[i] - p2[i] - p0[i];
    }

    return result;

}

float * Polynomial::sumPolynomial(float * left, float * right, int size)  {

    float * result = new float[size];
    for (int i = 0; i < size; i++) {
        result[i] = left[i] + right[i];
    }

    return result;
}

int Polynomial::degreePowerOfTwo() const {
    int power = 1;

    while(power < this->degree) {
        power *= 2;
    }
    return power;
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


int main() {

    Polynomial * firstPolynomial = readPolynomial();
    if(firstPolynomial  == nullptr) {
        return 1;
    }
    cout << "Polynomial_One: " << *firstPolynomial << endl;
    cout << firstPolynomial->degreePowerOfTwo() << endl;

    Polynomial * secondPolynomial = readPolynomial();
    if(secondPolynomial   == nullptr) {
        return 1;
    }
    cout << "Polynomial_Two: " << *secondPolynomial  << endl;

    cout << *firstPolynomial->trivialMultiplication(*secondPolynomial) << endl;
    cout << *firstPolynomial->karatsubaMultiplication(*secondPolynomial) << endl;


    return 0;
}