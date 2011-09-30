#include "linkedlist_test.c"
#include "hashtable_test.c"

int main()
{
    int ll_success = ll_test_main();
    int ht_success = ht_test_main();
    exit(ll_success & ht_success);
}
