
/* TODO
- Try some more string peparations
- Tests different svm types
- filtrage de bayes ? 
- Reduce model
- tweak edit distance
- tout move dans data dir + en faire un argument
- re organiser projet, copier juste svm en fait drop le reste

-> Beaucoup de char spéciaux? -> SVM (+ SVM moins précis avant?)
                              -> Bayes
*/
#include <string>
#include <iostream>
#include "SVM_Spam.h"

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    SVM_Spam test(argv[1]);
    //test.prepare("regular_training.txt", "spam_training.txt", 0.08f);
    //test.save_model("model.svm");
    test.load_model("model.svm");

    //test.predict_train_data();
    //test.predict_file ("test.txt", SVM_Spam::PRINT_ALL);
    test.predict_file("random3.txt", SVM_Spam::PRINT_BAD);
    //test.predict_file("ely_spamdetect_20170303.txt", SVM_Spam::PRINT_GOOD);
    //test.predict_file("goodOnes.txt", SVM_Spam::PRINT_BAD);
    //test.predict_file("regular_training.txt", SVM_Spam::PRINT_BAD);
    //test.predict_file("spam_training.txt", SVM_Spam::PRINT_BAD);
    //test.do_cross_validation("regular_training.txt", "spam_training.txt", 2);
        
    std::string inputLine;
    while (true)
    {
        std::getline(std::cin, inputLine);
        if (inputLine != "")
            test.predict(inputLine, SVM_Spam::PRINT_ALL);
    }
}