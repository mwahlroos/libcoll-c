#include <stdlib.h>

#include "ccoll_linkedlist_test.h"
#include "hashtable_test.h"

int main()
{
    int ccoll_ll_success = ccoll_ll_test_main();
    int ht_success = ht_test_main();
    exit(ccoll_ll_success & ht_success);
}
