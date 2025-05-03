#include <astd/astd.h>
#include <iostream>
#include <thread>
#include <optional>
#include <variant>

Amazing::Ring<int32_t> ring;

void produce()
{
    int32_t v[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    while (true)
    {
        ring.write(v, 10);
        std::cout << ring.size() << '\n';
    }
}

void consume()
{
    Amazing::Vector<int32_t> v(10);
    while (true)
    {
        ring.read(v.data(), 10);
        std::cout << ring.size() << '\n';
    }
}

int main()
{
    // std::thread producer(&produce);
    // std::thread consumer(&consume);
    //
    // producer.join();
    // consumer.join();

    Amazing::String ss = Amazing::to_str(23);

    Amazing::HashMap<int, int> map;
    map.emplace(2, 4);
    map.emplace(3, 5);
    map.emplace(4, 6);

    int m1 = map[2];
    int m2 = map[3];
    int m3 = map[4];

    Amazing::String s("chr");
    Amazing::String f("fl");
    s += f;

    size_t pos = s.find("rfk");

    Amazing::Vector<int64_t> v;
    v.push_back(10);
    v.push_back(9);
    v.push_back(6);
    v.push_back(2);
    v.push_back(7);
    v.push_back(1);
    v.push_back(4);
    v.push_back(7);
    v.push_back(6);

    Amazing::Vector<double> d;
    d.push_back(8);
    d.push_back(10);
    d.push_back(9);
    d.push_back(6);
    d.push_back(2);
    d.push_back(7);
    d.push_back(1);
    d.push_back(4);
    d.push_back(7);
    d.push_back(6);


    Amazing::List<double> l;
    l.push_back(1);
    l.push_back(4);
    l.push_back(9);
    l.push_back(8);


    Amazing::Map<int64_t, uint32_t> m;
    m.emplace(1, 2);
    m.emplace(Amazing::Pair<int64_t, uint32_t>(34, 1));
    auto it = m.find(1);
    //m.insert(Amazing::Pair<int64_t, uint32_t>(32, 12));
    //m.insert(Amazing::Pair<int64_t, uint32_t>(2, 12));
    //m.insert(Amazing::Pair<int64_t, uint32_t>(2, 11));

    m.erase(1);

    m[2] = 14;

    m.clear();

    struct Tr
    {
        int v;
        double d;
    };

    Amazing::UniquePtr<Tr> p(1, 2);

    for (size_t i = 0; i < v.size(); i++)
        std::cout << v[i] << '\n';

    for (size_t i = 0; i < d.size(); i++)
        std::cout << d[i] << '\n';

    return 0;
}