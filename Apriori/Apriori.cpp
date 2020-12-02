#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iomanip>
#include <iterator>
#include <time.h>

using namespace std;

float min_support = 0.2;
float min_freq = 0;

int count_items(const vector<vector<int>> data, const set<int> item)
{
    set<int>::iterator iter;
    int count = 0;
    for (int i = 0; i < data.size(); ++i)
    {
        // parallel
        bool findAllItem = false; // 檢查是不是所有的item都在transcation當中
        for (iter = item.begin(); iter != item.end(); ++iter)
        {
            bool find = false;
            for (int j = 0; j < data[i].size(); ++j)
            {
                if (*iter == data[i][j]) // 表示找到了
                {
                    find = true;
                    break;
                }
            }
            if (!find)
                break;
            // cout << "crash here?" << endl;
            if (iter == next(item.end(), -1) && find)
                findAllItem = true;
        }
        if (findAllItem)
            ++count;
    }
    return count;
}

class mapComp
{
public:
    bool operator()(const set<int> &s1, const set<int> &s2) const
    {
        if (s1.size() != s2.size())
            if (s1.size() < s2.size())
                return true;
            else
                return false;
        else
            return s1 < s2;
    }
};

void SplitString(const string &s, vector<int> &v, const string &c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        string tmp = s.substr(pos1, pos2 - pos1);
        v.push_back(atoi(tmp.c_str()));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
    {
        string tmp = s.substr(pos1);
        v.push_back(atoi(tmp.c_str()));
    }
}

void del_element(map<set<int>, int, mapComp> &items)
{
    vector<set<int>> del;
    set<int>::iterator set_iter;

    map<set<int>, int, mapComp>::iterator map_iter;

    for (map_iter = items.begin(); map_iter != items.end(); map_iter++)
    {
        if (map_iter->second < min_freq)
            del.push_back(map_iter->first);
    }

    for (vector<set<int>>::iterator del_iter = del.begin(); del_iter != del.end(); ++del_iter)
        items.erase(*del_iter);
}

void myPrint(map<set<int>, int, mapComp> &items)
{
    map<set<int>, int, mapComp>::iterator map_iter;
    set<int>::iterator set_iter;
    // 打印出結果
    for (map_iter = items.begin(); map_iter != items.end(); map_iter++)
    {
        cout << "items: {";
        for (set_iter = (map_iter->first).begin(); set_iter != (map_iter->first).end(); set_iter++)
        {
            cout << *set_iter << ",";
        }
        cout << "}, count:";
        cout << map_iter->second << endl;
    }
}

void PrintSet(const set<int> a)
{
    set<int>::iterator iter = a.begin();
    for (iter = a.begin(); iter != a.end(); ++iter)
        cout << *iter << ",";
    cout << endl;
}

int main(int argc, char *argv[])
{
    time_t start = time(NULL);
    min_support = atof(argv[1]);
    string fileName = argv[2];
    char buffer[5000];
    int lineCount = 0;
    vector<string> data;

    // time_t startReadTime = time(NULL);
    fstream file;
    file.open(fileName);

    do
    {
        file.getline(buffer, sizeof(buffer));
        string line(buffer);
        data.push_back(line);
        lineCount += 1;
    } while (!file.eof());
    lineCount -= 1;

    file.close();

    // time_t read_end = time(NULL);
    // cout << "read time:" << int(read_end - startReadTime) << endl;

    vector<vector<int>> transcation;
    for (int i = 0; i < lineCount; ++i)
    {
        vector<int> a;
        transcation.push_back(a);
        SplitString(data[i], transcation[i], ",");
    }

    min_freq = lineCount * min_support;
    cout << "min_freq: " << min_freq << endl;

    // print tanscation
    map<set<int>, int, mapComp> items;
    map<set<int>, int, mapComp>::iterator map_iter;
    // count all single items
    for (int i = 0; i < transcation.size(); i++)
    {
        for (int j = 0; j < transcation[i].size(); j++)
        {
            set<int> tmp;
            tmp.insert(transcation[i][j]);
            map_iter = items.find(tmp);
            // key is in item
            if (map_iter != items.end())
            {
                map_iter->second += 1;
            }
            else
            {
                items[tmp] = 1;
            }
        }
    }

    // 刪除小於min_support的元素
    del_element(items);

    bool flag = false; // 是否有加入新的item
    map<set<int>, int, mapComp> old_map = items;
    map<set<int>, int, mapComp> new_map;

    // main function
    // time_t main_start = time(NULL);
    do
    {
        flag = false; // add new item to map or not

        set<int> new_set;
        int tmp[2];
        map<set<int>, int, mapComp>::iterator map_iter;
        map<set<int>, int, mapComp>::iterator map_iter2;
        // 對於old_map中每個元素暴力比較，看能不能merge
        for (map_iter = old_map.begin(); map_iter != next(old_map.end(), -1); ++map_iter)
        {
            for (map_iter2 = next(map_iter, 1); map_iter2 != old_map.end(); ++map_iter2)
            {
                //檢查兩個set前n-1個元素是不是相同的，如果是相同則代表可以merge
                tmp[0] = *(map_iter->first).rbegin();
                tmp[1] = *(map_iter2->first).rbegin();
                // cout << "tmp[0] " << tmp[0] << endl;
                // cout << "tmp[1] " << tmp[1] << endl;

                set<int> set1Copy = map_iter->first;
                set<int> set2Copy = map_iter2->first;
                set1Copy.erase(tmp[0]);
                set2Copy.erase(tmp[1]);

                bool compare_flag = (set1Copy == set2Copy);
                // 兩個set相同，且他們組合成的新Set的數目會大於min_support就加到map當中
                if (compare_flag)
                {
                    new_set.clear();
                    new_set = set1Copy;
                    new_set.insert(tmp[0]);
                    new_set.insert(tmp[1]);
                    // cout << "new set is" << endl;
                    // cout << "-------------" << endl;
                    // PrintSet(new_set);
                    // cout << "-------------" << endl;
                    int count = count_items(transcation, new_set); // 這個function沒經過測試
                    if (count >= min_freq)
                    {
                        new_map[new_set] = count;
                        flag = true;
                        // myPrint(items);
                    }
                }
            }
        }
        // 將創造出來的新item放到item當中
        for (map_iter = new_map.begin(); map_iter != new_map.end(); ++map_iter)
        {
            items[map_iter->first] = map_iter->second;
        }
        old_map = new_map;
        new_map.clear();
    } while (flag);
    // time_t main_end = time(NULL);
    // cout << "calcuation time: " << int(main_end - main_start) << endl;

    // 寫入檔案
    // time_t write_start = time(NULL);
    fstream fs;
    fs.open(argv[3], ios::out);

    for (map_iter = items.begin(); map_iter != items.end(); ++map_iter)
    {
        set<int> _set;
        set<int>::iterator SetIter;
        _set = map_iter->first;
        for (SetIter = _set.begin(); SetIter != _set.end(); ++SetIter)
        {
            fs << *SetIter;
            if (SetIter != (--_set.end()))
                fs << ',';
            else
                fs << ":";
        }
        fs << fixed << setprecision(4) << float(map_iter->second) / float(lineCount) << endl;
    }
    // time_t write_end = time(NULL);
    // cout << "write time: " << int(write_end - write_start) << endl;

    fs.close();

    time_t end = time(NULL);
    cout << "run time: " << (end - start) << endl;

    // myPrint(items);
}