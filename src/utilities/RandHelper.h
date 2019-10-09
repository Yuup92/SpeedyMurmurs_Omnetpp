#ifndef RANDHELPER_H_
#define  RANDHELPER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


class RandHelper {

    public:
        RandHelper();

        static void rand_set_and_complement_list(int*, int*, int, int);

    private:
        static int random_list_selection(int *, int);
};

#endif
