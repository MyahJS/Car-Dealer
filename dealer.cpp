// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    // Constructor
    // preconditions: a size and hash function are passed
    // postconditions: a new hash table is created
    //  make sure the size of the table is a prime number between MAXPRIME and MINPRIME
    //  allocate memory for the table and initialize all member variables
    if (size < MINPRIME){
        m_currentCap = MINPRIME;
    } else if (size > MAXPRIME){
        m_currentCap = MAXPRIME;
    } else {
        m_currentCap = findNextPrime(size);
    }
        
    m_hash = hash;
    m_newPolicy = NONE;

    m_currProbing = probing;
    m_currentTable = new Car[m_currentCap];
    for (int i = 0; i < m_currentCap; ++i)
        m_currentTable[i] = EMPTY;
    m_currentSize = 0;
    m_currNumDeleted = 0;

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
    if (m_currentTable)
        delete[] m_currentTable;
    if (m_oldTable)
        delete[] m_oldTable;  
}

void CarDB::changeProbPolicy(prob_t policy){
    // ChangeProbPolicy
    // precondtitions: hash table exists
    // postconditions: set passed collision handling policy to m_newPolicy
    m_newPolicy = policy;
}

bool CarDB::insert(Car car){
    // Insert
    // preconditions:
    // postconditions:

    // check if id is valid
    if (car.getDealer() < MINID || car.getDealer() > MAXID)
        return false;

    // find first index using hash function
    int index = m_hash(car.getModel()) % m_currentCap;
    int org_index = index;

    // find an unoccupied index using the specified probing strategy
    int i = 0;
    if (m_currProbing==QUADRATIC){
        while (!(m_currentTable[index]==EMPTY)){
            if (i>=m_currentCap)
                return false;
            if (m_currentTable[index]==car) // check for dup
                return false;
            index = ((org_index % m_currentCap) + (i*i)) % m_currentCap;
            i++;
        }
        m_currentTable[index] = car;
    } else {
        while (!(m_currentTable[index]==EMPTY)){
            if (i>=m_currentCap)
                return false;
            if (m_currentTable[index]==car) // check for dup
                return false;
            index = ((org_index % m_currentCap) + i * (11 - (org_index % 11))) % m_currentCap;
            i++;
        }
        m_currentTable[index] = car;
    }

    // rehash here 
    if (m_oldTable==nullptr){
        if (lambda()>0.5){
            m_oldTable = m_currentTable;
            delete[] m_currentTable;

            m_oldCap = m_currentCap;
            m_currentCap = findNextPrime(m_currentCap*4);
            m_currentTable = new Car[m_currentCap];

            m_oldNumDeleted = m_currNumDeleted;
            m_currNumDeleted = 0;
            
            m_oldProbing = m_currProbing;
            m_currProbing = (m_newPolicy==NONE) ? m_currProbing : m_newPolicy;

            m_oldSize = m_currentSize;
            m_currentSize = 0;

            // transfer first 25% of cars
            int portion = m_oldSize/4;
            int i = 0;
            int j = 0;
            while (j<portion){
                if (m_oldTable[i].getUsed()==true){
                    // insert into new table
                    index = m_hash(m_oldTable[i].getModel()) % m_currentCap;
                    org_index = index;
                    int k = 0;
                    if (m_currProbing==QUADRATIC){
                        while (!(m_currentTable[index]==EMPTY)){
                            index = ((org_index % m_currentCap) + (k*k)) % m_currentCap;
                            k++;
                        }
                    } else {
                        while (!(m_currentTable[index]==EMPTY)){
                            index = ((org_index % m_currentCap) + k * (11-(org_index % 11))) % m_currentCap;
                            k++;
                        }
                    }
                    m_currentTable[index] = m_oldTable[i];
                    m_oldTable[i].m_used = false;   // mark transfered cars
                    m_currentSize++;
                    j++;
                }
                i++;
            }
        }
    } else {
        // transfer the next 25% of cars
        int portion = m_oldSize/4;
        if (m_currentSize>=(portion*3)){
            portion = m_oldSize;
        } else {
            portion = portion + m_currentSize;
        }
        int i = 0;
        int j = m_currentSize;
        while (j<portion){
            if (m_oldTable[i].getUsed()==true){
                // insert into new table
                index = m_hash(m_oldTable[i].getModel()) % m_currentCap;
                org_index = index;
                int k = 0;
                if (m_currProbing==QUADRATIC){
                    while (!(m_currentTable[index]==EMPTY)){
                        index = ((org_index % m_currentCap) + (k*k)) % m_currentCap;
                        k++;
                    }
                } else {
                    while (!(m_currentTable[index]==EMPTY)){
                        index = ((org_index % m_currentCap) + k * (11-(org_index % 11))) % m_currentCap;
                        k++;
                    }
                }
                m_currentTable[index] = m_oldTable[i];
                m_oldTable[i].m_used = false;   // mark transfered cars
                m_oldNumDeleted++;
                m_currentSize++;
                j++;
            }
            i++;
        }
        // check if rehash complete
        if (m_currentSize>=m_oldSize)
            delete[] m_oldTable;
            m_oldCap = 0;
            m_oldNumDeleted = 0;
            m_oldProbing = NONE;
            m_oldSize = 0;
            m_oldTable = nullptr;
    }
    return true;
}

bool CarDB::remove(Car car){
    // Remove
    // preconditions:
    // postconditions:

    // find first index using hash function
    int index = m_hash(car.getModel());
    int index2 = index;
    int i = 0;
    // find node in current or old tree and set as deleted
    if (m_oldTable==nullptr){
        if (m_currProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                i++;
            }
        } else {
            while (!(m_currentTable[index]==car) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                i++;
            }
        }
        if (m_currentTable[index]==car){
            m_currentTable[index].m_used = false;
            m_currNumDeleted++;
        } else {
            return false;
        }
    } else {
        if (m_currProbing==QUADRATIC && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + (i*i)) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==QUADRATIC && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY))){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + (i*i)) % m_oldCap;
                i++;
            }
        }
        if (m_currentTable[index]==car){
            m_currentTable[index].m_used = false;
            m_currNumDeleted++;
        } else if (m_oldTable[index2]==car){
            m_oldTable[index2].m_used = false;
            m_oldNumDeleted++;
        } else {
            return false;
        }
    }

    // rehash here
    if (m_oldTable==nullptr){
        if (deletedRatio()>0.8){
            m_oldTable = m_currentTable;
            delete[] m_currentTable;

            m_oldCap = m_currentCap;
            m_currentCap = findNextPrime(m_currentCap*4);
            m_currentTable = new Car[m_currentCap];

            m_oldNumDeleted = m_currNumDeleted;
            m_currNumDeleted = 0;
            
            m_oldProbing = m_currProbing;
            m_currProbing = (m_newPolicy==NONE) ? m_currProbing : m_newPolicy;

            m_oldSize = m_currentSize;
            m_currentSize = 0;

            // transfer first 25% of cars
            int portion = m_oldSize/4;
            int i = 0;
            int j = 0;
            int index;
            while (j<portion){
                if (m_oldTable[i].getUsed()==true){
                    // insert into new table
                    index = m_hash(m_oldTable[i].getModel());
                    int k = 0;
                    if (m_currProbing==QUADRATIC){
                        while (!(m_currentTable[index]==EMPTY)){
                            index = ((m_hash(m_oldTable[i].getModel()) % m_currentCap) + (k*k)) % m_currentCap;
                            k++;
                        }
                    } else {
                        while (!(m_currentTable[index]==EMPTY)){
                            index = ((m_hash(m_oldTable[i].getModel()) % m_currentCap) + k * (11-(m_hash(m_oldTable[i].getModel()) % 11))) % m_currentCap;
                            k++;
                        }
                    }
                    m_currentTable[index] = m_oldTable[i];
                    m_oldTable[i].m_used = false;   // mark transfered cars
                    m_currentSize++;
                    j++;
                }
                i++;
            }
        }
    } else {
        // transfer the next 25% of cars
        int portion = m_oldSize/4;
        if (m_currentSize>=(portion*3)){
            portion = m_oldSize;
        } else {
            portion = portion + m_currentSize;
        }
        int i = 0;
        int j = m_currentSize;
        int index;
        while (j<portion){
            if (m_oldTable[i].getUsed()==true){
                // insert into new table
                index = m_hash(m_oldTable[i].getModel());
                int k = 0;
                if (m_currProbing==QUADRATIC){
                    while (!(m_currentTable[index]==EMPTY)){
                        index = ((m_hash(m_oldTable[i].getModel()) % m_currentCap) + (k*k)) % m_currentCap;
                        k++;
                    }
                } else {
                    while (!(m_currentTable[index]==EMPTY)){
                        index = ((m_hash(m_oldTable[i].getModel()) % m_currentCap) + k * (11-(m_hash(m_oldTable[i].getModel()) % 11))) % m_currentCap;
                        k++;
                    }
                }
                m_currentTable[index] = m_oldTable[i];
                m_oldTable[i].m_used = false;   // mark transfered cars
                m_oldNumDeleted++;
                m_currentSize++;
                j++;
            }
            i++;
        }
        // check if rehash complete
        if (m_currentSize>=m_oldSize)
            delete[] m_oldTable;
            m_oldCap = 0;
            m_oldNumDeleted = 0;
            m_oldProbing = NONE;
            m_oldSize = 0;
            m_oldTable = nullptr;
    }
    return true;
}

Car CarDB::getCar(string model, int dealer) const{
    // GetCar
    // preconditions:
    // postconditions:

    // get first index using hash function
    int index = m_hash(model);
    int index2 = index;
    int i = 0;

    // check if old table is empty
    if (m_oldTable==nullptr){
        if (m_currProbing==QUADRATIC){
            while (!(m_currentTable[index].getDealer()==dealer) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(model) % m_currentCap) + (i*i)) % m_currentCap;
                i++;
            }
        } else {
            while (!(m_currentTable[index].getDealer()==dealer) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(model) % m_currentCap) + i * (11-(m_hash(model) % 11))) % m_currentCap;
                i++;
            }
        }
        if (m_currentTable[index].getDealer()==dealer){
            return m_currentTable[index];
        } else {
            return EMPTY;
        }
    } else {
        if (m_currProbing==QUADRATIC && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY) && !(m_currentTable[index].getDealer()==dealer) && !(m_oldTable[index2].getDealer()==dealer)){
                index = ((m_hash(model) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(model) % m_oldCap) + (i*i)) % m_oldCap;
                i++; 
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY) && !(m_currentTable[index].getDealer()==dealer) && !(m_oldTable[index2].getDealer()==dealer)){
                index = ((m_hash(model) % m_currentCap) + i * (11-(m_hash(model) % 11))) % m_currentCap;
                index2 = ((m_hash(model) % m_oldCap) + i * (11-(m_hash(model) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==QUADRATIC && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY) && !(m_currentTable[index].getDealer()==dealer) && !(m_oldTable[index2].getDealer()==dealer)){
                index = ((m_hash(model) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(model) % m_oldCap) + i * (11-(m_hash(model) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY) && !(m_currentTable[index].getDealer()==dealer) && !(m_oldTable[index2].getDealer()==dealer)){
                index = ((m_hash(model) % m_currentCap) + i * (11-(m_hash(model) % 11))) % m_currentCap;
                index2 = ((m_hash(model) % m_oldCap) + (i*i)) % m_oldCap;
                i++;
            }
        }
        if (m_currentTable[index].getDealer()==dealer){
            return m_currentTable[index];
        } else if (m_oldTable[index2].getDealer()==dealer){
            return m_oldTable[index2];
        } else {
            return EMPTY;
        }
    }
}

float CarDB::lambda() const {
    // Lambda
    // preconditions:
    // postconditions:
    return m_currentSize/m_currentCap;
}

float CarDB::deletedRatio() const {
    // DeletedRatio
    // preconditions:
    // postconditons:
    return m_currNumDeleted/m_currentSize;    
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
    // UpdateQuantity
    // preconditions:
    // postconditions:

    // find first index using hash function
    int index = m_hash(car.getModel());
    int index2 = index;
    int i = 0;
    // find node in current or old tree and set quantity
    if (m_oldTable==nullptr){
        if (m_currProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                i++;
            }
        } else {
            while (!(m_currentTable[index]==car) && !(m_currentTable[index]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                i++;
            }
        }
        if (m_currentTable[index]==car){
            m_currentTable[index].setQuantity(quantity);
            return true;
        } else {
            return false;
        }
    } else {
        if (m_currProbing==QUADRATIC && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + (i*i)) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==QUADRATIC && m_oldProbing==DOUBLEHASH){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car) && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY)){
                index = ((m_hash(car.getModel()) % m_currentCap) + (i*i)) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_oldCap;
                i++;
            }
        } else if (m_currProbing==DOUBLEHASH && m_oldProbing==QUADRATIC){
            while (!(m_currentTable[index]==car) && !(m_oldTable[index2]==car && !(m_currentTable[index]==EMPTY) && !(m_oldTable[index2]==EMPTY))){
                index = ((m_hash(car.getModel()) % m_currentCap) + i * (11 - (m_hash(car.getModel()) % 11))) % m_currentCap;
                index2 = ((m_hash(car.getModel()) % m_oldCap) + (i*i)) % m_oldCap;
                i++;
            }
        }
        if (m_currentTable[index]==car){
            m_currentTable[index].setQuantity(quantity);
            return true;
        } else if (m_oldTable[index2]==car){
            m_oldTable[index2].setQuantity(quantity);
            return true;
        } else {
            return false;
        }
    }
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