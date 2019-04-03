//
// Created by Steven Harriton on 12/27/18.
//

namespace BPB {
    using std::string;
    using std::vector;

    class FILES;

    class DATA
    {
    public:
        DATA(string pro):PRO_name{pro}{}
        virtual ~DATA(){};
        virtual void data_parse(FILES &a) = 0;
    private:
        string PRO_name{};
    };

    class track
    {
    public:
        int index;
        string title;
        vector <string> writer;
        vector <string> writer_pro;
        vector <string> publisher;
        vector <string> pro;
        vector <string> share;
        bool found_in_ascap;
        bool found_in_sesac;
        bool found_in_bmi;
        bool should_have_reg_with_ascap{};
        bool should_have_reg_with_sesac{};
        bool should_have_reg_with_bmi{};

        explicit track(int i, string t, vector<string> w, vector<string> w_p,
              vector<string> p, vector<string> pr, vector<string> s,
              bool found_a = false, bool found_s = false, bool found_b = false,
              bool should_have_reg_a = false, bool should_have_reg_s = false,
              bool should_have_reg_b = false);

        bool operator==(const std::vector<track>:: iterator right);
        bool operator==(const string & a);
    };

    class bpb : public DATA
    {
    public:
        bpb (string a, int rows) : DATA{a},row_cnt{rows} {}
        ~bpb() {};
        void data_parse(FILES &a);
        string get_one_field(FILES &a);
        void are_these_registered();
        void ask_user_if_print();
        void title_adjustments(void);

        vector <string> titles_adjusted_for_bmi{};     //bmi has very specific naming conventions. A and THE are removed if first word of a title
        vector <track> track_data{};

    private:
        int row_cnt;
    };
    class ascap : public DATA
    {
    public:
        ascap (string pro,int rows) : DATA{pro},row_cnt{rows} {}
        ~ascap() {};
        void data_parse(FILES &a);
        vector <string> push_pro_info_1st_time(FILES &a);
        vector <string> push_pro_info(FILES &a,const string & old);

        int row_cnt;
        vector <track> track_data{};
    };

    class bmi : public DATA
    {
    public:
        bmi (string pro, int size) : DATA{pro}, row_cnt{size}{}
        ~bmi(){}
        void data_parse(FILES &a);

        int row_cnt;
        vector <track> track_data{};
    };

    class sesac : public DATA
    {
    public:
        sesac (string a) : DATA(a) {}
        ~sesac(){};
        void data_parse(FILES &a);

        vector <track> track_data{};
    };

    class FILES
    {
    public:
        string Path_name;
        enum pub {ascap,bmi,bpb,sesac};
        pub p;
        std::fstream f;

        explicit FILES(string path);
        ~FILES(){ f.close(); }
        string PRO_type();
    };
}

class thread_handle
{
    std::thread t;
public:
    ~thread_handle();

    template<typename Callable,typename ... Args>
    explicit thread_handle(Callable&& func,Args&& ... args):
    t(std::forward<Callable>(func),std::forward<Args>(args)...)
    {}

    bool joinable() const noexcept;
    void join(void);
    thread_handle(thread_handle const & ) = delete;
    thread_handle& operator=(thread_handle const & ) = delete;
};

template<typename T>
void print_titles(T &data)
{
    int offset{};
    if(typeid(data).name() == typeid(BPB::bpb).name())
        offset = 2;

    for (int i = 0 ; i < std::size(data.track_data) ; ++i)
    {
        if((i == 0 || i == 1) && offset == 2)    // if this is track_data from bpb object skip 2 indexes so
            continue;                            // i lines up with the row number in the csv.
        std::cout << "\n" << i + offset << " " << (data.track_data.begin()+i)->title;
    }
}

int number_of_rows(BPB::FILES &file);
void ask_for_user_to_download(int dest,std::string website);
bool navigate_to_sites();
void ascap_download();
void sesac_download();
void bmi_download();
void ascap_import_data_and_search_titles(BPB::bpb & bpb,std::string && path);
void sesac_import_data_and_search_titles(BPB::bpb & bpb,std::string && path);
void bmi_import_data_and_search_titles(BPB::bpb bpb);
int number_of_rows(std::fstream &keywords);
void remove_A(std::string & title);
void remove_THE(std::string & title);
void remove_COMMA(std::string & title);
void remove_APOSTR(std::string & title);
void remove_DASH(std::string & title);
void remove_PARENTH(std::string & title);
inline bool is_Integer(const std::string & s);
void error_message(std::string const & field_name, std::string const & message, int index);
void error_message(std::string const & field_name, std::string const & message, int index,int pub_num);
void error_message(std::string const & message);
void error_message(std::string const & field_name,std::string const & message,std::string const & title, int index);
inline bool is_Float(const std::string & s);
bool IsLetters(const std::string & input);
size_t stringCount(const std::string& referenceString, const std::string& subString);