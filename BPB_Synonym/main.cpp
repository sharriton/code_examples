#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <istream>
#include <set>
#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

/*************************************************
    BPB Synonym 1.0
    Author: Steven Harriton, 2018
    email: stevenharriton@gmail.com

    Changes:
    2018-10-10: fixed errors from input miscalculation
    2018-10-23: increased input speed from BPB file
 *************************************************/

using namespace std;

void get_track(FILE * bpb);

class CSV
{
public:
//    int field_number;
    int moods_index;
    int length_index;
    char buffer;
    CSV(fstream &bpb)
    {
        moods_index = 28;
        length_index = 26;
        buffer = '\0';
    }
    void skip_to_field(fstream &bpb, int collumn_loc);
};


void fill(fstream &bpb,vector<string> &a);   // adding data from each field
void fill2(fstream &bpb,vector<string> &a);  // adding data from last field in row
void find_keywords(set<string> a[],vector <string> b[],int bpb_rows, int size);
int BPB_rows_cnt;
int number_of_rows(fstream &keywords);
int get_month (void);
int get_year (void);
void concat_date(char * text_file_name, char file_extension);
void create_dir (void);
void print(string);

int main(void)
{
    fstream bpb("./bpb.csv");                      // open bpb
    if (!bpb.is_open())
        cout << "Unable to open file";

    CSV BPB(bpb);                                  // create BPB object

    int counter = number_of_rows(bpb);             // get bpb number of rows
    BPB_rows_cnt = counter-1;                      // account for the fact that row 1 is column titles

    bpb.clear();
    bpb.seekg(0, bpb.beg);

    size_t index = 9999;
    string *mood = new string[BPB_rows_cnt];

    for (int i = 0; i < BPB_rows_cnt; ++i)    //  copying mood field to string
    {
        BPB.buffer = '\0';
        BPB.skip_to_field(bpb, BPB.moods_index);
        getline(bpb, mood[i], '\"');
        bpb.ignore(10000, '\r');
        cout << i << ". " << mood[i] << endl;
    }

    set <string> *a = new set<string>[BPB_rows_cnt];   // final collection for moods

    for (int j = 0; j < BPB_rows_cnt; ++j)
    {
        long comma_cntr = count(mood[j].begin(),mood[j].end(),',');
        long word_start = 0;
        long word_len = mood[j].find(',');
        for (int i = 0; i < comma_cntr + 1; ++i)
        {
            if (comma_cntr == (i))
                word_len = mood[j].length();
            a[j].insert(mood[j].substr(word_start, abs(word_len - word_start)));
            word_start = word_len + 2;
            word_len = (mood[j].find(',', word_len + 1));
        }
    }

    for (int k = 0; k < BPB_rows_cnt; ++k)     //print all mood data
    {
        cout << k << ".) ";
        for_each(a[k].begin(), a[k].end(), print);
        cout << endl;
    }

    bpb.seekg(0, bpb.beg);

    vector<string> *bpb_import= new vector<string>[BPB_rows_cnt];
    for(int j = 0; j < BPB_rows_cnt ; ++j)
    {
        for (int i = 0; i <179;i++)
        {
            if(i < 178)
                fill(bpb, bpb_import[j]);    // importing data with ", as delimiter
            else
                fill2(bpb, bpb_import[j]);   // importing last field with \r\n endline delimiter
        }
    }

    bpb.close();

    fstream keywords("./keywords1.csv");
    if (!keywords.is_open())
        cout << "Unable to open file";
    int keys = number_of_rows(keywords)+1;

    if(keywords.eof())              //checking to make sure keywords successfully reset stream pos to beginning
    {
        cout << " END OF FILE ERROR!!";
        return 0;
    }

    string *keywords_full_rows = new string[keys];

    counter = 0;
    while (keywords.good())
    {
        getline(keywords, keywords_full_rows[counter], '\n');   // pulling in each row of keywords
        ++counter;
    }

    string temp = "";
    vector<string> *key_vec = new vector<string>[keys] ;        // breaking up string, pushing it to vector
    for (int i1 = 0; i1 < keys; ++i1)
    {
        for (int l = 0; l < keywords_full_rows[i1].length(); ++l)
        {
            temp += keywords_full_rows[i1][l];
            if(keywords_full_rows[i1][l] == ',')
            {
                temp.resize(temp.length() - 1);
                key_vec[i1].emplace_back(temp);
                temp = "";
            }
            if(l == keywords_full_rows[i1].length()-1 )
            {
                key_vec[i1].emplace_back(temp);
                temp = "";
            }
        }
    }

    find_keywords(a, key_vec, BPB_rows_cnt, keys);    // adding new keywords to mood field with conditions

    keywords.close();

    ofstream output;
    string filename = "bpb_mood_update.csv";  // creating new csv with updated mood field

    output.open(filename);
    if (!output.is_open())
        cout << "Unable to open file";

    vector<string>:: iterator bpb_it;
    set<string>:: iterator mood_it;

    for (int l1 = 0; l1 < BPB_rows_cnt; ++l1)    // writing data to new csv
    {
        bpb_it = bpb_import[l1].begin();
        mood_it = a[l1].begin();
        for (int k1 = 0; k1 < 179; ++k1)
        {
                output << *bpb_it;       // going through each field, pasting data from the BPB catalogue
                ++bpb_it;

            if (k1 == 27)                // this is the mood field
            {
                if((*mood_it).length() == 0) // if its blank
                    output << "\"\",";
                else
                {
                    output << '\"';     // if its not blank first add quotes- mood vector has no quotes
                    for (int i = 0; i < 50; ++i)  // iterate through all elements of mood field
                    {

                        output << *mood_it;
                        mood_it++;
                        if (mood_it == a[l1].end())    //if reach end of mood data
                            break;
                        else
                            output << ", ";           // adds comma space between mood elements

                    }

                    output << "\",";      // add field delimiter
                }
                bpb_it++;                 // skips mood field of BPB_import
            }
        }
    }
    return 0;
}


int number_of_rows(fstream &keywords)
        {
                int counter = 0;
                while(keywords.ignore(10000,'\r'))
                {
                    ++counter;
                }
            keywords.clear();
            keywords.seekg(0,keywords.beg);
                return counter;
        };


void concat_date(char * text_file_name, char * file_extension)
{
    char year[6]="\0";
    char month[3]="\0";
    sprintf(year,"_%d", get_year ());
    sprintf(month,"_%d", get_month ());
    strcat(text_file_name,month);
    strcat(text_file_name,year);
    strcat(text_file_name,file_extension);
}
int get_month (void)
{
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    int month = tm.tm_mon + 1;

    return month;
}

int get_year (void)
{
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    int year = tm.tm_year + 1900;

    return year;
}

void CSV::skip_to_field(fstream &bpb, int collumn_loc)
{
            for (int delim_count = 0; delim_count < collumn_loc - 1;delim_count)
            {
                bpb.get(buffer);
                if (buffer == '\"')
                {
                    bpb.get(buffer);
                    if (buffer == ',')
                    {
                        bpb.get(buffer);
                        if (buffer == '\"')
                            delim_count++;
                    }
                }
            }
}

void print(string s)
{
    cout << s << ",";
}

void fill(fstream &bpb,vector<string> &a)
{
    string field;
    string temp;
    char c;
    int flag = 0;
    while (bpb.get(c))
    {
        temp += c;
        if (c == '\"' && flag == 1)   // continue until delimiter
        {
            bpb.get(c);
            temp += c;
            if (c == ',')
                break;
        }
        flag = 1;
    }
    field += temp;
    a.emplace_back(field);
}

void fill2(fstream &bpb,vector<string> &a)
{
    string field;
    string temp;
    char c;
    int flag = 0;
    while (!bpb.eof())
    {
        bpb.get(c);
        temp += c;
        if (c == '\"' && flag == 1)
        {
            bpb.get(c);
            temp += c;

            break;
        }
        flag = 1;

    }
    bpb.get(c);
    field += temp;
    a.emplace_back(field);
}


void find_keywords(set<string> a[],vector <string> b[],int bpb_rows, int keys)
{
    set<string>::iterator a_it;
    vector<string>::iterator b_it;
    int flag = 0;

    for (int j = 0; j < bpb_rows; ++j)          // iterate through each row of the bpb catalogue
    {
        for (int i = 0; i < keys; ++i)          // iterate through each row of keys
        {
            b_it = b[i].begin();
            while (b_it != b[i].end())         // while theres still key words to iterate through:
            {
                a_it = find(a[j].begin(), a[j].end(), *b_it);   // check if each key word is in collection a
                if (a_it != a[j].end())                         // if it is, then:
                {
                    flag = 1;                                   // set flag
                    break;
                } else
                    b_it++;
            }
            if (flag == 1)                             // if keyword inside collection a
            {
                b_it = b[i].begin();                   // reset iterator to start of potential added words
                while (b_it != b[i].end())             // iterate through all new keywords to add to collection a
                {
                    a[j].insert(*b_it);
                    b_it++;
                }
            }
        }
    }
}

