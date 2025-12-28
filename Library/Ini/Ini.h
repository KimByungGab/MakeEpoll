#pragma once

#include <iostream>
#include <map>
#include <string>
#include <fstream>

using namespace std;

class Ini
{
public:
    Ini();

    /**
     * @brief 초기화
     * @author 김병갑
     * @param void
     * @return void
     */
    void Init();

    /**
     * @brief Ini 파일 읽기
     * @author 김병갑
     * @param filename 파일명
     * @return void
     */
    void loadIni(const string& filename);

    /**
     * @brief section과 key로 value 찾기
     * @author 김병갑
     * @param string 섹션명
     * @param string 키 이름
     * @return ini 값
     */
    string FindSectionData(string section, string key);

private:
    map<string, map<string, string>> m_iniData;
};