#include "SVM_Spam.h"
#include "Timer.h"

#include "svm.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <regex>
#include <functional>
#include "CharEquivalences.h"
#include <codecvt>

using namespace std;

const int LIMIT_INPUT_LOL = 3000; //0 to disable
const std::string equivalence_filename = "characters_equivalences.txt";

SVM_Spam::SVM_Spam(std::string data_dir)
    : model(nullptr), data_dir(data_dir)
{
    bool result = equi.load_character_equivalences(data_dir + '/' + equivalence_filename);
    if (!result)
        std::cerr << "Failed to load equivalences file" << std::endl;

    param.svm_type = C_SVC;
    param.kernel_type = EDIT;
    param.data_type = STRING;
    param.degree = 3; //OSEF
    param.gamma = 0.5; //OSEF
    param.coef0 = 0; //OSEF
    param.nu = 0.8;
    param.cache_size = 1024;
    param.C = 0.09;
    param.eps = 1e-5;
    param.p = 0.1;
    param.shrinking = 0;
    param.probability = 0;

    //weight
    param.nr_weight = 2;
    param.weight_label = new double[param.nr_weight];
    param.weight_label[0] = 1.0f; //non spam
    param.weight_label[1] = -1.0f; //spam
    param.weight = new double[param.nr_weight];
    param.weight[0] = 1.0f;
    param.weight[1] = 1.1f;
    /*
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;
    */
}

SVM_Spam::~SVM_Spam()
{
    svm_destroy_param(&param);
    delete model;
}

void SVM_Spam::prepare(std::string regular_file, std::string spam_file, double overrideC /*= 0.0f*/)
{
    double oldC = param.C;
    if (overrideC != 0.0f)
        param.C = overrideC;

    std::cout << "Preparing model from files " << regular_file << " (regular) and " << spam_file << " (spam)..." << std::endl;
    Timer timer("Total training time: ");
    regular_strings.reserve(200000);
    spam_strings.reserve(200000);
    read_file(regular_file, regular_strings);
    read_file(spam_file, spam_strings);

    if (LIMIT_INPUT_LOL)
    {
        //LIMIT_INPUT_LOL???
        std::random_shuffle(regular_strings.begin(), regular_strings.end());
        if (regular_strings.size() > LIMIT_INPUT_LOL)
            regular_strings.resize(LIMIT_INPUT_LOL);

        std::random_shuffle(spam_strings.begin(), spam_strings.end());
        if(spam_strings.size() > LIMIT_INPUT_LOL)
            spam_strings.resize(LIMIT_INPUT_LOL);

        // LIMIT_INPUT_LOL \!\!\!
    }

    struct svm_problem prob;
    prob.l = unsigned int(regular_strings.size() + spam_strings.size());

    svm_data* x = new svm_data[prob.l];
    prob.y = new double[prob.l];

    unsigned int idx = 0;
    for (auto const& itr : regular_strings)
    {
        x[idx].s = const_cast<char*>(itr.c_str());
        prob.y[idx] = 1.0f;
        idx++;
    }
    for (auto const& itr : spam_strings)
    {
        x[idx].s = const_cast<char*>(itr.c_str());
        prob.y[idx] = -1.0f;
        idx++;
    }

    prob.x = x;

    // Validate parameters
    const char* err = svm_check_parameter(&prob, &param);
    if (err != NULL)
    {
       std::cout << "svm_check_parameter failed with error " << err << std::endl;
       return;
    }

    // Train model
    model = svm_train(&prob, &param);

    // Cleaning up
    delete x;
    delete prob.y;
    if (param.C != oldC) //restore C if it was overriden
        param.C = oldC;

    std::cout << "Finished preparing model" << std::endl;
    timer.stop(true);
}

// Just run prediction on the data the model was fed on
void SVM_Spam::predict_train_data()
{
    if (!model || regular_strings.empty() || spam_strings.empty())
    {
        std::cerr << "SVM_Spam has not been trained yet" << std::endl;
        return;
    }

    std::cout << "Re testing data we trained the model on..." << std::endl;
    Timer test_timer("Total time: ");
    svm_data testnode;
    double retval;

    int regularSuccessTests = 0;
    int regularFailedTests = 0;
    int spamSuccessTests = 0;
    int spamFailedTests = 0;

    for (auto const& itr : regular_strings)
    {
        testnode.s = const_cast<char*>(itr.c_str());
        retval = svm_predict(model, testnode);

        retval == 1.0 ? regularSuccessTests++ : regularFailedTests++;
    }

    for (auto const& itr : spam_strings)
    {
        testnode.s = const_cast<char*>(itr.c_str());
        retval = svm_predict(model, testnode);
        retval == -1.0 ? spamSuccessTests++ : spamFailedTests++;
    }

    std::cout << "Result:" << std::endl;
    std::cout << std::endl;
    std::cout << regularSuccessTests << "\t" << spamFailedTests << std::endl;
    std::cout << regularFailedTests << "\t" << spamSuccessTests << std::endl;
    std::cout << std::endl;
    test_timer.stop(true);
}

double SVM_Spam::predict(std::string const& line, PrintOptions printOptions /*= PRINT_NONE */, std::ofstream* spam_dump /* = nullptr */ )
{
    double retval;
    svm_data testnode;
    std::string line_prepared(line);
    bool str_eligible = prepare_string(line_prepared);
    if (str_eligible)
    {
        testnode.s = const_cast<char*>(line_prepared.c_str());
        retval = svm_predict(model, testnode);
    }
    else {
        retval = 1.0f;  //string was ineligible, set as good
    }

    //print to console
    if (    (retval >= 0.0f && printOptions & PRINT_GOOD )
         || (retval <= 0.0f  && printOptions & PRINT_BAD  ) )
    {
        std::cout << retval << "\t" << line << std::endl << std::endl;
    }

    //send spam to spam dump
    if (spam_dump && !spam_dump->fail() && retval == -1.0f)
        *spam_dump << line << std::endl;

    return retval;
}

void SVM_Spam::predict_file(std::string file, PrintOptions printOptions /*= PRINT_NONE*/)
{
    if (!model)
    {
        std::cerr << "SVM_Spam has no model loaded" << std::endl;
        return;
    }

    std::ofstream spam_dump("spamdump.txt", std::ios_base::app | std::ios_base::out);
    if (spam_dump.fail())
        abort();
    spam_dump << "=========================================================================" << std::endl;

    std::cout << "Predicting " << file << " file..." << std::endl;
    Timer test_timer("");
    double retval;
    unsigned int spam = 0;
    unsigned int non_spam = 0;

    std::vector<std::string> tests;
    read_file(file, tests, false);
    size_t line_count = tests.size();
    int idx = 0;
    int percentage = 5;
    for (auto const& itr : tests)
    {
        retval = predict(itr, printOptions, &spam_dump);
        retval == -1 ? spam++ : non_spam++;
        idx++;
        if ((100 * idx / line_count) > percentage)
        {
            std::cout << percentage << "%" << std::endl;
            percentage += 5;
        }
    }

    std::cout << "Spam: " << spam << " | Non-Spam: " << non_spam << " | Exec time: " << test_timer.stop(false) << "s" << std::endl;
}

// A string is not eligible if it's not long enough when we removed most non significative data
bool SVM_Spam::eligible_for_prepare(std::string const& str)
{
    std::string _str(str);

    // remove this bunch of special characters
    std::regex remove_chars(R"|([ !\.;\?:-_=$\(\)'"{}\[\]/\\<>\*&\+\^"])|");
    _str = std::regex_replace(_str, remove_chars, "");

    //TODO + Remove all CHINESE character, so that if we only have a few other characters left, we skip
    //TODO + Remove all RUSSIAN character, so that if we only have a few other characters left, we skip

    // remove chinese characters
    // _str = std::regex_replace(_str, std::regex("([\u4e00}-\u9fa5}]+)"), "");
    
    // remove digits
    std::remove_if(_str.begin(), _str.end(), &::iswdigit);

    if (_str.size() <= 6)
        return false;

    return true;
}

/* Replace similar looking characters
*/
void SVM_Spam::replace_equivalent_characters(std::string& str)
{
    // + TODO, si pas character alpha, on peut skip
    // + TODO, gros tableau de correspondance pour speed up

    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    std::wstring str_16(converter.from_bytes(str));

    for (int i = 0; i < equi.regex_check.size(); i++)
    {
        std::wregex& regex = equi.regex_check[i];
        wchar_t firstChar = equi.groups[i][0];
        str_16 = std::regex_replace(str_16, regex, &firstChar);
    }

    str = converter.to_bytes(str_16);
}

bool SVM_Spam::prepare_string(std::string& str)
{
    if (!SVM_Spam::eligible_for_prepare(str))
        return false;

    // ! Order is important !

    //Remove wow vanilla links
    str = std::regex_replace(str, std::regex("\\|c........\\|H[^\\|]+\\|h([^\\|]+)\\|h\\|r"), "$1");

    //to lowercase
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    //
    replace_equivalent_characters(str);

    //remove spaces
    str.erase(std::remove_if(str.begin(), str.end(), [](char ch) { return std::isspace<char>(ch, std::locale::classic()); }), str.end());

    //replace all digits with 1's
    std::replace_if(str.begin(), str.end(), &::iswdigit, '1');

    return true;
}

void SVM_Spam::read_file(std::string filename, std::vector<std::string>& output, bool prepare /*= true*/)
{
    std::ifstream training_file(data_dir + '/' + filename);

    std::string line;
    while (std::getline(training_file, line))
    {
        if (prepare)
        {
            if (prepare_string(line))
                output.push_back(line);
        } else 
            output.push_back(line);
    }

    if (output.size() == 0)
        *((unsigned int*)0) = 0xDEAD;
}

bool SVM_Spam::save_model(std::string file)
{
    if (!model)
    {
        std::cerr << "SVM_Spam has no model loaded" << std::endl;
        return false;
    }

    int ret = svm_save_model((data_dir + '/' + file).c_str(), model);
    return ret == 0;
}

bool SVM_Spam::load_model(std::string file)
{
    delete model;
    model = svm_load_model((data_dir + '/' + file).c_str());
    bool ok = model != nullptr;
    if (!ok)
        std::cerr << "Failed to load model " << file << std::endl;

    return ok;
}

void SVM_Spam::test_C(std::string training_regular_file, std::string training_spam_file, std::string testing_regular_file, std::string testing_spam_file, std::vector<double> const& testValues)
{
    std::vector<std::string> testsRegulars;
    std::vector<std::string> testSpams;
    read_file(testing_regular_file, testsRegulars, true);
    read_file(testing_spam_file, testSpams, true);

    for (auto testvalue : testValues)
    {
        std::cout << "Testing C with value testvalue (testing file regular: " << testing_regular_file << ", testing file spam: " << testing_spam_file << ")" << std::endl;
        int regular_success = 0;
        int regular_failure = 0;
        int spam_success = 0;
        int spam_failure = 0;

        prepare(training_regular_file, training_spam_file, testvalue);
        for (auto itr : testsRegulars)
        {
            double res = predict(itr, SVM_Spam::PRINT_NONE);
            if (res < 0.0f)
                regular_failure++;
            else
                regular_success++;
        }

        for (auto itr : testSpams)
        {
            double res = predict(itr, SVM_Spam::PRINT_NONE);
            if (res < 0.0f)
                spam_success++;
            else
                spam_failure++;
        }

        std::cout << "Result for C = " << testvalue << ":" << std::endl;
        std::cout << regular_success << "\t" << spam_success << std::endl;
        std::cout << regular_failure << "\t" << spam_failure << std::endl;
        std::cout << "Regular: " << regular_success * 100.0f / testsRegulars.size() << "%" << std::endl;
        std::cout << "Spam: " << spam_success * 100.0f / testSpams.size() << "%" << std::endl;
        std::cout << std::endl;
    }
}

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

void SVM_Spam::do_cross_validation(std::string training_regular_file, std::string training_spam_file, int nr_fold)
{
    read_file("regular_training.txt", regular_strings);
    read_file("spam_training.txt", spam_strings);

    svm_parameter param;
    param.svm_type = C_SVC;
    param.kernel_type = EDIT;
    param.data_type = STRING;
    param.degree = 3; //OSEF
    param.gamma = 0.5; //OSEF
    param.coef0 = 0; //OSEF
    param.nu = 0.5;
    param.cache_size = 1024;
    param.C = 0.1;
    param.eps = 1e-5;
    param.p = 0.1;
    param.shrinking = 0;
    param.probability = 0;

    //weight
    param.nr_weight = 2;
    param.weight_label = new double[param.nr_weight];
    param.weight_label[0] = 1.0f; //non spam
    param.weight_label[1] = -1.0f; //spam
    param.weight = new double[param.nr_weight];
    param.weight[0] = 1.0f;
    param.weight[1] = 1.0f;

    struct svm_problem prob;
    prob.l = unsigned int(regular_strings.size() + spam_strings.size());

    svm_data* x = new svm_data[prob.l];
    prob.y = new double[prob.l];

    unsigned int idx = 0;
    for (auto const& itr : regular_strings)
    {
        x[idx].s = const_cast<char*>(itr.c_str());
        prob.y[idx] = 1.0f;
        idx++;
    }
    for (auto const& itr : spam_strings)
    {
        x[idx].s = const_cast<char*>(itr.c_str());
        prob.y[idx] = -1.0f;
        idx++;
    }

    prob.x = x;

    // --- Original func from this point ---

    unsigned int i;
    int total_correct = 0;
    double total_error = 0;
    double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
    double *target = Malloc(double, prob.l);

    svm_cross_validation(&prob, &param, nr_fold, target);
    if (param.svm_type == EPSILON_SVR ||
        param.svm_type == NU_SVR)
    {
        for (i = 0; i<prob.l; i++)
        {
            double y = prob.y[i];
            double v = target[i];
            total_error += (v - y)*(v - y);
            sumv += v;
            sumy += y;
            sumvv += v*v;
            sumyy += y*y;
            sumvy += v*y;
        }
        printf("Cross Validation Mean squared error = %g\n", total_error / prob.l);
        printf("Cross Validation Squared correlation coefficient = %g\n",
            ((prob.l*sumvy - sumv*sumy)*(prob.l*sumvy - sumv*sumy)) /
            ((prob.l*sumvv - sumv*sumv)*(prob.l*sumyy - sumy*sumy))
        );
    }
    else
    {
        for (i = 0; i<prob.l; i++)
            if (target[i] == prob.y[i])
                ++total_correct;
        printf("Cross Validation Accuracy = %g%%\n", 100.0*total_correct / prob.l);
    }
    free(target);
}
