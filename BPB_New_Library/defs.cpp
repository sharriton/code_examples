//
// Created by Steven Harriton on 12/27/18.
//

#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <type_traits>
#include "declarations.h"

using namespace BPB;

bool navigate_to_sites()
{
    std::cout << "\n<All PRO csv's must be downloaded and in the same folder as the BPB_New_Library .exe>";
    std::cout << "\nWould you like to navigate to the PRO websites? y/n>\n";
    string c;
    getline(std::cin,c);
    if (c[0] == 'n')
        return false;
    else
        return true;

}

void bpb::ask_user_if_print()
{
    std::cout << "\n<Would you like to print the BPB titles before proceeding? y/n>\n";
    string c;
    getline(std::cin,c);
    if (c[0] == 'y')
        print_titles(*this);
}

bool track::operator==(const std::vector<track>:: iterator right)
{
    return title == right->title;
}

bool track::operator==(const string & a)
{
    return title == a;
}

int number_of_rows(std::fstream &keywords)
{
    int counter = 0;
    while(keywords.ignore(10000,'\r'))
    {
        ++counter;
    }
    keywords.clear();
    keywords.seekg(0,keywords.beg);
    return counter;
}

int number_of_rows(BPB::FILES &file)
{
    string temp{};
    int row_cntr{};
    while(file.f.good())
    {
        getline(file.f,temp, '\n');
        ++row_cntr;
    }
    file.f.clear();
    file.f.seekg(0,file.f.beg);
    return row_cntr;
}

void ascap_import_data_and_search_titles(BPB::bpb & bpb,std::string && path)   // import ascap and search
{
    BPB::FILES csv(path);
    int ascap_row_numb = number_of_rows(csv.f) / 2; // every title is listed at least twice so /2
                                                    // we don't need to allocate memory for the repeated titles
    BPB::ascap data(csv.PRO_type(),ascap_row_numb); //
    data.data_parse(csv);

    int matches {};
    auto i = bpb.track_data.begin();
    auto should_have_ascap_iterator = bpb.track_data.begin();

    for (int j = 0; j < std::size(bpb.track_data); ++j)
    {
        auto found = find(data.track_data.begin(), data.track_data.end(), i);

        if (found != data.track_data.end())
        {
            if(should_have_ascap_iterator->should_have_reg_with_ascap)
            {
                string temp{};
                std::stringstream ss;
                ss<<*i->writer.begin();
                ss>>temp;
                auto find_writer_name = find(found->writer.begin(),found->writer.end(),temp);
                if(find_writer_name != found->writer.end())
                {
                    (bpb.track_data.begin() + j)->found_in_ascap = true;
                    matches++;
                }
            }
        }
        ++i;
        ++should_have_ascap_iterator;
    }
}

void sesac_import_data_and_search_titles(BPB::bpb & bpb,std::string && path)   // import sesac and search
{
    BPB::FILES csv(path);
    BPB::sesac data(csv.PRO_type());
    data.data_parse(csv);

    int matches {};
    auto i = bpb.track_data.begin();
    auto should_have_sesac_iterator = bpb.track_data.begin();

    for (int j = 0; j < std::size(bpb.track_data); ++j)
    {
        auto found = find(data.track_data.begin(), data.track_data.end(), i);

        if (found != data.track_data.end())
        {
            if(should_have_sesac_iterator->should_have_reg_with_sesac)
            {
                string temp{};
                std::stringstream ss;
                ss<<*i->writer.begin();
                ss>>temp;
                auto find_writer_name = find(found->writer.begin(),found->writer.end(),temp);
                if(find_writer_name != found->writer.end())
                {
                    (bpb.track_data.begin() + j)->found_in_sesac = true;
                    matches++;
                }
            }
        }
        ++i;
        ++should_have_sesac_iterator;
    }
}

void bmi_import_data_and_search_titles(BPB::bpb bpb)        // import bmi file version !
{
    BPB::FILES bmi_csv("./bmi.csv");
    int size = number_of_rows(bmi_csv) / 2;
    BPB::bmi bmi_data(bmi_csv.PRO_type(),size);
    bmi_data.data_parse(bmi_csv);

    int matches {};
    auto bpb_iterator_1 = bpb.titles_adjusted_for_bmi.begin();
    auto bpb_iterator_2 = bpb.track_data.begin();

    for (int j = 0; j < std::size(bpb.titles_adjusted_for_bmi); ++j)
    {
        auto found = find(bmi_data.track_data.begin(), bmi_data.track_data.end(), *bpb_iterator_1);

        if (found != bmi_data.track_data.end())
        {
            if(bpb_iterator_2->should_have_reg_with_bmi)
            {
            string temp{};
            std::stringstream ss;
            ss<< *(bpb.track_data.begin()+j)->writer.begin();
            ss>>temp;
            auto find_writer_name = find(found->writer.begin(),found->writer.end(),temp);
                if(find_writer_name != found->writer.end())
                {
                    (bpb.track_data.begin() + j)->found_in_bmi = true;
                    matches++;
                }
            }
        }
        ++bpb_iterator_1;
        ++bpb_iterator_2;
    }
}

void bpb::are_these_registered()
{
    bool registered_ascap = false;
    bool registered_sesac = false;
    bool registered_bmi = false;
    int need_updated_registrations{};
    int unregistered_cnt{};

    auto should_have_ascap_iterator = track_data.begin();
    auto should_have_sesac_iterator = track_data.begin();
    auto should_have_bmi_iterator = track_data.begin();

    std::ofstream results;
    std::string file_name {"results.txt"};
    results.open(file_name);
    if (!results)
        throw std::system_error(errno, std::system_category(), "failed to open " + file_name);

    for(const auto & track : track_data)
    {
        if(track.found_in_ascap && should_have_ascap_iterator->should_have_reg_with_ascap)
            registered_ascap = true;
        if(track.found_in_sesac && should_have_ascap_iterator->should_have_reg_with_sesac)
            registered_sesac = true;
        if(track.found_in_bmi && should_have_ascap_iterator->should_have_reg_with_bmi)
        {
            registered_bmi = true;
        }

        if((!registered_ascap) && should_have_ascap_iterator->should_have_reg_with_ascap)
        {
            results << "\n" << track.title << " - ascap registration missing";
        }
        if((!registered_sesac) && should_have_ascap_iterator->should_have_reg_with_sesac)
        {
            results << "\n" << track.title << " - sesac registration missing";
        }
        if((!registered_bmi) && should_have_ascap_iterator->should_have_reg_with_bmi)
        {
            results << "\n" << track.title << " - bmi registration missing";
        }
        if((should_have_ascap_iterator->should_have_reg_with_ascap && !track.found_in_ascap) || (should_have_ascap_iterator->should_have_reg_with_bmi && !track.found_in_bmi) ||
        (should_have_ascap_iterator->should_have_reg_with_sesac && !track.found_in_sesac))
            unregistered_cnt++;
        if((should_have_ascap_iterator->should_have_reg_with_ascap == track.found_in_ascap) && (should_have_ascap_iterator->should_have_reg_with_bmi == track.found_in_bmi) &&
           (should_have_ascap_iterator->should_have_reg_with_sesac == track.found_in_sesac))
            need_updated_registrations++;
        ++should_have_ascap_iterator;
        ++should_have_sesac_iterator;
        ++should_have_bmi_iterator;

    }
    results << "\n<<<" << need_updated_registrations << " tracks need updated registrations!>>>\n";
}

void ask_for_user_to_download(int dest,string destination)
{
    std::cout << "\n<Press Enter to open up the " + destination + " repository website>\n";
    switch(dest)
    {
        case 1:
            std::cin.ignore();
            ascap_download();
            break;
        case 2:
            std::cin.ignore();
            sesac_download();
            break;
        case 3:
            std::cin.ignore();
            bmi_download();
            break;
    }
}

vector<string> ascap::push_pro_info_1st_time(FILES &a,int index)
{
    string temp{};
    vector<string> temp_v {};
    for (int i = 0; i < 3; ++i)
    {
        getline(a.f, temp, ',');         // read PRO info 1-3
        temp_v.push_back(temp);          // assign PRO info 1-3
    }
    return temp_v;
}

vector<string> ascap::push_pro_info(FILES &a,int index,string old)
{
    string temp = old;
    vector<string> temp_v {};
    for (int i = 0; i < 2; ++i)
    {
        temp_v.push_back(temp);          // assign PRO info 1-2

        getline(a.f, temp, ',');         // read PRO info 1-3
    }
            temp_v.push_back(temp);      // assign PRO info 3
    return temp_v;
}

void clean_string(string & temp)
{
    temp.erase(std::remove(temp.begin(),temp.end(),'\n'),temp.end());
    temp.erase(std::remove(temp.begin(),temp.end(),'\r'),temp.end());
    temp.erase(std::remove(temp.begin(),temp.end(),'\"'),temp.end());
    temp.erase(std::remove(temp.begin(),temp.end(),'\0'),temp.end());

    auto i = temp.end() - 1;                                           // find the first trailing ' '
    for(;*i == ' ';--i)
    {}
    if(i != temp.end() - 1)
        temp.erase(std::remove(i,temp.end(),' '),temp.end());          // delete trailing spaces
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// All data_parse functions are unique. The csv file formats for each PRO are completely
// different and cannot be parse the same. I hold the data field by field in temp variables.
// Once I have the data I want for each track, I push_back into the track_data vector
// using {} initializer list, which creates a new track object in the vector.
/////////////////////////////////////////////////////////////////////////////////////////////////
void ascap::data_parse(FILES &a)
{

    int index {};
    string temp {}, temp_title{};
    vector <string> temp_pub{}, temp_writer{}, temp_writer_pro{}, temp_pro{}, temp_share{};

    bool first_time = true;

    getline(a.f, temp);                        // ignoring first line of
    while(getline(a.f, temp, ','))             // read in title
    {
        if(first_time)                         // only do it this way first time
        {
            clean_string(temp);                // remove trailing ' ' and any \n and \r
            temp_title = temp;                 // assign title
            temp_writer_pro = push_pro_info_1st_time(a,index); // read and assign pro info 1st time only
            first_time = false;
        }
        else
            temp_writer_pro = push_pro_info(a,index,temp);     // read and assign pro with this function every time after 1st time

        getline(a.f, temp, ',');               // read and discard second use of same title

        for (int i = 0; i < 15; ++i)           // get variable number of writers and publishers
        {
            getline(a.f, temp, ',');           // this will either be W for writer or P for publisher
            if (temp == "W")
            {
                getline(a.f, temp, ',');
                temp_writer.push_back(temp);   // add composer name
            }
            else if (temp == "P")
            {
                getline(a.f, temp, ',');
                temp_pub.push_back(temp);      // add publisher name
            }
            getline(a.f, temp, ',');           // empty
            getline(a.f, temp, ',');           // grab title

            clean_string(temp);                //remove trailing ' ' and any \n and \r
            if (temp != temp_title)            // if its a different title, add it to next index and import its data
            {
                ++index;

                // push_back all the data of one composition of music into the vector
                track_data.push_back({++index,temp_title,{temp_writer},{temp_writer_pro},{temp_pub},{temp_pro},{temp_share}
                ,false,false,false,false,false,false});
                temp_title = temp;             // next track title
                getline(a.f, temp);            // skip to next line
                                               //reset all variables to prepare for parsing next tracks data
                temp_writer.clear();
                temp_writer_pro.clear();
                temp_pub.clear();
                temp_pro.clear();
                temp_share.clear();
                first_time = true;
                break;
            }
        }
    }
}

void sesac::data_parse(FILES &a)
{
    int index {};
    bool first_time = true;

    string temp {};
    string temp_title{};
    string temp_pro_s{};
    string temp_pub_or_writer{};
    string temp_share_s{};

    vector <string> temp_pub{}, temp_writer{}, temp_writer_pro{}, temp_pro{}, temp_share{};

    getline(a.f, temp);                              // ignoring first line of

    while(getline(a.f, temp, ','))                   // skip work ip number
    {
        getline(a.f, temp, ',');                     // get title
        clean_string(temp);                          // remove trailing ' ' and any \n and \r
        if(first_time)                               // first time we only assign title
        {

            temp_title = temp;                       // title assignment
            first_time = false;
        }
        else if (temp != temp_title)                 // if not equal it means we have a new track title
        {
            // push_back all the data of one composition of music into the vector
            track_data.push_back({++index,temp_title,{temp_writer},{temp_writer_pro},{temp_pub},{temp_pro},
                                  {temp_share},false,false,false,false,false,false});

            temp_title = temp;                       // new track title
            temp_pub.clear();
                                                     //reset all variables to prepare for parsing next tracks data
            temp_writer.clear();
            temp_writer_pro.clear();
            temp_pro.clear();
            temp_share.clear();
            first_time = true;
        }

        for (int i = 0; i < 3; ++i)
        {
            getline(a.f, temp, ',');                 // get and ignore reg date, ISWC, and AFF#
        }
            getline(a.f, temp, ',');
            clean_string(temp);
            getline(a.f, temp_pro_s, ',');
            getline(a.f, temp_pub_or_writer, ',');
            getline(a.f, temp_share_s, '\n');
            clean_string(temp_share_s);

            if (temp_pub_or_writer == "\"W\"")
            {
                temp_writer.push_back(temp);
                temp_writer_pro.push_back(temp_pro_s);
                temp_share.push_back(temp_share_s);
            }
            else if (temp_pub_or_writer == "\"P\"")
            {
                temp_pub.push_back(temp);
                temp_pro.push_back(temp_pro_s);
                temp_share.push_back(temp_share_s);
            }
    }
}

void bmi::data_parse(FILES &a)
{
    int index {};
    bool first_time = true;

    string temp {}, temp_title{};
    string temp_name_writ_or_pub{};
    string temp_pro_s{};
    string temp_pub_or_writer{};
    string temp_share_s{};

    vector <string> temp_pub{}, temp_writer{}, temp_writer_pro{}, temp_pro{}, temp_share{};

    getline(a.f, temp);                              // skip first line
    getline(a.f, temp, ',');                         // ignoring title number
    getline(a.f, temp, ',');                         // ignoring song number

    while(getline(a.f, temp, ','))                   // ignoring work id
    {
        getline(a.f, temp, ',');                     // get title
        clean_string(temp);                          // remove trailing ' ' and any \n and \r
        if(first_time)                               // first time we only assign title
        {
            temp_title = temp;
            first_time = false;                      // signifies first iteration of a new title
        }
        else if (temp != temp_title)                 // if not equal it means we have a new track title
        {
            // push_back all the data of one composition of music into the vector
            track_data.push_back({++index,temp_title,{temp_writer},{temp_writer_pro},{temp_pub},
                                  {temp_pro},{temp_share},false,false,false,false,false,false});
            temp_title = temp;
            temp_writer.clear();
            temp_writer_pro.clear();
            temp_pub.clear();
            temp_pro.clear();
            temp_share.clear();
            first_time = true;
        }

        getline(a.f, temp_pub_or_writer, ',');
        getline(a.f, temp, ',');                     // ignore pub type

        getline(a.f, temp_name_writ_or_pub, ',');    // will be name of writer or publisher
        clean_string(temp_name_writ_or_pub);

        getline(a.f, temp_share_s, ',');             // share %
        clean_string(temp_share_s);
        getline(a.f, temp, ',');                     // ignore IP name #
        getline(a.f, temp_pro_s, ',');               // get pro name
        clean_string(temp_pro_s);

        for (int i = 0; i < 2; ++i)
        {
            getline(a.f, temp, ',');                 // ignore reg date, ISWC
        }

        if (temp_pub_or_writer == "W")
        {
            temp_writer.push_back(temp_name_writ_or_pub);
            temp_writer_pro.push_back(temp_pro_s);
            temp_share.push_back(temp_share_s);
        }
        else if (temp_pub_or_writer == "P")
        {
            temp_pub.push_back(temp_name_writ_or_pub);
            temp_pro.push_back(temp_pro_s);
            temp_share.push_back(temp_share_s);
        }

        getline(a.f, temp, ',');                     // ignore garbage line
        getline(a.f, temp, ',');                     // ignore garbage line
    }
}

std::string bpb::get_one_field(FILES &a)
{
    char c{};
    string temp_s{};

    while(a.f.get(c))
    {
        if(c == '\"')
        {
            a.f.get(c);
            if(c == ',')
            {
                a.f.get(c);
                if(c == '\"')
                {
                    return temp_s;
                }
            }
        }
        temp_s += c;
    }
    return temp_s;
}

void bpb::data_parse(FILES &a)
{
    int index {2};
    string temp {}, temp_title{};
    vector <string> temp_pub{}, temp_writer{}, temp_writer_pro{}, temp_pro{}, temp_share{};

    bool should_have_reg_with_ascap{false};
    bool should_have_reg_with_sesac{false};
    bool should_have_reg_with_bmi{false};
//    for (int j = 0; j < 20000; ++j)
//    {
        getline(a.f, temp);                              // skip first line
//        index++;
//    }
    size_t csv_cols_count = stringCount(temp,"\",\"");
    if(csv_cols_count > 178 || csv_cols_count < 178)           // if the bpb.csv has the wrong number of rows exit
        error_message("BPB.csv contains incorrect number of columns");

    while(index < bpb::row_cnt)
    {
        string test_s_id = get_one_field(a);                                      // ignoring source id
        if(!is_Integer(test_s_id))
        {
            std::cout << test_s_id << "\n";
            error_message("Source ID", "Not an integer. Probably includes illegal characters letters.", index);
        }

        temp_title = get_one_field(a);                         // get title
        clean_string(temp_title);                              // remove trailing ' ' and any \n and \r
        std::transform(temp_title.begin(), temp_title.end(), temp_title.begin(), ::toupper);
//        std::cout << "\n" << index << " " << temp_title;

        get_one_field(a);                                    // ignore composer description

        for (int j = 0; j < 43; ++j)                     // skip 30 fields to get to publisher 1
        {
            temp = get_one_field(a);
        }

        for (int pub_num = 0; pub_num < 6; ++pub_num)
        {
            temp_pub.push_back(temp);            // add publisher 1 - 6
//            std::cout << "\n" << temp;
            clean_string(temp);

            temp = get_one_field(a);             // add publisher pro 1 - 6
            if(! IsLetters(temp))
                error_message("Publisher PRO","Contains non-alphabet characters",index,pub_num);

            temp_pro.push_back(temp);
            if(temp == "ASCAP")
                should_have_reg_with_ascap = true;
            if(temp == "BMI")
                should_have_reg_with_bmi = true;
            if(temp == "SESAC")
                should_have_reg_with_sesac = true;

//            std::cout << "\n" << temp;

            for (int j = 0; j < 6; ++j)         // skip to next pub field -
            {                                   // on last repeat of k and last repeat of j temp is
                temp = get_one_field(a);        // assigned with writer 1 first name
            }
        }
        for (int i = 0; i < 4; ++i)
        {
            temp_writer.emplace_back(temp += " " + get_one_field(a));   // writer # 1

            get_one_field(a);                                           // ignore writer one company
            temp_writer_pro.emplace_back(get_one_field(a));

            get_one_field(a);                                           // ignore CAE / IPI
            string t_share = get_one_field(a);
            if(!is_Integer(t_share))
            {
                error_message("Writer Share", "Not an integer or float. Probably includes letters", index);
            }
            temp_share.emplace_back(t_share);

            get_one_field(a);                                           // ignore writer publisher and writer role
            get_one_field(a);

            temp = get_one_field(a);
         }
//        for(const auto & writer : temp_writer)
//        { std::cout << "\n" << writer; }

        track_data.push_back({++index,temp_title,{temp_writer},{temp_writer_pro},{temp_pub},{temp_pro},{temp_share}
        ,false,false,false,should_have_reg_with_ascap,should_have_reg_with_sesac,should_have_reg_with_bmi});

        getline(a.f, temp);                              // skip to next line
        temp_title.clear();
        temp_writer.clear();
        temp_writer_pro.clear();
        temp_pub.clear();
        temp_pro.clear();
        temp_share.clear();
        should_have_reg_with_ascap=false;
        should_have_reg_with_sesac=false;
        should_have_reg_with_bmi=false;
    }
}

void remove_A(string & title)
{
    if(title.substr(0,2) == "A ")
        title = title.substr(2,title.size()-2);
}

void remove_THE(string & title)
{
    if(title.substr(0,4) == "THE ")
        title = title.substr(4,title.size()-4);
}

void remove_APOSTR(string & title)
{
    auto b = title.find('\'');
    if(b != std::string::npos)
        title[b] = ' ';
}

void remove_COMMA(string & title)
{
    auto b = title.find(',');
    if(b != std::string::npos)
        title[b] = ' ';
}

void remove_DASH(string & title)
{
    auto b = title.find('-');
    if(b != std::string::npos)
        title[b] = ' ';
}
void remove_PARENTH(string & title)
{
    title.erase(std::remove(title.begin(),title.end(),'('),title.end());
    title.erase(std::remove(title.begin(),title.end(),')'),title.end());
}

void bpb::title_adjustments(void)
{
    for (auto i = track_data.begin(); i < track_data.end()-1; ++i)
    {
        string temp_title = i->title;
        remove_A(temp_title);
        remove_THE(temp_title);
        remove_APOSTR(temp_title);
        remove_COMMA(temp_title);
        remove_DASH(temp_title);
        remove_PARENTH(temp_title);
        titles_adjusted_for_bmi.push_back(temp_title);
//        std::cout << "\n" << "OLD = " << i->title << "  NEW = " << temp_title;
    }
}

void ascap_download()
{
    system("open https://www.ascap.com/repertory#ace/works/publisher/313766/VTOWN%20CARTEL%20MUSIC");
}

void sesac_download()
{
    system("open https://www.sesac.com/#/repertory/search");
}

void bmi_download()
{
    system("open https://www.bmi.com");
}

FILES::FILES(std::string path):Path_name{path}
{
    if(path == "./bmi.csv")
        p=bmi;
    if(path == "./ascap.csv")
        p=ascap;
    if(path == "./sesac.csv")
        p=sesac;
    if(path == "./bpb.csv")
        p=bpb;

    f.open(Path_name);
    if (!f.is_open())
    {
        std::cout << "Unable to open file";
    }
    else
    {
//        std::cout << "\n" << path << " file open" << " with enum set to ";
//        if(p==bmi)
//            std::cout<<"bmi\n";
//        if(p==ascap)
//            std::cout<<"ascap\n";
//        if(p==sesac)
//            std::cout<<"sesac\n";
//        if(p==bpb)
//            std::cout<<"bpb\n";
    }
}

string FILES::PRO_type()
{
    switch(p)
    {
        case ascap: return"ascap";
        case bmi: return"bmi";
        case bpb: return"bpb";
        case sesac: return"sesac";
    }
}

inline bool is_Integer(const std::string & s)
{
    if(s.empty()) return true;
    if(!isdigit(s[0])) return false;

    if(s.find('.') != std::string::npos)
        return is_Float(s);
    char * p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}

inline bool is_Float(const std::string & s)
{
        std::istringstream iss(s);
        float f;
        iss >> std::noskipws >> f;   // noskipws considers leading whitespace invalid
                                     // Check the entire string was consumed and if either failbit or badbit is set
        return iss.eof() && !iss.fail();

}

void error_message(std::string const & field_name, std::string const & message, int index)
{
    std::cout << "\nERROR! - In row " << index << " inside " << field_name << " field";
    std::cout << "\nERROR TYPE - " << message;
    exit(-1);
}

void error_message(std::string const & field_name, std::string const & message, int index,int pub_num)
{
    std::cout << "\nERROR! - In row " << index << " inside " << field_name << " " << pub_num << " field";
    std::cout << "\nERROR TYPE - " << message;
    exit(-1);
}

void error_message(std::string const & message)
{
    std::cout << "\nERROR!\nERROR TYPE - " << message;
    exit(-1);
}
bool IsLetters(const string & input)
{

    for(auto const &c : input)
//        for(size_t i = 0; i < input.size(); i++)
    {  if (! isalpha(c))
            return false;  // character is not alphabetic
    }
    return true;           // Every character is alphabetic
}

size_t stringCount(const std::string& referenceString,
                   const std::string& subString) {

    const size_t step = subString.size();

    size_t count(0);
    size_t pos(0) ;

    while( (pos=referenceString.find(subString, pos)) !=std::string::npos) {
        pos +=step;
        ++count ;
    }

    return count;

}