#include  "hashset.h"

using namespace std;

template<class key_type, class hash_func, class key_equal>
const int HashSet<key_type, hash_func, key_equal>::num_primes = 25;


template<class key_type, class hash_func, class key_equal>
const unsigned long HashSet<key_type, hash_func, key_equal>::prime_list[] =
        {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
         196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
         50331653, 100663319, 201326611, 402653189, 805306457};


template<class key_type, class hash_func, class key_equal>
bool HashSet<key_type, hash_func, key_equal>::search(const key_type &k)
{

    int p = hf(k) % table_size(); // так-так, начинаем описание. Берем п как остаток от деления, потому что может

    while ((*ht)[p].used) //пока элемент помечен как использованный (образует фул последоватеьлность без разрывов)
    {
        if (eq((*ht)[p].key, k) &&
            !(*ht)[p].flagfordeleting) //если этот элемент равен по ключу и при этом не помечет как удаленный
            return true;  // он есть
        p++;  //гуляем до конца
        if (p == table_size())
            p = 0;  // wrap around to beginnin
    }
    return false; // последовательность кончилась - вернем фолс
}

template<class key_type, class hash_func, class key_equal>
void HashSet<key_type, hash_func, key_equal>::remove(const key_type &k)
{

    int p = hf(k) % table_size(); // см комменты выше, потому что можем

    while ((*ht)[p].used) // цикл аналогичен серчу, с той лишь разницей, что когда мы находим нужный элемент, мы помечаем его флагом для удаления и останавливаемся
    {
        if (eq((*ht)[p].key, k) && !(*ht)[p].flagfordeleting)
        {
            (*ht)[p].flagfordeleting = true;
            return;
        }
        p++;
        if (p == table_size())
            p = 0;
    }

    throw std::invalid_argument("bad remove"); //кидаем эксепшн если ниче не нашли


}


template<class key_type, class hash_func, class key_equal>
void HashSet<key_type, hash_func, key_equal>::insert(const key_type &k)
{
    if (load_factor() > .7) // эта штука для того чтобы вызвать ресайз
    {
        resize();
    }

    int p = hf(k) % table_size(); // ну можем же!

    while ((*ht)[p].used) //поиск элемента незанятого
        p++;

    if (p == table_size()) // и снова поиск(почти)
        p = 0;

    while (((*ht)[p].used)) // и еще раз поиск
        p++;

    (*ht)[p].key = k;  //присовим кею значение и пометим как использумый элемент
    (*ht)[p].used = true;
    entries++;//не забудем увеличить кол-во элментов
}

template<class key_type, class hash_func, class key_equal>
int HashSet<key_type, hash_func, key_equal>::resize()
{

    if (prime == num_primes - 1) //эксепшн
    {
        cerr << "maximal table size reached, aborting ... " << endl;
        exit(2);
    }

    int _previossize = prime_list[prime]; //старый размер у нас бл вот такой
    prime++; //теперь увеличим его на еденицу из констант
    int _newsize = prime_list[prime]; // у нас есть новый размер!
    vector<Entry> *ptr = new vector<Entry>(_newsize);  // и новый вектор!
    int i = 0;  //  и новый счетчик
    while (i < _previossize)  //  собсна считем, пока не перевалим через кол-во элементов в ласт векторе
    {
        if ((*ht)[i].used && (*ht)[i].flagfordeleting == false)  // если элемент был использован и не помечен как удаляемый
        {
            key_type buffer = (*ht)[i].key;  //  запишем ключ в буффер
            int p = hf(buffer) % table_size();  // опять потому что можем

            while (p < _newsize && (*ptr)[p].used)  //  и еще раз можем
                p++;

            if (p == _newsize)  //  и сейчас
                p = 0;

            while ((*ptr)[p].used)  // да!
                p++;
            (*ptr)[p].key = buffer;  // запишем в ключ буффер
            (*ptr)[p].used = true;  // и пометим элемент как используемый

        }
        i++;  // увеличим цикл
    }
    delete ht;  // удалит старое
    ht = ptr;  // присвоем старому новое
    return _newsize;  // ввернем новый размер
}