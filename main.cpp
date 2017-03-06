
/* TODO
- Tweak C + weight
    - Etablir un nouveau set clean, un nouveau set bad
    - Run les predicts sur les deux et observer le taux de réussite
    - Variation de C : 1 10 100 1000 ... puis plus précis
- Try some more string peparations
- Tests different svm types
- filtrage de bayes ? 
- Reduce model
- tweak edit distance

-> Beaucoup de char spéciaux? -> SVM (+ SVM moins précis avant?)
                              -> Bayes
*/
#include <string>
#include <iostream>
#include "svm.h"
#include "SVMModel.h"

int main(int argc, char **argv)
{
    SVMModel test("E:/work/SPAM/data/");
    //test.prepare("regular_training.txt", "spam_training.txt", 0.09f);
    //test.save_model("model.svm");
    test.load_model("model.svm");

    //test.predict_train_data();
    //test.predict_file ("test.txt", SVMModel::PRINT_ALL);
    test.predict_file ("random3.txt", SVMModel::PRINT_BAD);
    //test.predict_file("ely_spamdetect_20170303.txt", SVMModel::PRINT_GOOD);
    //test.predict_file("goodOnes.txt", SVMModel::PRINT_BAD);
    //test.predict_file("regular_training.txt", SVMModel::PRINT_BAD);
    //test.predict_file("spam_training.txt", SVMModel::PRINT_BAD);
    //std::vector<double> testValues = { 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f };
    //test.test_C("regular_training.txt", "spam_training.txt", "goodOnes.txt", "ely_spamdetect_20170303.txt", testValues);
    //test.do_cross_validation("regular_training.txt", "spam_training.txt", 2);
        
    std::string inputLine;
    while (true)
    {
        std::getline(std::cin, inputLine);
        test.predict(inputLine, SVMModel::PRINT_ALL);
    }
}