#include <filesystem>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "FrameAnalysisData.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "MMTFileUtils.h"
#include "MMTLogUtils.h"
#include "IndexBufferBufFile.h"
#include "MMTStringUtils.h"
#include <set>
#include <unordered_map>
#include "ConstantBufferBufFile.h"
#include "IndexBufferTxtFile.h"
#include "MMTFormatUtils.h"


void FrameAnalysisData::ReadFrameAnalysisFileList(std::wstring WorkFolder) {
    this->WorkFolder = WorkFolder;
    this->FrameAnalysisFileNameList.clear();
    for (const auto& entry : std::filesystem::directory_iterator(WorkFolder)) {
        if (!(entry.is_regular_file())) {
            continue;
        }
        this->FrameAnalysisFileNameList.push_back(entry.path().filename().wstring());
    }

    //����Ҳ���.buf�ļ��򵯳�����
    if (this->FindFrameAnalysisFileNameListWithCondition(L"", L".buf").size() == 0) {
        LOG.Error("Can't find any .buf file, please set your analyse_option config in your d3dx.ini to \nanalyse_options = dump_rt dump_tex  dump_cb dump_vb dump_ib buf txt  ");
    }
}


FrameAnalysisData::FrameAnalysisData() {

}


FrameAnalysisData::FrameAnalysisData(std::wstring WorkFolder, std::wstring DrawIB) {
    this->ReadFrameAnalysisFileList(WorkFolder);
    this->TrianglelistIndexList = this->ReadTrianglelistIndexList(DrawIB);
    this->PointlistIndexList = this->ReadPointlistIndexList();
}


FrameAnalysisData::FrameAnalysisData(std::wstring WorkFolder) {
    this->ReadFrameAnalysisFileList(WorkFolder);
}


std::vector<std::wstring> FrameAnalysisData::FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr) {
    std::vector<std::wstring> findFileNameList;
    for (std::wstring fileName :this->FrameAnalysisFileNameList ) {
        if (fileName.find(searchStr) != std::string::npos && fileName.substr(fileName.length() - endStr.length()) == endStr) {
            findFileNameList.push_back(fileName);
        }
    }
    return findFileNameList;
}


std::vector<std::wstring> FrameAnalysisData::ReadTrianglelistIndexList(std::wstring drawIB) {
    //���ȸ���DrawIB����ȡ���е�Trianglelist Index
    std::vector<std::wstring> trianglelistIndexList;
    for (std::wstring fileName : this->FrameAnalysisFileNameList) {
        //ֻ��Ҫtxt�ļ�
        if (!boost::algorithm::ends_with(fileName, L".txt")) {
            continue;
        }
        //�������DrawIB
        if (fileName.find(L"-ib=" + drawIB) != std::string::npos) {
            IndexBufferTxtFile ibTxtFile(this->WorkFolder + fileName, false);
            if (ibTxtFile.Topology == L"trianglelist") {
                if (FindFrameAnalysisFileNameListWithCondition(ibTxtFile.Index + L"-vb0", L".txt").size() != 0) {
                    trianglelistIndexList.push_back(ibTxtFile.Index);
                }
            }
        }
    }
    return trianglelistIndexList;
}


std::vector<std::wstring> FrameAnalysisData::ReadPointlistIndexList() {
    std::vector<std::wstring> pointlistIndexList;
    for (std::wstring fileName: this->FrameAnalysisFileNameList) {
        //ֻ��Ҫtxt�ļ�
        if (!boost::algorithm::ends_with(fileName, L".txt")) {
            continue;
        }

        if (fileName.find(L"-vb0") != std::string::npos) {
            VertexBufferDetect vbDetect(this->WorkFolder + fileName);
            if (vbDetect.Topology == L"pointlist") {
                pointlistIndexList.push_back(vbDetect.Index);
            }
        }
    }
    return pointlistIndexList;
}


std::map<std::wstring, uint64_t> FrameAnalysisData::Get_TrianglelistIndex_VertexCount_Map(VertexCountType InputVertexCountType, std::wstring DrawIB) {
    LOG.Info(L"��ʼ��ȡTrianglelistIndex_VertexCount_Map");

    //����Ѿ���ȡ���Ļ����͸������棬��ֹ��ζ�ȡ��
    if (this->DrawIB_TrianglelistIndex_VertexCount_Map.contains(DrawIB)) {
        this->Show_Trianglelist_Index_VertexCount_Map(DrawIB);
        return this->DrawIB_TrianglelistIndex_VertexCount_Map[DrawIB];
    }

    //1.��Ϊ�Ǹ���DrawIB�����ģ����Խ���������ã�����ÿ�ζ����¸�ֵ
    std::map<std::wstring, uint64_t> Tmp_TrianglelistIndex_VertexCount_Map;
    for (std::wstring FileName : this->FrameAnalysisFileNameList) {
        if (!FileName.ends_with(L".txt")) {
            continue;
        }
        if (FileName.find(L"-ib") != std::string::npos && FileName.find(DrawIB) != std::string::npos) {
            //LOG.Info(L"Processing ib file: " + FileName);
            IndexBufferTxtFile indexBufferFileData = IndexBufferTxtFile(this->WorkFolder + FileName, false);
            if (indexBufferFileData.Topology == L"trianglelist") {
                std::vector<std::wstring> tmpNameList = this->FindFrameAnalysisFileNameListWithCondition(indexBufferFileData.Index + L"-vb0", L".txt");
                if (tmpNameList.size() == 0) {
                    //LOG.Info(L"Special Type, Continue : Can't find vb0 for -ib:" + FileName);
                    continue;
                }
                VertexBufferDetect vertexBufferDetect(this->WorkFolder + tmpNameList[0]);
                LOG.Info(L"���ڼ��Trianglelist�ļ���" + FileName);

                LOG.Info(L"BufFileRemoveNull:" + std::to_wstring(vertexBufferDetect.fileBufferRealVertexCount));
                LOG.Info(L"BufFileTotal:" + std::to_wstring(vertexBufferDetect.fileBufferVertexCount));
                LOG.Info(L"TxtFileCountLine:" + std::to_wstring(vertexBufferDetect.fileRealVertexCount));
                LOG.Info(L"TxtFileShow:" + std::to_wstring(vertexBufferDetect.fileShowVertexCount));
                LOG.NewLine();

                if (VertexCountType::BufFileRemoveNull == InputVertexCountType) {
                    Tmp_TrianglelistIndex_VertexCount_Map[vertexBufferDetect.Index] = vertexBufferDetect.fileBufferRealVertexCount;
                }
                else if(VertexCountType::BufFileTotal == InputVertexCountType){
                    Tmp_TrianglelistIndex_VertexCount_Map[vertexBufferDetect.Index] = vertexBufferDetect.fileBufferVertexCount;
                }
                else if (VertexCountType::TxtFileCountLine == InputVertexCountType) {
                    Tmp_TrianglelistIndex_VertexCount_Map[vertexBufferDetect.Index] = vertexBufferDetect.fileRealVertexCount;
                }
                else if (VertexCountType::TxtFileShow == InputVertexCountType) {
                    Tmp_TrianglelistIndex_VertexCount_Map[vertexBufferDetect.Index] = vertexBufferDetect.fileShowVertexCount;

                    if (vertexBufferDetect.fileShowVertexCount == 0) {
                        LOG.Warning(L"�����޷���Trianlgelist��txt�ļ��ж�ȡ���ݣ�ʹ�����ж�ȡ�����������");
                        Tmp_TrianglelistIndex_VertexCount_Map[vertexBufferDetect.Index] = vertexBufferDetect.fileRealVertexCount;
                    }

                }
            }
        }
    }
    this->DrawIB_TrianglelistIndex_VertexCount_Map[DrawIB] = Tmp_TrianglelistIndex_VertexCount_Map;

    LOG.NewLine();
    this->Show_Trianglelist_Index_VertexCount_Map(DrawIB);
    return this->DrawIB_TrianglelistIndex_VertexCount_Map[DrawIB];
}

std::map<std::wstring, uint64_t> FrameAnalysisData::Get_PointlistIndex_VertexCount_Map(VertexCountType InputVertexCountType) {
    LOG.Info(L"��ʼ��ȡPointlistIndex_VertexCount_Map");
    //��ΪPointlist�ǹ̶��ģ����Կ����ظ�ʹ��֮ǰ�Ľ��
    if (this->PointlistIndex_VertexCount_Map.size() != 0) {
        this->Show_Pointlist_Index_VertexCount_Map();
        return this->PointlistIndex_VertexCount_Map;
    }

    std::map<std::wstring, uint64_t> Tmp_PointlistIndex_VertexCount_Map;
    for (std::wstring FileName : this->FrameAnalysisFileNameList) {
        if (!FileName.ends_with(L".txt")) {
            continue;
        }
        if (FileName.find(L"-vb0") != std::string::npos) {
            VertexBufferDetect vbDetect(this->WorkFolder + FileName);

            if (vbDetect.Topology == L"pointlist") {
                LOG.Info(L"���ڼ��pointlist�ļ���" + FileName);

                LOG.Info(L"BufFileRemoveNull:" + std::to_wstring(vbDetect.fileBufferRealVertexCount));
                LOG.Info(L"BufFileTotal:" + std::to_wstring(vbDetect.fileBufferVertexCount));
                LOG.Info(L"TxtFileCountLine:" + std::to_wstring(vbDetect.fileRealVertexCount));
                LOG.Info(L"TxtFileShow:" + std::to_wstring(vbDetect.fileShowVertexCount));
                LOG.NewLine();

                if (VertexCountType::BufFileRemoveNull == InputVertexCountType) {
                    Tmp_PointlistIndex_VertexCount_Map[vbDetect.Index] = vbDetect.fileBufferRealVertexCount;
                }
                else if (VertexCountType::BufFileTotal == InputVertexCountType) {
                    Tmp_PointlistIndex_VertexCount_Map[vbDetect.Index] = vbDetect.fileBufferVertexCount;
                }
                else if (VertexCountType::TxtFileCountLine == InputVertexCountType) {
                    Tmp_PointlistIndex_VertexCount_Map[vbDetect.Index] = vbDetect.fileRealVertexCount;
                }
                else if (VertexCountType::TxtFileShow == InputVertexCountType) {
                    Tmp_PointlistIndex_VertexCount_Map[vbDetect.Index] = vbDetect.fileShowVertexCount;
                }
            }
        }
    }

    this->PointlistIndex_VertexCount_Map = Tmp_PointlistIndex_VertexCount_Map;
    LOG.NewLine();
    this->Show_Pointlist_Index_VertexCount_Map();
    return this->PointlistIndex_VertexCount_Map;
}


void FrameAnalysisData::Show_Pointlist_Index_VertexCount_Map() {
    LOG.Info(L"PointlistIndexVertexNumberMap:");
    for (const auto& pair : this->PointlistIndex_VertexCount_Map) {
        LOG.Info(L"Index: " + pair.first + L", VertexCount: " + std::to_wstring(pair.second));
    }
    LOG.NewLine();
}

void FrameAnalysisData::Show_Trianglelist_Index_VertexCount_Map(std::wstring DrawIB) {
    LOG.Info(L"TrianglelistIndexVertexNumberMap:");
    for (const auto& pair : this->DrawIB_TrianglelistIndex_VertexCount_Map[DrawIB]) {
        LOG.Info(L"Index:" + pair.first + L", VertexCount: " + std::to_wstring(pair.second));
    }
    LOG.NewLine();
}





//����Mod������£�Dump����IB���иı䣬����ͨ������log�ļ����ҵ����IB����ʵIBֵ
std::wstring FrameAnalysisData::Get_RealDrawIB_FromLog(std::wstring DrawIB) {
    std::vector<std::wstring> originalIBFileList = this->FindFrameAnalysisFileNameListWithCondition(DrawIB,L".txt");
    if (originalIBFileList.size() > 0) {
        return DrawIB;
    }
    //��������Ҫͨ������log�ļ����ҵ����IB����ʵIBֵ
    std::wstring LogTxtFilePath = this->WorkFolder + L"log.txt";
    if (!std::filesystem::exists(LogTxtFilePath)) {
        LOG.Error(L"Can't find log.txt in your FrameAnalysisFolder: " + this->WorkFolder);
    }
    LOG.Info("Find log.txt in FrameAnalysisFolder");
    std::vector<std::wstring> LogLineList = MMTFile::ReadAllLinesW(LogTxtFilePath);

    //���Ұ���hash=DrawIB����һ��
    std::wstring Index = L"";
    for (std::wstring logLine : LogLineList) {
        if (logLine.find(L"hash=" + DrawIB) != std::wstring::npos) {
            Index = logLine.substr(0, 6);
            break;
        }
    }
    if (Index == L"") {
        LOG.Error(L"Can't find valid index for: " + DrawIB);
    }
    else {
        LOG.Info(L"Find Index:" + Index);
    }

    std::vector<std::wstring> ibHashFileList = this->FindFrameAnalysisFileNameListWithCondition(Index + L"-ib", L".txt");
    if (ibHashFileList.size() == 0) {
        LOG.Error(L"Can't find ib file for index: " + Index);
    }
    std::wstring ibFileName = ibHashFileList[0];
    LOG.Info(L"Find ibFileName: " + ibFileName);
    std::wstring NewIBHash = ibFileName.substr(10, 8);
    LOG.Info(L"Find NewIB Hash success: " + NewIBHash);
    LOG.NewLine();
    return NewIBHash;

}



std::wstring FrameAnalysisData::FindDedupedTextureName(std::wstring WorkFolder, std::wstring TextureFileName) {
    //ȥdeduped�ļ����������ͼ��Ӧ��hash��Ӧ���ļ���Ȼ���ȡ���format��Ϣ
    //��Ϊdiffuse��ͼ�϶�����BC7_UNORM_SRGB����BC7_UNORM  ����0b3e1922-BC7_UNORM_SRGB.dds 
    //�������Թ��˵�һ��λ��ͬ��һ���ֽ����
    //�Ƚ����⣬����texture_hash = 1 ���޷���Ч,����WWMI�޷���Ч��
    std::wstring dedupedFolderPath = WorkFolder + L"deduped\\";
    std::wstring DiffuseHashValue = MMTString::GetFileHashFromFileName(TextureFileName);
    if (TextureFileName.ends_with(L".dds")) {
        std::vector<std::wstring> textureDDSFileNameList = MMTFile::FindFileNameListWithCondition(dedupedFolderPath, DiffuseHashValue, L".dds");
        if (textureDDSFileNameList.size() == 0) {
            LOG.Info(L"�޷���deduped�ļ������ҵ���Hashֵ��dds��ͼ�ļ�:" + DiffuseHashValue);
            return L"";
        }
        else {
            return textureDDSFileNameList[0];
        }
    }
    else if (TextureFileName.ends_with(L".jpg")) {
        std::vector<std::wstring> textureJPGFileNameList = MMTFile::FindFileNameListWithCondition(dedupedFolderPath, DiffuseHashValue, L".jpg");
        if (textureJPGFileNameList.size() == 0) {
            LOG.Info(L"�޷���deduped�ļ������ҵ���Hashֵ��jpg��ͼ�ļ�:" + DiffuseHashValue);
            return L"";
        }
        else {
            return textureJPGFileNameList[0];
        }
    }
    else {
        return L"";
    }
}


std::wstring FrameAnalysisData::GetIBMatchFirstIndexByIndex(std::wstring WorkFolder, std::wstring Index) {
    std::vector<std::wstring> ibFileNameList = FindFrameAnalysisFileNameListWithCondition(Index + L"-ib", L".txt");
    std::wstring ibFileName = L"";
    if (ibFileNameList.size() >= 1) {
        ibFileName = ibFileNameList[0];
    }
    else {
        LOG.Warning(L"Can't find ib file for index: " + Index);
        return L"";
    }
    IndexBufferTxtFile ibTxtFile(WorkFolder + ibFileName, false);
    //LOG.Info(L"��ǰMatch_First_index: " + ibTxtFile.FirstIndex);
    return ibTxtFile.FirstIndex;
}


IndexBufferBufFile FrameAnalysisData::GetIBBufFileByIndex(std::wstring Index) {
    LOG.Info(L"����ʹ��Index: " + Index + L"����ȡ��Ӧ��IB��txt�ļ�:");
    IndexBufferBufFile EmptyIBBufFile;
    std::vector<std::wstring> IBTxtFileNameList = this->FindFrameAnalysisFileNameListWithCondition(Index + L"-ib=", L".txt");
    if (IBTxtFileNameList.size() == 0) {
        LOG.Warning(L"�޷�ͨ��Index��ȡIB��txt�ļ�,���ؿն���");
        return EmptyIBBufFile;
    }
    std::wstring IBTxtFileName = IBTxtFileNameList[0];
    IndexBufferTxtFile IBTxtFile(this->WorkFolder + IBTxtFileName, false);
    std::wstring Format = IBTxtFile.Format;
    std::wstring IBBufFileName = MMTString::GetFileNameWithOutSuffix(IBTxtFileName) + L".buf";
    IndexBufferBufFile IBBufFile(this->WorkFolder + IBBufFileName, Format);
    IBBufFile.MatchFirstIndex = IBTxtFile.FirstIndex;
    return IBBufFile;
}


std::unordered_map<std::wstring, IndexBufferBufFile> FrameAnalysisData::Get_MatchFirstIndex_IBBufFile_Map_FromLog(std::wstring DrawIB) {
    LOG.Info(L"Get_MatchFirstIndex_IBBufFile_Map_FromLog: ");
    //��������Ҫͨ������log�ļ����ҵ����IB����ʵIBֵ
    std::wstring LogTxtFilePath = this->WorkFolder + L"log.txt";
    if (!std::filesystem::exists(LogTxtFilePath)) {
        LOG.Error(L"Can't find log.txt in your FrameAnalysisFolder: " + this->WorkFolder);
    }
    LOG.Info("Find log.txt in FrameAnalysisFolder");
    std::vector<std::wstring> LogLineList = MMTFile::ReadAllLinesW(LogTxtFilePath);

    //���Ұ���hash=DrawIB����һ��
    std::unordered_map<std::wstring, IndexBufferBufFile> UniqueVertexCount_IBBufFile_Map;
    for (std::wstring logLine : LogLineList) {
        if (logLine.find(L"hash=" + DrawIB) != std::wstring::npos) {
            std::wstring Index = logLine.substr(0, 6);
            //ֱ���ҵ���Ӧ��IB�ļ�
            IndexBufferBufFile IBBufFile = this->GetIBBufFileByIndex(Index);

            //������Ҫ�����Log�ļ���鵽Index����ʵDrawNumber
            
            if (IBBufFile.UniqueVertexCount != 0) {
                if (!UniqueVertexCount_IBBufFile_Map.contains(IBBufFile.MatchFirstIndex)) {
                    UniqueVertexCount_IBBufFile_Map[IBBufFile.MatchFirstIndex] = IBBufFile;
                    LOG.Info(L"Detect MatchFirstIndex: " + IBBufFile.MatchFirstIndex);
                }
            }
        }
    }
    return UniqueVertexCount_IBBufFile_Map;
    LOG.NewLine();
}


std::vector<std::wstring> FrameAnalysisData::ReadRealTrianglelistIndexListFromLog(std::wstring OriginalDrawIB) {
    //��������Ҫͨ������log�ļ����ҵ����IB����ʵIBֵ
    std::wstring LogTxtFilePath = this->WorkFolder + L"log.txt";
    if (!std::filesystem::exists(LogTxtFilePath)) {
        LOG.Error(L"Can't find log.txt in your FrameAnalysisFolder: " + this->WorkFolder);
    }
    LOG.Info("Find log.txt in FrameAnalysisFolder");
    std::vector<std::wstring> LogLineList = MMTFile::ReadAllLinesW(LogTxtFilePath);

    //���Ұ���hash=DrawIB����һ��
    std::vector<std::wstring> RealTrianglelistIndexList;
    for (std::wstring logLine : LogLineList) {
        if (logLine.find(L"hash=" + OriginalDrawIB) != std::wstring::npos) {
            std::wstring Index = logLine.substr(0, 6);
            RealTrianglelistIndexList.push_back(Index);
        }
    }
    return RealTrianglelistIndexList;
}


std::unordered_map<std::wstring, uint64_t> FrameAnalysisData::Get_Index_CS_CB0_VertexCount_Map_FromCSBufferFiles() {
    //ʡ���ظ�ִ���˷���Դ
    if (this->Index_CS_CB0_VertexCount_Map.size() != 0) {
        return Index_CS_CB0_VertexCount_Map;
    }
    
    std::unordered_map<std::wstring, uint64_t> Tmp_Index_CS_CB0_VertexCount_Map;

    for (std::wstring FileName: this->FrameAnalysisFileNameList) {
        if (FileName.find(L"-cs=") == std::wstring::npos) {
            continue;
        }
        if (FileName.find(L"-cs-cb0=") == std::wstring::npos) {
            continue;
        }
        if (!FileName.ends_with(L".buf")) {
            continue;
        }
        ConstantBufferBufFile CSBufFile(this->WorkFolder + FileName);
        uint64_t VertexCount = CSBufFile.lineCBValueMap[0].X;
       
        std::wstring Index = FileName.substr(0, 6);
        //LOG.Info(L"Index: " + Index + L" VertexCount: " + std::to_wstring(VertexCount) );
        Tmp_Index_CS_CB0_VertexCount_Map[Index] = VertexCount;
    }
    return Tmp_Index_CS_CB0_VertexCount_Map;
}


std::wstring FrameAnalysisData::GetComputeIndexFromCSCB0_ByVertexCount(uint64_t VertexCount) {
    //����cs-cb0��ȡ����cs��Index�Ͷ�������Map
    std::unordered_map<std::wstring, uint64_t> Index_CS_CB0_VertexCount_Map = this->Get_Index_CS_CB0_VertexCount_Map_FromCSBufferFiles();
    //���ݶ������ҵ���Ӧ��Index��Ϊ��ȡ�õ�Index
    std::wstring CS_ExtractIndex = L"";
    for (const auto& pair : Index_CS_CB0_VertexCount_Map) {
        if (VertexCount == pair.second) {
            CS_ExtractIndex = pair.first;
            break;
        }
    }
    return CS_ExtractIndex;
}


std::map<uint64_t, std::wstring> FrameAnalysisData::Get_MatchFirstIndex_IBFileName_Map(std::wstring DrawIB) {
    std::map<uint64_t, std::wstring> firstIndexFileNameMap;
    std::vector<std::wstring> TrianglelistIndexList = this->ReadTrianglelistIndexList(DrawIB);
    for (std::wstring index : TrianglelistIndexList) {
        std::vector<std::wstring> trianglelistIBFileNameList = this->FindFrameAnalysisFileNameListWithCondition(index + L"-ib", L".txt");
        std::wstring trianglelistIBFileName;
        if (trianglelistIBFileNameList.empty()) {
            continue;
        }
        trianglelistIBFileName = trianglelistIBFileNameList[0];
        IndexBufferTxtFile indexBufferFileData(this->WorkFolder + trianglelistIBFileName, false);
        std::wstring firstIndex = indexBufferFileData.FirstIndex;
        firstIndexFileNameMap[std::stoi(firstIndex)] = trianglelistIBFileName;



    }

    for (auto it = firstIndexFileNameMap.begin(); it != firstIndexFileNameMap.end(); ++it) {
        LOG.Info(L"firstIndex: " + std::to_wstring(it->first) + L", trianglelistIBFileName: " + it->second);
    }
    LOG.NewLine();
    return firstIndexFileNameMap;
}

uint64_t FrameAnalysisData::Get_VertexCount_ByAddIBFileUniqueCount(std::wstring DrawIB) {

    std::map<uint64_t, std::wstring> MatchFirstIndex_IBFileName_Map = this->Get_MatchFirstIndex_IBFileName_Map(DrawIB);
    uint64_t TotalVertexCount = 0;

    for (const auto& pair: MatchFirstIndex_IBFileName_Map) {
        std::wstring IBFileName = pair.second;
        IndexBufferTxtFile IBTxtFile(this->WorkFolder + IBFileName, true);
        TotalVertexCount = TotalVertexCount + IBTxtFile.UniqueVertexCount;
    }

    return TotalVertexCount;
}



std::unordered_map<std::string, std::vector<std::byte>> FrameAnalysisData::ReadCategoryBufferFile(std::wstring BufferReadPath, std::string CategoryName, D3D11GameType d3d11GameType) {
    //
    std::vector<std::byte> Buffer;
    std::ifstream BufFile(BufferReadPath, std::ifstream::binary);
    // ��ȡ�ļ���С
    BufFile.seekg(0, std::ios::end);
    std::streamsize fileSize = BufFile.tellg();
    BufFile.seekg(0, std::ios::beg);
    // ���� Buffer �Ĵ�С�������ļ�����
    Buffer.resize(fileSize);
    BufFile.read(reinterpret_cast<char*>(Buffer.data()), fileSize);
    BufFile.close();

    //��ȡ�����Category��ȡ����
    uint64_t CategoryStride = d3d11GameType.CategoryStrideMap[CategoryName];

    //��ȡ������
    uint64_t VertexNumber = fileSize / CategoryStride;
    std::vector<std::string> CategoryElementList = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList, CategoryName);
    
    
    std::unordered_map<std::string, std::vector<std::byte>> ElementName_BufData_Map;
    for (uint64_t i = 0; i < VertexNumber; i++) {
        uint64_t OffsetStart = i * CategoryStride;
        for (std::string ElementName : CategoryElementList) {
            uint64_t ByteWidth = d3d11GameType.ElementNameD3D11ElementMap[ElementName].ByteWidth;
            std::vector<std::byte> ElementData = MMTFormat::GetRange_Byte(Buffer, OffsetStart, OffsetStart + ByteWidth);
            ElementName_BufData_Map[ElementName].insert(ElementName_BufData_Map[ElementName].end(), ElementData.begin(), ElementData.end());
            OffsetStart = OffsetStart + ByteWidth;
        }
    }

    return ElementName_BufData_Map;
}




uint64_t FrameAnalysisData::GetTrianglelistMaxVertexCount(VertexCountType InputVertexCountType, std::wstring DrawIB) {
    uint64_t TranglelistMaxVertexCount = 0;
    std::map<std::wstring, uint64_t> TrianglelistIndexVertexCountMap = this->Get_TrianglelistIndex_VertexCount_Map(InputVertexCountType, DrawIB);

    for (const auto& TrianglelistIndexPair : TrianglelistIndexVertexCountMap) {
        if (TrianglelistIndexPair.second > TranglelistMaxVertexCount) {
            TranglelistMaxVertexCount = TrianglelistIndexPair.second;
        }
    }

    LOG.Info(L"TranglelistMaxVertexCount: " + std::to_wstring(TranglelistMaxVertexCount));
    LOG.NewLine();
    return TranglelistMaxVertexCount;
}

std::wstring FrameAnalysisData::GetTrianglelistExtractIndex(VertexCountType InputVertexCountType, std::wstring DrawIB, D3D11GameType d3d11GameType, bool checkTexcoordSlotExists) {
    std::map<std::wstring, uint64_t> TrianglelistIndexVertexCountMap = this->Get_TrianglelistIndex_VertexCount_Map(InputVertexCountType, DrawIB);
    uint64_t TranglelistMaxVertexCount = GetTrianglelistMaxVertexCount(InputVertexCountType,DrawIB);

    std::wstring TexcoordExtractSlot = MMTString::ToWideString(d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD"].ExtractSlot);
    LOG.Info(L"TexcoordExtractSlot: " + TexcoordExtractSlot);

    std::wstring TrianglelistExtractIndex = L"";
    for (const auto& TrianglelistIndexPair : TrianglelistIndexVertexCountMap) {
        if (TrianglelistIndexPair.second == TranglelistMaxVertexCount) {
            if (checkTexcoordSlotExists) {
                //��������ΪTexcoord����Trianglelist����ȡ������Ҫ�ж����Index����ȡ�ļ��Ƿ���Ĵ��ڲ���
                std::vector<std::wstring> filenames = this->FindFrameAnalysisFileNameListWithCondition(TrianglelistIndexPair.first + L"-" + TexcoordExtractSlot, L".txt");
                if (filenames.size() == 0) {
                    LOG.Info(TrianglelistIndexPair.first + L"'s slot " + TexcoordExtractSlot + L" can't find. skip this.");
                    continue;
                }
                else {
                    TrianglelistExtractIndex = TrianglelistIndexPair.first;
                }
            }
            else {
                TrianglelistExtractIndex = TrianglelistIndexPair.first;
            }
        }
    }
    if (TrianglelistExtractIndex == L"") {
        LOG.Error(MMT_Tips::TIP_CantFindExtractIndex);
    }

    LOG.Info(L"TrianglelistExtractIndex: " + TrianglelistExtractIndex);
    LOG.NewLine();
    return TrianglelistExtractIndex;
}


std::wstring FrameAnalysisData::GetPointlistExtractIndex(VertexCountType InputVertexCountType, uint64_t TrianglelistMaxVertexCount) {
    std::wstring PointlistExtractIndex = L"";
    std::map<std::wstring, uint64_t> PointlistIndexVertexCountMap = this->Get_PointlistIndex_VertexCount_Map(InputVertexCountType);
    for (const auto& pair: PointlistIndexVertexCountMap) {
        if (pair.second == TrianglelistMaxVertexCount) {
            PointlistExtractIndex = pair.first;
            break;
        }
    }
    LOG.Info(L"PointlistExtractIndex: " + PointlistExtractIndex);
    LOG.NewLine();
    return PointlistExtractIndex;
}