#include "Ini.h"

Ini::Ini()
{
    Init();
}

void Ini::Init()
{
    m_iniData.clear();
}

void Ini::loadIni(const string& filename)
{
    ifstream file(filename);
    string line, section;

    while(getline(file, line))
    {
        // 엔터거나 주석이면 건너뛰기
        if(line.empty() || line[0] == ';')
            continue;

        // 섹션 찾음
        if(line.front() == '[' && line.back() == ']')
            section = line.substr(1, line.size() - 2);
        else
        {
            // 섹션이 아니면 키&밸류겠지?
            size_t equalPos = line.find('=');

            // 이퀄을 찾았으면 키 밸류 넣기
            if(equalPos != string::npos)
            {
                string key = line.substr(0, equalPos);
                string value = line.substr(equalPos + 1);
                m_iniData[section][key] = value;
            }
        }
    }
}

string Ini::FindSectionData(string section, string key)
{
    auto sectionIter = m_iniData.find(section);
    if(sectionIter == m_iniData.end())
        return string();

    auto mapKeyValue = sectionIter->second;
    auto keyIter = mapKeyValue.find(key);
    if(keyIter == mapKeyValue.end())
        return string();

    return keyIter->second;
}