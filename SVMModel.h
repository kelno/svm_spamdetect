#ifndef H_SVMMODEL
#define H_SVMMODEL

#include <vector>
#include <string>
#include "svm.h"

class SVMModel
{

public:
    SVMModel();
    ~SVMModel();

    void prepare(std::string regular_file, std::string spam_file);

    // Just run prediction on the data the model was fed on
    void predict_train_data();

	// Run testing file
    void predict_file(std::string file, bool print_all = true, bool print_spam_only = false);
    double predict(std::string const& line, std::ofstream& spam_dump, bool print = true, bool print_spam_only = false);

    bool save_model(std::string file);
    bool load_model(std::string file);

private:
    //return false if string can be ignored
    bool prepare_string(std::string& str);

    void read_file(std::string filename, std::vector<std::string>& output, bool prepare = true);

    std::vector<std::string> regular_strings;
    std::vector<std::string> spam_strings;
    svm_model* model;
    svm_parameter param;
};

#endif