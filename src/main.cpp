#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <set>
#include <vector>
#include <iomanip> // setw
#include <algorithm>

#include "dictionary.h"

using namespace std;

void lower(string &s);

string stripPunct(const string &s);

void checkSpelling(ifstream &in, Dictionary &dict);


// program arguments to run, example: main.exe ../../res/wordlist.txt ../../res/test.txt
int main(int argc, char *argv[])
{
    ifstream inf("../../res/my_text.txt");
    if (!inf)
    {
        cerr << "Could not open " << "../../res/my_text.txt" << "\n";
        return EXIT_FAILURE;
    }

// Read dictionary, but let user know what we are working on.
    cout << "Loading dictionary, this may take awhile..." << endl;

    Dictionary d("../../res/wordlist.txt");
    checkSpelling(inf, d);

    inf.close();

    return EXIT_SUCCESS;
}


string makeline(int n) //генерируем бинарную строку из числа
{
    string line = "";
    while (n != 0)
    {
        if (n % 2)
            line += '1';
        else
            line += '0';
        n /= 2;
    }
    return line;
}


vector<string> comb(int a) //создаем вектор всевозможных бинарных строк заданной длины
{
    vector<string> vectorofbinarystring;
    int p = 1;
    for (int i = 0; i < a; i++)
        p *= 2;
    for (int i = 0; i < p; i++)
        vectorofbinarystring.push_back(makeline(i));
    for (int i = 0; i < vectorofbinarystring.size(); i++)
        while (vectorofbinarystring[i].size() < a)
            vectorofbinarystring[i] += '0';
    return vectorofbinarystring;
}


vector<pair<int, int>> makepairs(int a)//возвращаем верктор всевозможных пар
{
    vector<pair<int, int>> vectorsofrair;
    for (int i = 0; i < a - 1; i++)
        vectorsofrair.push_back({i, i + 1});
    return vectorsofrair;
}

//done
void output(ifstream &in, Dictionary &dict,
            vector<string> &ispravlenia) // вывод и проверка на уникальность того, что мы воводим
{
    auto it = ispravlenia.begin();//итератор
    sort(ispravlenia.begin(), ispravlenia.end()); //сначла отсортируем вектор
    ispravlenia.erase(unique(ispravlenia.begin(), ispravlenia.end()),
                      ispravlenia.end()); //потом удалим из него все неуникальные
    for (it; it != ispravlenia.end(); ++it) //выодим
        std::cout << "\t\t" + *it << std::endl;
}

//done
void insertanyletter(ifstream &in, Dictionary &dict, vector<string> &ispravlenia, string word)
{
    int i = 0; // содаем перменные для прохода по циклу и буфферное слово
    char change = 'a';
    string wordchek;
    while (i <= word.length())
    {
        while (change <= 'z')
        {
            wordchek = word;
            wordchek.insert(i, 1, change);
            if (dict.search(wordchek)) //добавляем в ответ ток те слова, которые есть в словаре
                ispravlenia.push_back(wordchek);
            change++;
        }
        i++;
        change = 'a';
    }
}

//done
void replacement(ifstream &in, Dictionary &dict, vector<string> &ispravlenia, string word)
{

    //действуем аналогично циклу  insertanyletter
    int i = 0;
    char change = 'a';
    string wordchek;
    while (i <= word.length())
    {
        while (change <= 'z')
        {
            wordchek = word;
            wordchek[i] = change;
            if (dict.search(wordchek))
                ispravlenia.push_back(wordchek);
            change++;
        }
        i++;
        change = 'a';
    }
}

//done
void removing(ifstream &in, Dictionary &dict, vector<string> &ispravlenia, string word)
{
    int i = 0; //проходимся по всей длинне слова и удаляем элемент
    string wordchek;
    while (i <= word.length()) //удаляем все буквы в слове, поочередно и по одной
    {
        wordchek = word;
        wordchek.erase(i, 1);
        if (dict.search(wordchek)) //аналогично добавляем только подходящие слова
            ispravlenia.push_back(wordchek);

        i++;
    }
}


void combination(ifstream &in, Dictionary &dict, vector<string> &ispravlenia, string word)
{
    vector<string> combinations = comb(word.size() - 1);  //создаем все комбинации
    vector<pair<int, int>> pairs = makepairs(word.size());//создаем все пары
    for (int i = 1; i < combinations.size(); i++) //проходимся по всем комбинациям
    {
        string wordcheck = word;
        bool flag = 1;
        vector<bool> letscheckit; //вектор в котором отмечаем какие буквы были задействованы
        for (int j = 0; j < word.size(); j++)
            letscheckit.push_back(0);
        for (int j = 0; j < combinations[i].size(); j++) //проходимся по символам бинарной строки
        {
            if (combinations[i][j] == '1') //данная пара будет свапнута
            {
                if (letscheckit[pairs[j].first] or
                    letscheckit[pairs[j].second]) //если один из символов мы уже свапали, то помечаем флаг нулем
                    flag = 0;
                else
                { //отмечаем даныне символы свапнутыми и делаем свап
                    letscheckit[pairs[j].first] = 1;
                    letscheckit[pairs[j].second] = 1;
                    char t = wordcheck[pairs[j].first];
                    wordcheck[pairs[j].first] = wordcheck[pairs[j].second];
                    wordcheck[pairs[j].second] = t;
                }
            }
        }
        if (flag && dict.search(wordcheck)) //если с флагом все хорошо, то можно добавлять
            ispravlenia.push_back(wordcheck);
    }

}


void checkSpelling(ifstream &in, Dictionary &dict)
{

    int line_number = 0;

    while (in)
    {
        line_number++;
        set<string> totest;
        string line;
        getline(in, line);
        if (line[line.size() - 1] == '\r') //на Линуксе плохо с файлами от винды
            line.pop_back();
        stringstream ss(stringstream::in | stringstream::out);
        ss << line;
        vector<string> ispravlenia; //тут будет ответ
        string word; //считаываем слово
        while (ss >> word)
        {
            if (word.length() == 0 || dict.search(word)) //если слово нулевое или есть уже в словаре - пропускаем
                continue;

            word = stripPunct(word); //приведем слово к нижнему регистру и очистим от мусора
            lower(word);

            if (!dict.search(
                    word)) //если слово не в словаре, то выполняем четыре поиска ошибок(ух какая декомпозиция, я бы себе
            {//бонусом балла 2 добавил бы. Или хотя бы 1
                combination(in, dict, ispravlenia, word);
                removing(in, dict, ispravlenia, word);
                replacement(in, dict, ispravlenia, word);
                insertanyletter(in, dict, ispravlenia, word);
            }
            std::cout << "Line " << line_number << ": '" << word << "'\n";//информация для проверки
            std::cout << "\tsuggestions:" << "\n";
            output(in, dict, ispravlenia); //вывод исправленного файла
        }

    }
}

void lower(string &s)
{

    // Ensures that a word is lowercase
    for (int i = 0; i < s.length(); i++)
    {
        s[i] = tolower(s[i]);
    }
}

string stripPunct(const string &s)
{

    // Remove any single trailing
    // punctuation character from a word.
    if (ispunct(s[s.length() - 1]))
    {
        return s.substr(0, s.length() - 1);
    } else
    {
        return s;
    }
}
