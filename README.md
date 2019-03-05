# code_examples
C++ coding examples
___________________
BPB New Library
___________________

Bullet Proof Bear is a music library company that administers rights on a collection of music. When a new library
is aquired, this program will help to determine if the aquired tracks need to have their registrations updated
or if they need to be registered for the first time.
 
This program checks if a music composition is registered with the appropriate performing rights organization.
The bpb.csv holds the data for all the compositions that need to be checked. The ascap.csv, bmi.csv and sesac.csv
files hold the data for the 3 PRO's being searched. A composition can have multiple publishers who each have a
PRO they are affiliated with. The program first imports track title, all writers, all publishers, PRO's assigned
to publishers, and the publishing share of all the 45,000+ tracks in the bpb.csv. It then gives the user an
option to print all the titles to make sure they were imported correctly. After that the user is given the option
of having the program open the browser to each PRO's web site so the user can download the appropriate csv files.
 
The program then imports the data from each PRO and searches it for titles matching the titles of BPB tracks. Then
it prints out the track titles and the registrations they are missing into results.txt.

Due to the nature of my contract, I can’t upload the main database. The code opens in a text file or IDE if you want to 
see my work, but it won't actually run without the BPB database csv. 
__________________
Customer
__________________

This was practice with OOP. The goal was to pull records from a text file with variable size customer records, fill each customers corresponding object with data, manipulate the data, sort the data, find average values of all customers etc.

the customers.txt file has the data in it. The first input response that is needed is the file path of the customers.txt file. If you run the program in terminal, all you have to do is drag the file into the terminal window, drop it, and press enter and it should work just fine.

__________________
BPB Synonymn
__________________

This app was designed for Vtown Cartel Music. It's still a work in progress. There's more functionality that the client wants. 
Many of the OOP items have yet to be fully installed. Vtown is a music licensing company here in California. They have a 
database of around 50,000 entries and 180 rows. There is a field called Mood. They wanted to create a program that would 
scan mood keywords by category from a separate text file. I’ll call that file ‘Z’ for clarity sake. After those keyword 
categories were copied, they wanted the app to iterate through the 50,000 entries in the database and compare the mood field 
against the different keyword categories from file ‘Z’. If one of the mood keywords present in the main database was also in a 
category from file ‘Z’, then all of the other keywords in that category would get added to that corresponding mood field 
in the main database. The employer was hoping to standardize that field of the database and make it more consistent when 
TV/Film clients searched through the database looking for music they needed. The challenge was to deliver what they wanted
without duplicate keywords in the main database.

Due to the nature of my contract, I can’t upload the main database. The code opens in a text file or IDE if you want to 
see my work, but it won't actually run without the BPB database csv. The test keyword text file is included. 



