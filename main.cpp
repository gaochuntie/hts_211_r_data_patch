#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

using namespace std;

vector<string> lines; // 全局数据

// 步骤1：删除前两行
template<typename T>
void remove_first_n_lines(vector<T>& vec, int n) {
    if (vec.size() > n) vec.erase(vec.begin(), vec.begin() + n);
    else vec.clear();
}

// 步骤2：删除所有以BKB开头到下一个SC开头之间的内容（不含SC）
void remove_BKB_to_SC(vector<string>& vec) {
    vector<string> result;
    bool skip = false;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (!skip && vec[i].rfind("BKB", 0) == 0) {
            skip = true;
            continue;
        }
        if (skip && vec[i].rfind("SC", 0) == 0) {
            skip = false;
        }
        if (!skip) result.push_back(vec[i]);
    }
    vec = result;
}

// 步骤3：将所有\s+替换为,
void replace_whitespace_with_comma(vector<string>& vec) {
    regex ws_re("\\s+");
    for (auto& line : vec) {
        line = regex_replace(line, ws_re, ",");
    }
}

// 步骤4：将所有\nHVD,替换为""
void remove_newline_HVD(vector<string>& vec) {
    for (auto& line : vec) {
        size_t pos;
        while ((pos = line.find("\nHVD,")) != string::npos) {
            line.replace(pos, 6, "");
        }
    }
}

// 步骤5：将所有\nNEZ替换为""
void remove_newline_NEZ(vector<string>& vec) {
    for (auto& line : vec) {
        size_t pos;
        while ((pos = line.find("\nNEZ")) != string::npos) {
            line.replace(pos, 5, "");
        }
    }
}

// 步骤6：删除所有空行
void remove_empty_lines(vector<string>& vec) {
    vector<string> result;
    for (const auto& line : vec) {
        if (!line.empty() && line.find_first_not_of(", ") != string::npos) {
            result.push_back(line);
        }
    }
    vec = result;
}

// 步骤7：删除行首的SC,
void remove_SC_prefix(vector<string>& vec) {
    for (auto& line : vec) {
        if (line.rfind("SC,", 0) == 0) {
            line = line.substr(3);
        }
    }
}

// 步骤8：删除第1个,到第5个,之间的内容（不包括第1和第5个,本身）
void remove_between_commas(vector<string>& vec) {
    for (auto& line : vec) {
        size_t first = line.find(",");
        if (first == string::npos) continue;
        size_t fifth = first;
        int count = 1;
        while (count < 5 && fifth != string::npos) {
            fifth = line.find(",", fifth + 1);
            ++count;
        }
        if (count == 5 && fifth != string::npos) {
            line = line.substr(0, first + 1) + line.substr(fifth);
        }
    }
}

// 步骤4+5：合并HVD和NEZ行到上一行
void merge_HVD_NEZ_to_prev_line(vector<string>& vec) {
    for (size_t i = 1; i < vec.size(); ) {
        if (vec[i].rfind("HVD,", 0) == 0) {
            vec[i-1] += vec[i].substr(4); // 合并去掉HVD,
            vec.erase(vec.begin() + i);
        } else if (vec[i].rfind("NEZ", 0) == 0) {
            vec[i-1] += vec[i].substr(3); // 合并去掉NEZ
            vec.erase(vec.begin() + i);
        } else {
            ++i;
        }
    }
}

// 读取文件到全局lines
bool read_file(const string& filename) {
    ifstream fin(filename);
    if (!fin) return false;
    string line;
    while (getline(fin, line)) {
        lines.push_back(line);
    }
    fin.close();
    return true;
}

// 步骤3-5需要将所有内容合并为一行再处理，然后再按行分割
template<typename T>
void join_lines_to_single_string(const vector<T>& vec, string& out) {
    ostringstream oss;
    for (const auto& l : vec) {
        oss << l << "\n";
    }
    out = oss.str();
}

void split_string_to_lines(const string& in, vector<string>& out) {
    out.clear();
    istringstream iss(in);
    string line;
    while (getline(iss, line)) {
        out.push_back(line);
    }
}

// 写入文件
bool write_file(const string& filename, const vector<string>& vec) {
    ofstream fout(filename, ios::trunc);
    if (!fout) return false;
    for (const auto& line : vec) {
        fout << line << "\n";
    }
    fout.close();
    return true;
}

int main() {
    system("chcp 65001 >nul");
    cout << "仅支持华星HTS-211R使用，请将文件复制粘贴并且重命名为odata.txt放置于本文件夹内，回车开始格式化文档" << endl;
    cin.get();
    if (!read_file("odata.txt")) {
        cout << "无法打开odata.txt，请检查文件是否存在。" << endl;
        return 1;
    }
    // 步骤1
    remove_first_n_lines(lines, 2);
    // 步骤2
    remove_BKB_to_SC(lines);
    // 步骤3：每行空白替换为逗号
    replace_whitespace_with_comma(lines);
    // 步骤4+5：合并HVD和NEZ行到上一行
    merge_HVD_NEZ_to_prev_line(lines);
    // 步骤6
    remove_empty_lines(lines);
    // 步骤7
    remove_SC_prefix(lines);
    // 步骤8
    remove_between_commas(lines);
    // 步骤9
    if (write_file("final.dat", lines)) {
        cout << "格式化完成，结果已保存为final.dat。" << endl;
    } else {
        cout << "写入final.dat失败。" << endl;
    }
    cout<<"最终文件为final.dat"<<endl;
    system("pause");
    return 0;
}
