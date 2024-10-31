#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>


class D3D11Element {
public:
    //可变项
    std::string SemanticName = "";
    std::string SemanticIndex = "";
    std::string Format = "";
    int ByteWidth = 0;
    std::string ExtractSlot = "";
    std::string ExtractTechnique = "";
    std::string Category = "";

    //固定项
    std::string InputSlot = "0";
    std::string InputSlotClass = "per-vertex";
    std::string InstanceDataStepRate = "0";

    //需要动态计算
    int ElementNumber = 0;
    int AlignedByteOffset = 0;

    std::string GetIndexedSemanticName() {
        if (SemanticIndex == "0") {
            return SemanticName;
        }
        else {
            return SemanticName + SemanticIndex;
        }
    }
};


class D3D11GameType {
public:
    std::string GameType;

    //是否在Texcoord槽位Patch 0x00来改变槽位长度来增加Mod制作难度
    int TexcoordPatchNull = 0;

    //是否使用GPU计算骨骼姿态变换
    bool GPUPreSkinning = false;

    //是否需要补充blendweights，默认不需要
    bool PatchBLENDWEIGHTS = false;

    //UE4补充0
    bool UE4PatchNullInBlend = false;

    //UnityCS特殊值
    std::string RootComputeShaderHash = "";

    //原本的类别，在ini文件中替换时写到的draw的位置类别，每一个配置里都需要手动指定
    std::unordered_map <std::string, std::string> CategoryDrawCategoryMap;
    std::vector<std::string> OrderedFullElementList;
    std::vector<std::string> OrderedCategoryNameList;

    //需要后续计算得出的属性
    std::unordered_map<std::string, D3D11Element> ElementNameD3D11ElementMap;
    std::unordered_map <std::string, std::string> CategorySlotMap;
    std::unordered_map <std::string, std::string> CategoryTopologyMap;
    std::unordered_map<std::string, uint64_t> CategoryStrideMap;

    D3D11GameType();

    //根据提供的ElementList获取总的Stride
    uint64_t getElementListStride(std::vector<std::string>);

    //获取Category Stride Map
    std::unordered_map<std::string, uint64_t> getCategoryStrideMap(std::vector<std::string> inputElementList);

    //获取Category List
    std::vector<std::string> getCategoryList(std::vector<std::string> inputElementList);

    //获取Category的ElementList
    std::vector<std::string> getCategoryElementList(std::vector<std::string> inputElementList,std::string category);

    //根据OrderedElementList重新排序传入的ElementList
    std::vector<std::string> getReorderedElementList(std::vector<std::string> elementList);

    //展示
    void ShowElementList(std::vector<std::string> ElementList);

    void Show();
};



//D3D11GameTyleList的包装类，方便对一个游戏的所有d3d11GameType进行快速筛选和操作。
class D3D11GameTypeLv2 {
public:
    std::vector<D3D11GameType> CurrentD3D11GameTypeList;
    std::unordered_map<std::string, D3D11GameType> GameTypeName_D3d11GameType_Map;
    std::vector<std::string> Ordered_GPU_Object_D3D11GameTypeList;

    D3D11GameTypeLv2() {};

    D3D11GameTypeLv2(std::vector<D3D11GameType> D3D11GameTypeList);

    std::vector<D3D11GameType> GetUniqueD3D11GameTypeList();

};