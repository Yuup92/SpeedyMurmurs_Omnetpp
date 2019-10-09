#include "./RandHelper.h"

RandHelper::RandHelper() {

}

void RandHelper::rand_set_and_complement_list(int *set, int *complement, int range, int setLength) {

    int numList[range];
    int lengthList = range;
    // Populate a list from range [0-lengthList)
    for(int i = 0; i < lengthList; i++) {
        numList[i] = i;
    }

    // Populate a set array with random values
    for(int i = 0; i < setLength; i++) {
        set[i] = RandHelper::random_list_selection(numList, lengthList);
        lengthList--;
    }

    // Add complement  values to complement array
    for(int i = 0; i < lengthList; i++) {
        complement[i] = numList[i];
    }
}

int RandHelper::random_list_selection(int *list, int length) {

    int randIndex = rand() % length;
    int res = *(list + randIndex);

    for(int i = randIndex; i < (length - 1); i++) {
        list[i] = list[i+1];
    }

    return res;
}
