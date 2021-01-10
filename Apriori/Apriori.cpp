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
#include <algorithm>

using namespace std;

float min_support = 0.2;
float min_freq = 0;

struct timespec Start, End, temp;
double diff(struct timespec start, struct timespec end){
    if((end.tv_nsec - start.tv_nsec) < 0){
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else{
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    double time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;
    return time_used;
}

int count_items(const vector<vector<int>> data, const set<int> item)
{
    int count = 0;
    vector<int> v(item.size());
    for(int i=0; i< data.size(); ++i){
        vector<int>::iterator it = set_intersection(data[i].begin(), data[i].end(), item.begin(), item.end(), v.begin());
        v.resize(it - v.begin());
        if(v.size() == item.size()){
            ++count;
        }
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
    clock_gettime(CLOCK_MONOTONIC, &Start);
    min_support = atof(argv[1]);
    string fileName = argv[2];
    char buffer[5000];
    int lineCount = 0;
    vector<string> data;

    
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

    vector<vector<int>> transcations;
    for (int i = 0; i < lineCount; ++i)
    {
        vector<int> a;
        transcations.push_back(a);
        SplitString(data[i], transcations[i], ",");
    }

    for(int i=0; i<transcations.size(); ++i){
        sort(transcations[i].begin(), transcations[i].end());
    }

    int max_item_num = -999;
    for(int i=0; i<transcations.size(); ++i){
        if(transcations[i][transcations[i].size()-1] > max_item_num){
            max_item_num = transcations[i][transcations[i].size()-1];
        }
    }

    min_freq = lineCount * min_support;
    cout << "min_freq: " << min_freq << endl;

    // print tanscation
    map<set<int>, int, mapComp> items;
    map<set<int>, int, mapComp>::iterator map_iter;
    // count all single items
    vector<int> counter(max_item_num+1, 0);
    for (int i = 0; i < transcations.size(); i++)
    {
        for (int j = 0; j < transcations[i].size(); j++)
        {
            counter[transcations[i][j]] += 1;
        }
    }

    // 只保留大於min_support的元素
    for(int i=0; i<max_item_num+1; ++i){ // 這裡可以做些優化，說不定max_item #會小於999
        if(counter[i] >= min_freq){
            set<int> tmp;
            tmp.insert(i);
            items[tmp] = counter[i];
        }
    }

    bool flag = false; // 是否有加入新的item
    map<set<int>, int, mapComp> old_map = items;
    map<set<int>, int, mapComp> new_map;

    // main function
    // time_t main_start = time(NULL);
    while(old_map.size() != 0)
    {
        // 對於old_map中每個元素暴力比較，看能不能merge
        for(map<set<int>, int, mapComp>::iterator map_iter = old_map.begin(); map_iter != next(old_map.end(), -1); ++map_iter)
        {
            for(map<set<int>, int, mapComp>::iterator map_iter2 = next(map_iter, 1); map_iter2 != old_map.end(); ++map_iter2)
            {
                set<int> new_set;
                set<int> set1Copy = map_iter->first;
                set<int> set2Copy = map_iter2->first;
                //檢查兩個set前n-1個元素是不是相同的，如果是相同則代表可以merge
                set<int>::iterator iter_temp[2];
                iter_temp[0] = next(set1Copy.end(), -1);
                iter_temp[1] = next(set2Copy.end(), -1);
                int tmp[2];
                tmp[0] = *iter_temp[0];
                tmp[1] = *iter_temp[1];
                // PrintSet(set1Copy);
                // PrintSet(set2Copy);
                // cout << "tmp[0] " << tmp[0] << endl;
                // cout << "tmp[1] " << tmp[1] << endl;
                // 刪除ItemSet最後一個元素
                set1Copy.erase(iter_temp[0]);
                set2Copy.erase(iter_temp[1]);

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
                    int count = count_items(transcations, new_set); // 這個function沒經過測試
                    if (count >= min_freq)
                    {
                        items[new_set] = count; // 大於min_support的放入items當中
                        new_map[new_set] = count;
                        flag = true;
                        // myPrint(items);
                    }
                }
            }
        }
        old_map = new_map;
        new_map.clear();
    }
    

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

    clock_gettime(CLOCK_MONOTONIC, &End);
    cout << "thread run time: " << diff(Start, End) << endl;

    // myPrint(items);
}