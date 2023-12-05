// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester{
public:
    void insertTest(CarDB& carDB){
        cout << "Testing insert..." << endl;
        Car car1("model1", 10, 1001, false);
        Car car2("model2", 15, 1002, false);
        Car car3("model3", 20, 1003, false);

        carDB.insert(car1);
        carDB.insert(car2);
        carDB.insert(car3);
        
        checkIndex(carDB, car1, true);
        checkIndex(carDB, car2, true);
        checkIndex(carDB, car3, true);

        checkSize(carDB, 3);

        cout << endl;
    }
    void getCarTest(CarDB& carDB){
        cout << "Testing getCar..." << endl;

        // error case: car not in table
        Car nonExistingCar = carDB.getCar("nonexistent_model", 9999);

        if (nonExistingCar == EMPTY) {
            cout << "PASS: Got EMPTY object for a non-existing Car" << endl;
        } else {
            cout << "FAIL: Got a non-empty object for a non-existing Car" << endl;
        }

        // test with non colliding cars
        int dataSize = 3;

        Car data[dataSize] = {
            Car("model1", 10, 1001, false),
            Car("model2", 15, 1002, false),
            Car("model3", 20, 1003, false)
        };

        for (int i = 0; i < dataSize; ++i) {
            carDB.insert(data[i]);
        }

        // attempt to get each Car object and check if it matches the expected value
        for (int i = 0; i < dataSize; ++i) {
            Car retrievedCar = carDB.getCar(data[i].getModel(), data[i].getDealer());
            if (retrievedCar == data[i]) {
                cout << "PASS: Successfully retrieved the Car object" << endl;
            } else {
                cout << "FAIL: Incorrect Car object retrieved" << endl;
            }
        }

        // test with colliding cars
        dataSize = 3;

        Car data2[dataSize] = {
            Car("model3", 25, 1004, false),
            Car("model2", 30, 1005, false),
            Car("model1", 35, 1006, false)
        };

        for (int i = 0; i < dataSize; ++i) {
            carDB.insert(data2[i]);
        }

        // attempt to get each Car object and check if it matches the expected value
        for (int i = 0; i < dataSize; ++i) {
            Car retrievedCar = carDB.getCar(data2[i].getModel(), data2[i].getDealer());
            if (retrievedCar == data2[i]) {
                cout << "PASS: Successfully retrieved the Car object" << endl;
            } else {
                cout << "FAIL: Incorrect Car object retrieved" << endl;
            }
        }
        cout << endl;
    }
    void removeTest(CarDB& carDB){
        cout << "Testing remove..." << endl;

        // increase size to avoid rehashing
        carDB.m_currentSize = 20;

        // test with non colliding cars
        int dataSize = 3;

        Car data[dataSize] = {
            Car("model1", 10, 1001, false),
            Car("model2", 15, 1002, false),
            Car("model3", 20, 1003, false)
        };

        for (int i = 0; i < dataSize; ++i) {
            carDB.insert(data[i]);
        }

        // attempt to remove each Car object and check if it is removed successfully
        for (int i = 0; i < dataSize; ++i) {
            if (carDB.remove(data[i])) {
                cout << "PASS: Successfully removed the Car object" << endl;
            } else {
                cout << "FAIL: Failed to remove the Car object" << endl;
            }
        }

        // test with colliding cars
        dataSize = 3;

        Car data2[dataSize] = {
            Car("model3", 25, 1004, false),
            Car("model2", 30, 1005, false),
            Car("model1", 35, 1006, false)
        };

        for (int i = 0; i < dataSize; ++i) {
            carDB.insert(data2[i]);
        }

        // attempt to remove each Car object and check if it is removed successfully
        for (int i = 0; i < dataSize; ++i) {
            if (carDB.remove(data2[i])) {
                cout << "PASS: Successfully removed the Car object" << endl;
            } else {
                cout << "FAIL: Failed to remove the Car object" << endl;
            }
        }
        cout << endl;
    }
private:
    // helper to check for correct index
    void checkIndex(CarDB& carDB, const Car& car, bool expectedResult) {
        int index = carDB.m_hash(car.getModel()) % carDB.m_currentCap;

        if (carDB.m_currentTable[index] == car && expectedResult) {
            cout << "PASS: Inserted at the correct index" << endl;
        } else {
            cout << "FAIL: Not inserted at the correct index" << endl;
        }
    }
    // helper to check for correct table size 
    void checkSize(CarDB& carDB, int expectedSize) {
        if (carDB.m_currentSize == expectedSize) {
            cout << "PASS: Data size is correct" << endl;
        } else {
            cout << "FAIL: Incorrect data size" << endl;
        }
    }

};

unsigned int hashCode(const string str);

string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};

int main(){

    Tester tester;
    CarDB testdb1(MINPRIME, hashCode, QUADRATIC);
    tester.insertTest(testdb1);

    CarDB testdb2(MINPRIME, hashCode, QUADRATIC);
    tester.getCarTest(testdb2);

    CarDB testdb3(MINPRIME, hashCode, QUADRATIC);
    tester.removeTest(testdb3);
    
    vector<Car> dataList;
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;
    
    for (int i=0;i<49;i++){
        // generating random data
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                          RndID.getRandNum(), true);
        // saving data for later use
        dataList.push_back(dataObj);
        // inserting data in to the CarDB object
        if (!cardb.insert(dataObj)) cout << "Did not insert " << dataObj << endl;
    }
    
    // dumping the data m_currentTable[index] ints to the standard output
    cardb.dump();
    // checking whether all data points are inserted
    for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == cardb.getCar((*it).getModel(), (*it).getDealer()));
    }
    if (result)
        cout << "All data points exist in the CarDB object!\n";
    else
        cout << "Some data points are missing in the CarDB object\n";
   
    return 0;
}
unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}