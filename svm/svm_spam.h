#ifndef H_SVM_Spam
#define H_SVM_Spam

#include <vector>
#include <string>
#include "svm.h"

class SVM_Spam
{

public:
    SVM_Spam(std::string dataDir);
    ~SVM_Spam();

    void prepare(std::string regular_file, std::string spam_file, double overrideC = 0.0f);

    // Just run prediction on the data the model was fed on
    void predict_train_data();

    enum PrintOptions
    {
        PRINT_NONE = 0,
        PRINT_GOOD = 1,
        PRINT_BAD  = 2,

        PRINT_ALL  = PRINT_GOOD | PRINT_BAD,
    };

    // Run testing file
    void predict_file(std::string file, PrintOptions options = PRINT_NONE);
    double predict(std::string const& line, PrintOptions options = PRINT_NONE, std::ofstream* spam_dump = nullptr);

    void test_C(std::string training_regular_file, std::string training_spam_file, std::string testing_regular_file, std::string testing_spam_file, std::vector<double> const& testValues);
    void do_cross_validation(std::string training_regular_file, std::string training_spam_file, int nr_fold);

    bool save_model(std::string file);
    bool load_model(std::string file);

    bool load_character_equivalences();

private:
    //return false if string can be ignored for training
    bool prepare_string(std::string& str);

    void read_file(std::string filename, std::vector<std::string>& output, bool prepare = true);

    std::vector<std::string> regular_strings;
    std::vector<std::string> spam_strings;
    svm_model* model;
    svm_parameter param;
    std::string data_dir;
};

#endif