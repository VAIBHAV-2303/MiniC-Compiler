int main() {

    int a = 2000000000;
    int b = a;
    int c = a + b;
    print(c, '\n');

    uint d = (uint)2000000000;
    uint e = d;
    uint f = d + e;
    print(f, '\n');

    f = d + (uint)2300000000;
    print(f, '\n');

    uint u = (uint)5;
    uint v = (uint)10;
    print(u-v, '\n');
    print(u+10, '\n');

    return 0;
}