#define _STRING

/* TODO
- Tweak C + weight
    - Etablir un nouveau set clean, un nouveau set bad
    - Run les predicts sur les deux et observer le taux de réussite
    - Variation de C : 1 10 100 1000 ... puis plus précis
- Try some more string peparations
- Tests different svm types
- filtrage de bayes ? 
- Reduce model


-> Beaucoup de char spéciaux? -> SVM (+ SVM moins précis avant?)
                              -> Bayes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#include "Timer.h"
#include "SVMModel.h"

int main(int argc, char **argv)
{
    //Problem definition-------------------------------------------------------------
    SVMModel test;
    test.prepare("regular_training.txt", "spam_training.txt");
    test.save_model("model.svm");
    //test.load_model("model.svm");

    //test.predict_train_data();
    //test.predict_file("test.txt", true);
    //test.predict_file("zet_spamdetect_20170302.csv", true);
    test.predict_file("random.txt", true, true);

    //test.predict_file("regular_training.txt", true, true);
    //test.predict_file("spam_training.txt", true, true);
   

    std::string lol;
    std::cin >> lol;
}