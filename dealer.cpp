// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    // Constructor
    // preconditions: a size and hash function are passed
    // postconditions: a new hash table is created
    //  make sure the size of the table is a prime number between MAXPRIME and MINPRIME
    //  allocate memory for the table and initialize all member variables
    if (MINPRIME<=size<=MAXPRIME) {
        if (isPrime(size)){
            m_currentCap = size;
        } else {
            m_currentCap = findNextPrime(size);
        }
    } else {
        m_currentCap = (size<MINPRIME) ? MINPRIME : MAXPRIME;
    }
    m_hash = hash;
    m_currProbing = probing;
    m_currentTable = new Car[m_currentCap];
    m_currentSize = 0;
    m_currNumDeleted = 0;

    m_newPolicy = NONE;

    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = NONE;
}

CarDB::~CarDB(){
    // Destructor
    // preconditions: hash table exists
    // postconditions: deallocate memory
    if (m_currentTable){
        delete[] m_currentTable;
        m_currentTable = nullptr;
    } 
    if (m_oldTable){
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }  
}

void CarDB::changeProbPolicy(prob_t policy){
    
}

bool CarDB::insert(Car car){
    
}

bool CarDB::remove(Car car){
    
}

Car CarDB::getCar(string model, int dealer) const{
    
}

float CarDB::lambda() const {
      
}

float CarDB::deletedRatio() const {
    
}

void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool CarDB::updateQuantity(Car car, int quantity){
    
}

bool CarDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Car &car ) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity<< ")";
    else
        sout << "";
  return sout;
}

bool operator==(const Car& lhs, const Car& rhs){
    // since the uniqueness of an object is defined by model and delaer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}