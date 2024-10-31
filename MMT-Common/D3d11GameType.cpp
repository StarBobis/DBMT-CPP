#include "D3d11GameType.h"
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"

//����ElementList��������stride
uint64_t D3D11GameType::getElementListStride(std::vector<std::string> inputElementList) {
    uint64_t totalStride = 0;
	for (std::string elementName : inputElementList) {
		std::string elementNameUpper = boost::algorithm::to_upper_copy(elementName);
		D3D11Element elementObject = ElementNameD3D11ElementMap[elementNameUpper];
		totalStride = totalStride + elementObject.ByteWidth;
	}
	return totalStride;
}


D3D11GameType::D3D11GameType() {

}


//����ElementList��������stride
std::unordered_map<std::string, uint64_t>  D3D11GameType::getCategoryStrideMap(std::vector<std::string> inputElementList) {
    std::unordered_map<std::string, uint64_t> CategoryStrideMap;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        uint64_t byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString::ToWideString(elementObject.Category);
        uint64_t categoryStride = CategoryStrideMap[MMTString::ToByteString(elementCategory)];
        if (categoryStride == NULL) {
            categoryStride = byteWidth;
        }
        else {
            categoryStride = categoryStride + byteWidth;
        }
        CategoryStrideMap[MMTString::ToByteString(elementCategory)] = categoryStride;
    }
    return CategoryStrideMap;
}


std::vector<std::string>   D3D11GameType::getCategoryList(std::vector<std::string> inputElementList) {
    std::vector<std::string> CategoryList;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        int byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString::ToWideString(elementObject.Category);
        if (!boost::algorithm::any_of_equal(CategoryList, MMTString::ToByteString(elementCategory))) {
            CategoryList.push_back(MMTString::ToByteString(elementCategory));
        }

    }
    return CategoryList;
}


std::vector<std::string> D3D11GameType::getCategoryElementList(std::vector<std::string> inputElementList, std::string category) {
    std::vector <std::string> positionElementListTmp;
    for (std::string elementName : inputElementList) {
        D3D11Element d3d11ElementTmp = this->ElementNameD3D11ElementMap[elementName];
        if (d3d11ElementTmp.Category == category) {
            positionElementListTmp.push_back(elementName);
        }
    }
    return positionElementListTmp;
}


std::vector<std::string> D3D11GameType::getReorderedElementList(std::vector<std::string> elementList) {
    std::vector<std::string> orderedElementList;
    for (std::string elementName : this->OrderedFullElementList) {
        for (std::string element : elementList) {
            if (boost::algorithm::to_lower_copy(element) == boost::algorithm::to_lower_copy(elementName)) {
                orderedElementList.push_back(elementName);
                break;
            }
        }
    }
    return orderedElementList;
}


void D3D11GameType::ShowElementList(std::vector<std::string> ElementList) {
    LOG.Info("Show ElementList:");
    for (std::string ElementName : ElementList) {
        LOG.Info("ElementName: " + ElementName);
    }
    LOG.NewLine();
}


void D3D11GameType::Show() {
    LOG.Info("GameType: " + this->GameType);
}







D3D11GameTypeLv2::D3D11GameTypeLv2(std::vector<D3D11GameType> D3D11GameTypeList) {
    this->CurrentD3D11GameTypeList = D3D11GameTypeList;

    for (D3D11GameType d3d11GameType: D3D11GameTypeList) {
        this->GameTypeName_D3d11GameType_Map[d3d11GameType.GameType] = d3d11GameType;
    }

    // ��Ϊ���ǿ��ܻ��������ʱ���ٵ���һ�ζ�ȡReverseTypes������ݣ�
    // ��������Ϊ�˱�֤GPU-PreSkinning����ƥ�䣬�Լ���ֹ�ظ�ExtractTypes���ֵ
    // ��Ҫ����մ���
    this->Ordered_GPU_Object_D3D11GameTypeList.clear();

    // �ٷֱ�����
    for (const auto& pair : this->GameTypeName_D3d11GameType_Map) {
        if (pair.second.GPUPreSkinning) {
            this->Ordered_GPU_Object_D3D11GameTypeList.push_back(pair.first);
        }
    }
    for (const auto& pair : this->GameTypeName_D3d11GameType_Map) {
        if (!pair.second.GPUPreSkinning) {
            this->Ordered_GPU_Object_D3D11GameTypeList.push_back(pair.first);
        }
    }

}




std::vector<D3D11GameType> D3D11GameTypeLv2::GetUniqueD3D11GameTypeList() {
    //��ÿһ��GameType����ǰһ��GameType�����жϣ����ÿ��Ԫ�صĴ�С��ȫ��ͬ����Ԫ��Ҳ��ȫ��ͬ
    //����Ϊ���ظ���GameType������ӵ��б�
    std::vector<D3D11GameType> UniqueD3D11GameTypeList;
    for (D3D11GameType d3d11GameType : this->CurrentD3D11GameTypeList) {
        //��һ�����ù�ֱ����Ӽ���
        if (UniqueD3D11GameTypeList.size() == 0) {
            UniqueD3D11GameTypeList.push_back(d3d11GameType);
            continue;
        }

        //
        bool containThisGameType = false;
        for (D3D11GameType uniqueD3D11GameType : UniqueD3D11GameTypeList) {
            //1.�ж�Ԫ���б��Ƿ���ͬ
            std::vector<std::string> vec1 = uniqueD3D11GameType.OrderedFullElementList;
            std::vector<std::string> vec2 = d3d11GameType.OrderedFullElementList;
            bool areEqual1 = (vec1.size() == vec2.size()) && std::equal(vec1.begin(), vec1.end(), vec2.begin());
            if (areEqual1) {
                //Ԫ���б���ͬ���ж�patchBlendWeights�Ƿ���ͬ
                if (d3d11GameType.PatchBLENDWEIGHTS == uniqueD3D11GameType.PatchBLENDWEIGHTS) {
                    //�����Ȼ��ͬ���ж�ÿ��Ԫ�ض�Ӧ��ByteWidth�Ƿ���ͬ

                    bool allElementByteWidthSame = true;
                    for (std::string ElementName : uniqueD3D11GameType.OrderedFullElementList) {
                        D3D11Element element1 = uniqueD3D11GameType.ElementNameD3D11ElementMap[ElementName];
                        D3D11Element element2 = d3d11GameType.ElementNameD3D11ElementMap[ElementName];

                        if (element1.ByteWidth != element2.ByteWidth) {
                            allElementByteWidthSame = false;
                            break;
                        }
                    }

                    if (allElementByteWidthSame) {
                        containThisGameType = true;
                        break;
                    }
                }
            }

        }


        if (!containThisGameType) {
            UniqueD3D11GameTypeList.push_back(d3d11GameType);
        }
    }

    return UniqueD3D11GameTypeList;
}