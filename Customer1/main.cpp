#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <memory>
#include <iterator>

using namespace std;

class Customer
{
public:
    int customer_numb;
    map <string,string> name;
    int age;
    string address;
    bool married;
    int family_size;
    string occupation;
    Customer(int a):customer_numb(a){};

    void set_c_name(string a,string b)
    {
        name.emplace(make_pair(a,b));
    }
    void print_name(void)
    {

        cout << name.begin()->first << ' ' << name.begin()->second;
    }
    int getAge(void)
    {
        return this->age;
    }
    int getFamily(void)
    {
        return this->family_size;
    }

};

bool compare(Customer* a, Customer* b)
{
    return a->age < b->age;
}


void print_customer_data(vector <Customer*>const  a, int offset)   //prints 1 customers data
{
    auto it = a.begin();
    cout << "Name: ";
    it[offset]->print_name();
    cout << endl;
    cout << "Age: " << it[offset]->age << endl;
    cout << "Address: " << it[offset]->address << endl;
    cout << "Married: " << boolalpha << it[offset]->married << endl;
    cout.unsetf(ios::boolalpha);
    cout << "Family Size: " << it[offset]->family_size << endl;
    cout << "Occupation: " << it[offset]->occupation << endl;
}
bool check_input(void)
{
    if (!cin || cin.fail())    // check if input is bad
    {
        // user didn't input a number
        cin.clear();     // reset failbit
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
        // next, request user reinput
        cout << "... bad input ..." << endl;
        return false;
    }
    else
        return true;
}

string get_file_loc (void)
{
    cout << "---------------------------------------------------------" << endl;
    cout << "Before we begin, drag the -  customers.txt - " << endl <<
         "file into the terminal window and press enter" << endl;
    cout << "---------------------------------------------------------" << endl;

    string file_name;

    while (1)
    {
        cin >> file_name;
        fstream file(file_name);
        if (!file.is_open() || !check_input()) // check for incorrect input
        {
            cout << "---------------------------------------------------------" << endl;
            cout << "Unable to open file";
            cout << "Try again. Drag the file into the terminal window" << endl;
            cout << "and press enter." << endl;
            cout << "---------------------------------------------------------" << endl;
            continue;
        }
        else
        {
            file.close();
            break;
        }

    }

    return file_name;
}
int main()
{
    string file_name = get_file_loc();
    fstream file(file_name);
    if (!file.is_open() || !check_input()) // check for incorrect input
    {
        cout << "Unable to open file";
        return 1;
    }
    file.seekg(0, file.beg);

    int customer_numb;
    vector<Customer*> database;

    int row_counter = 0;
    string line;
    string temp;
    int numb = 0;
    cout << endl;
    //for some reason there are 3 hidden characters at the start of file
    char c;
    file.get(c);
    file.get(c);
    file.get(c);


    while(file.good())
    {
        getline(file, line);
        if(file.eof())
            break;
        std::stringstream ss(line);

        database.push_back(new Customer(numb));
        getline(ss, line, ',');           //last name field
        auto it = database.begin();

        temp = line;

        getline(ss, line, ',');       //first name field
        it[numb]->set_c_name(temp, line);

        getline(ss, line, ',');           //age field
        it[numb]->age = stoi(line);
        getline(ss, line, ',');           //address field
        it[numb]->address = line;
        getline(ss, line, ',');           //married field
        if (line == "true")
            it[numb]->married = true;
        else
            it[numb]->married = false;
        getline(ss, line, ',');           //family size field
        it[numb]->family_size = stoi(line);
        getline(ss, line, '\r');          //occupation field
        it[numb]->occupation = line;
        numb++;
    }


    bool run = true;
    while(run)                               // user interface
    {
        cout << endl << "There are " << database.size() <<
             " customers in the database." << endl;
        cout << "Enter 1 to print a specific record." << endl;
        cout << "Enter 2 to print all records." << endl;
        cout << "Enter 3 to compare attributes of each customer" << endl;
        cout << "Enter 4 sorting customers based on age" << endl;
        cout << "Enter 5 to quit" << endl;

        int response;
        cin >> response;

        if(check_input())    // check for incorrect input
        {
            if (response == 1)    //print specific record
            {
                cout << "Which record would you like to see? 0 - " << (database.size() - 1) << endl;
                while (run)
                {
                    cin>>response;
                    if (check_input())    // check for incorrect input
                    {
                        if (response >= 0 && response <= (database.size() - 1))
                        {
                            print_customer_data(database, response);
                            response = 1000;
                            break;
                        }
                    }
                    cout << "Sorry, it needs to be a record between 0 - " << database.size() - 1 << endl;
                    response = 1000;
                }

            }
            if (response == 2)        // print all records
            {
                for (int i = 0; i < database.size(); ++i)
                {
                    print_customer_data(database, i);
                    cout << endl;
                }
            }
            if (response == 3)        // find average
            {
                cout << "Enter 1 view average customer age." << endl;
                cout << "Enter 2 view average customer family size" << endl;
                while (run)
                {
                    cin >> response;
                    if (check_input())    // check for incorrect input
                    {
                        if (response == 1 || response == 2)
                        {
                            if (response == 1)       // average age
                            {
                                int average_age = 0;
                                for (int i = 0; i < database.size(); ++i)
                                {
                                    auto itr1 = database.begin();
                                    average_age += itr1[i]->getAge();
                                    cout << average_age << endl;

                                }
                                float temp_f1 = (float)average_age / (float)database.size();
                                cout << "The average customer age = " << (float)temp_f1 << endl;
                                response = 1000;
                                break;
                            } else                    // average family size
                            {
                                int averagef = 0;
                                for (int i = 0; i < database.size(); ++i)
                                {
                                    auto itr2 = database.begin();
                                    averagef += itr2[i]->getFamily();
                                }
                                float temp_f2 = (float)averagef / (float)database.size();
                                cout << "The average customer family size = " << (float)temp_f2 << endl;
                                response = 1000;
                                break;
                            }
                        }
                    }
                    cout << "Error. Answer needs to be 1 for average age, and 2 for average family size." << endl;
                    response = 1000;
                }
            }
            if (response == 4)       // sort
            {
                cout << "Sorted based on age.";
                auto it1 = database.begin();
                auto it2 = database.begin();
                sort(database.begin(),database.end(),compare);

            }
            if (response == 5)       // quit
            {
                cout << "...Quitting..." << endl;
                break;
            }
        }
    }
    return 0;
}