#include "IndexBufferBufFile.h"
#include <boost/algorithm/string.hpp>
#include <istream>
#include <fstream>
#include <set>
#include "MMTStringUtils.h"

#include "MMTConstants.h"

IndexBufferBufFile::IndexBufferBufFile(){

}

//通过一个NumberList来初始化IndexBufferBufFile，一般是组合IB文件的时候会用到
IndexBufferBufFile::IndexBufferBufFile(std::vector<uint64_t> NumberList) {
    this->NumberList = NumberList;

    UpdateAttributes();
}


void IndexBufferBufFile::UpdateAttributes() {
    uint64_t tmpMinNumber = 9999999;
    uint64_t tmpMaxNumber = 0;
    uint64_t readCount = 0;
    std::set<uint64_t> uniqueNumberSet;

    for (uint64_t Number : this->NumberList) {
        if (Number < tmpMinNumber) {
            tmpMinNumber = Number;
        }

        if (Number > tmpMaxNumber) {
            tmpMaxNumber = Number;
        }

        uniqueNumberSet.insert(Number);
        readCount++;
    }

    this->MaxNumber = tmpMaxNumber;
    this->MinNumber = tmpMinNumber;
    this->NumberCount = readCount;
    this->UniqueVertexCount = (uint64_t)uniqueNumberSet.size();
}


void IndexBufferBufFile::AddOffset(uint64_t Offset) {
    std::vector<uint64_t> NewNumberList;
    for (uint64_t Number : this->NumberList) {
        NewNumberList.push_back(Number + Offset);
    }
    this->NumberList = NewNumberList;
    UpdateAttributes();
}


IndexBufferBufFile::IndexBufferBufFile(std::wstring FileReadPath, std::wstring Format) {
    std::wstring FileName = MMTString::GetFileNameFromFilePath(FileReadPath);
    this->Index = FileName.substr(0,6);
    std::string lowerIBReadDxgiFormat = MMTString::ToByteString(boost::algorithm::to_lower_copy(Format));
    int readLength = -1;
    if (lowerIBReadDxgiFormat == MMTString::ToLowerCase(DXGI_FORMAT::DXGI_FORMAT_R16_UINT)) {
        readLength = 2;
    }
    else if (lowerIBReadDxgiFormat == MMTString::ToLowerCase(DXGI_FORMAT::DXGI_FORMAT_R32_UINT)) {
        readLength = 4;
    }
    std::ifstream ReadIBFile(FileReadPath, std::ios::binary);

    char* data = new char[readLength];

    int tmpMinNumber = 9999999;
    int tmpMaxNumber = 0;
    int readCount = 0;
    std::set<uint64_t> uniqueNumberSet;

    while (ReadIBFile.read(data, readLength)) {
        int number = 0;
        if (lowerIBReadDxgiFormat == MMTString::ToLowerCase(DXGI_FORMAT::DXGI_FORMAT_R16_UINT)) {
            std::uint16_t value = (static_cast<unsigned char>(data[1]) << 8) |
                static_cast<unsigned char>(data[0]);
            number = value;
        }

        if (lowerIBReadDxgiFormat == MMTString::ToLowerCase(DXGI_FORMAT::DXGI_FORMAT_R32_UINT)) {
            std::uint32_t value = (static_cast<unsigned char>(data[3]) << 24) |
                (static_cast<unsigned char>(data[2]) << 16) |
                (static_cast<unsigned char>(data[1]) << 8) |
                static_cast<unsigned char>(data[0]);
            number = value;
        }

        if (number < tmpMinNumber) {
            tmpMinNumber = number;
        }

        if (number > tmpMaxNumber) {
            tmpMaxNumber = number;
        }

        uniqueNumberSet.insert(number);

        this->NumberList.push_back(number);

        readCount++;
    }
    ReadIBFile.close();

    this->MaxNumber = tmpMaxNumber;
    this->MinNumber = tmpMinNumber;
    this->NumberCount = readCount;
    this->UniqueVertexCount = (uint64_t)uniqueNumberSet.size();
}


void IndexBufferBufFile::SaveToFile_UINT16(std::wstring FileWritePath, uint16_t Offset) {
    std::ofstream file(FileWritePath, std::ios::binary);
    for (const auto& data : this->NumberList) {
        uint16_t paddedData = data + Offset;
        file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint16_t));
    }
    file.close();
}


void IndexBufferBufFile::SaveToFile_UINT32(std::wstring FileWritePath, uint64_t Offset) {
    std::ofstream file(FileWritePath, std::ios::binary);
    for (const auto& data : this->NumberList) {
        uint32_t paddedData = data + Offset;
        file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
    }
    file.close();
}


std::string IndexBufferBufFile::GetSelfMinFormat() {
    if (this->MaxNumber > 65535) {
        return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
    }
    else {
        return DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
    }
}


void IndexBufferBufFile::SaveToFile_MinSize(std::wstring FileWritePath, uint64_t Offset) {
    if (this->MaxNumber > 65535) {
        SaveToFile_UINT32(FileWritePath, Offset);
    }
    else {
        SaveToFile_UINT16(FileWritePath, Offset);
    }
}


void IndexBufferBufFile::SelfDivide(int FirstIndex, int IndexCount) {
    //根据传入的索引和数量，截取出需要其中一部分IB值
    
    std::vector<uint64_t> newNumberList;
    std::set<uint64_t> uniqueNumberSet;
    int subCount = 0;
    uint64_t tmpMinNumber = 999999999;
    //uint64_t不能为负数，否则会变为uint64_t范围内的最大值，所以这里设为0
    uint64_t tmpMaxNumber = 0;
    for (int i = 0; i < this->NumberList.size(); i++) {

        if (i >= FirstIndex) {
            uint64_t number = this->NumberList[i];
            newNumberList.push_back(number);
            uniqueNumberSet.insert(number);
            subCount++;

            if (number < tmpMinNumber) {
                tmpMinNumber = number;
            }
            if (number > tmpMaxNumber) {
                tmpMaxNumber = number;
            }

            //读取指定数量后退出
            if (subCount == IndexCount) {
                break;
            }
        }
        
    }

    //并重新设置MinNumber MaxNumber NumberCount NumberList
    this->MinNumber = tmpMinNumber;
    this->MaxNumber = tmpMaxNumber;
    this->NumberCount = subCount;
    this->UniqueVertexCount = (uint64_t)uniqueNumberSet.size();
    this->NumberList = newNumberList;

}