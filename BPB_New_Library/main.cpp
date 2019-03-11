/**************************************************************************************************
    BPB New Library 1.0
    Author: Steven Harriton, 2019
    email: stevenharriton@gmail.com

    Changes:
    2018-10-10: fixed errors from input miscalculation
    2018-10-23: increased input speed from BPB file
    2018-12-10: added title printing for all data objects
    2019-1-5:   refactored data structures increases program efficiency
                added new criteria to search algorithm
    2019-1-17:  adjustments to BPB titles to account for BMI naming conventions
    2019-2-15:  search algorithm optimizations and corrections
    2019-2-28:  adding a second layer to the matching, using the first name of the first composer

 **************************************************************************************************/

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Bullet Proof Bear is a music library company that administers rights on a collection of music. When a new library
 // is aquired, this program will help to determine if the aquired tracks need to have their registrations updated
 // or if they need to be registered for the first time.
 //
 // This program checks if a music composition is registered with the appropriate performing rights organization.
 // The bpb.csv holds the data for all the compositions that need to be checked. The ascap.csv, bmi.csv and sesac.csv
 // files hold the data for the 3 PRO's being searched. A composition can have multiple publishers who each have a
 // PRO they are affiliated with. The program first imports track title, all writers, all publishers, PRO's assigned
 // to publishers, and the publishing share of all the 45,000+ tracks in the bpb.csv. It then gives the user an
 // option to print all the titles to make sure they were imported correctly. After that the user is given the option
 // of having the program open the browser to each PRO's web site so the user can download the appropriate csv files.
 //
 // The program then imports the data from each PRO and searches it for titles matching the titles of BPB tracks. Then
 // it prints out the track titles and the registrations they are missing into results.txt.
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <thread>
#include "declarations.h"

int main()
{
    BPB::FILES bpb_csv("./bpb.csv");                                        // import bpb file
    int bpb_row_numb = number_of_rows(bpb_csv.f);
    BPB::bpb bpb_data(bpb_csv.PRO_type(),bpb_row_numb);
    bpb_data.data_parse(bpb_csv);
    bpb_data.title_adjustments();                                           // creates a second version of titles
                                                                            // that uses Bmi.csv's naming conventions
    bpb_data.ask_user_if_print();

    bool decision = navigate_to_sites();

    if(decision)                                                            // if user wants to download the csv's
    {
        ask_for_user_to_download(1, "ascap");                               // prompt user download ascap csv
        ask_for_user_to_download(2, "sesac");                               // prompt user download sesac csv
        ask_for_user_to_download(3, "bmi");                                 // prompt user download bmi csv
    }

    std::cout << "\n<Press Enter to begin searching the PRO csv for matches to BPB titles>\n";
    std::cin.ignore();

    std::thread worker1;
    std::thread worker2;
    std::thread worker3;

    worker1 = std::thread(ascap_import_data_and_search_titles, std::ref(bpb_data), "./ascap.csv");   // ascap search
    worker2 = std::thread(sesac_import_data_and_search_titles, std::ref(bpb_data), "./sesac.csv");   // sesac search
    worker3 = std::thread(bmi_import_data_and_search_titles, std::ref(bpb_data));                    // bmi search

    worker1.join();
    worker2.join();
    worker3.join();

    bpb_data.are_these_registered();

    return 0;
}