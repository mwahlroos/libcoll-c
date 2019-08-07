#include <stdlib.h>

#include "linkedlist_test.h"
#include "hashtable_test.h"

int main()
{
    int ll_success = ll_test_main();
    int ht_success = ht_test_main();
    exit(ll_success & ht_success);
}
