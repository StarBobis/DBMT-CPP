#include "VertexBufferBufFile.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"
#include <fstream>
#include "MMTFormatUtils.h"
#include "MMTLogUtils.h"

VertexBufferBufFile::VertexBufferBufFile() {

}


VertexBufferBufFile::VertexBufferBufFile(std::wstring readVBBufFilePath, D3D11GameType d3d11GameType, std::vector<std::string> elementList) {

    int SplitStride = d3d11GameType.getElementListStride(elementList);
    std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(elementList);
    std::unordered_map<std::string, uint64_t> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(elementList);

    uint64_t VBFileSize = MMTFile::GetFileSize(readVBBufFilePath);
    uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

    //读取数据
    std::vector<std::byte> buffer(VBFileSize);
    std::ifstream VBFile(MMTString::ToByteString(readVBBufFilePath), std::ios::binary);
    VBFile.read(reinterpret_cast<char*>(buffer.data()), VBFileSize);
    VBFile.close();

    //通过类型，获取读取长度，这里还是有序的
    std::vector<int> readLengths;
    for (const std::string& category : CategoryList) {
        readLengths.push_back(CategoryStrideMap[category]);
    }

    int offset = 0;
    while (offset < buffer.size()) {
        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            // 指定要读取的字节长度
            int readLength = readLengths[i];
            std::vector<std::byte> categoryDataPatch;
            // 预分配足够的内存空间
            categoryDataPatch.reserve(readLength);
            // 使用std::copy而不是循环来提高效率
            std::copy(buffer.begin() + offset, buffer.begin() + offset + readLength, std::back_inserter(categoryDataPatch));

            // 找到tmp的这个
            std::unordered_map<std::wstring, std::vector<std::byte>>& tmpVBCategoryDataMap = this->CategoryVBDataMap;

            // 找到tmp的这个对应的category的数据
            std::vector<std::byte>& categoryData = tmpVBCategoryDataMap[MMTString::ToWideString(category)];


            // 将categoryDataPatch的数据直接插入categoryData
            categoryData.insert(categoryData.end(), categoryDataPatch.begin(), categoryDataPatch.end());

            offset += readLength;
        }

    }



};


void VertexBufferBufFile::SelfDivide(int MinNumber, int MaxNumber, int stride) {
    //这里MinNumber不额外+1的原因是，如果从0开始的话，就会读取包括0的部分
    //这里MaxNumber要+1的原因是，GetRange模仿python里的[i:i+1] 但是不包含i+1的效果看，所以不包括+1的那个
    //所以+1才能避免少读取一个
    LOG.Info(L"VertexBufferBufFile::SelfDivide(MinNumber:" + std::to_wstring(MinNumber) + L",MaxNumber:" + std::to_wstring(MaxNumber) + L",stride=" + std::to_wstring(stride) + L");");
    LOG.Info(L"this->FinalVB0Buf.size():"+std::to_wstring(this->FinalVB0Buf.size()) + L" this->FinalVB0Buf.size()/stride = VertexCount:" + std::to_wstring(this->FinalVB0Buf.size()/stride));

    this->FinalVB0Buf = MMTFormat::GetRange_Byte(this->FinalVB0Buf, stride * (MinNumber), stride * (MaxNumber+1));
}


void VertexBufferBufFile::SaveToFile(std::wstring VB0OutputFilePath) {
    LOG.Info(L"VertexBufferBufFile输出大小：" + std::to_wstring(this->FinalVB0Buf.size()));
    std::ofstream outputVBFile(VB0OutputFilePath, std::ofstream::binary);
    outputVBFile.write(reinterpret_cast<const char*>(this->FinalVB0Buf.data()), this->FinalVB0Buf.size());
    outputVBFile.close();
}



VertexBuffer::VertexBuffer(std::wstring ReadFilePath, uint64_t VertexNumber) {
    this->VertexIndex_ByteVector_Map = MMTFile::ReadBufMapFromFile(ReadFilePath, VertexNumber);


}


VertexBuffer::VertexBuffer(std::wstring ReadFilePath) {
    std::ifstream BufFile(ReadFilePath, std::ifstream::binary);
    BufFile.seekg(0, std::ios::end);
    size_t FileSize = BufFile.tellg();
    BufFile.seekg(0, std::ios::beg);

    std::vector<std::byte> Buffer(FileSize);
    BufFile.read(reinterpret_cast<char*>(Buffer.data()), FileSize);
    BufFile.close();

    this->ByteVector = Buffer;
}