void fizz();
void buzz();
void fizzbuzz();
void print_number(int n);

void test(int n)
{
    for (int i = 0; i < n; i++)
    {
        bool div_3 = i % 3 == 0;
        bool div_5 = i % 5 == 0;
        if (div_3 && div_5)
            fizzbuzz();
        else if (div_3)
            fizz();
        else if (div_5)
            buzz();
        else
            print_number(i);
    }
}

int somefunc(int x, int y) {
    int z = x + y;
    if (z % 2 == 0) {
        z = 0;
    } else {
        z = 1;
    }
    return z;
}
