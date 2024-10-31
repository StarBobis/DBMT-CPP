#include "Functions_Basic.h"


void Functions_Basic::InitializeGameTypeList(std::unordered_map<std::string, nlohmann::json> GameTypeName_GameTypeJson_Map) {

	std::vector<D3D11GameType> D3D11GameTypeList;

	for (const auto& pair : GameTypeName_GameTypeJson_Map) {
		D3D11GameType d3d11GameType;
		d3d11GameType.GameType = pair.first;
		nlohmann::json GameTypeJson = pair.second;

		if (GameTypeJson.contains("PatchBLENDWEIGHTS")) {
			d3d11GameType.PatchBLENDWEIGHTS = GameTypeJson["PatchBLENDWEIGHTS"];
		}

		//Naraka,AFKJ等UnityCS
		if (GameTypeJson.contains("RootComputeShaderHash")) {
			d3d11GameType.RootComputeShaderHash = GameTypeJson["RootComputeShaderHash"];
		}

		if (GameTypeJson.contains("GPU-PreSkinning")) {
			d3d11GameType.GPUPreSkinning = GameTypeJson["GPU-PreSkinning"];
		}

		//特殊Texcoord Patch
		if (GameTypeJson.contains("TexcoordPatchNull")) {
			d3d11GameType.TexcoordPatchNull = GameTypeJson["TexcoordPatchNull"];
		}

		//UE4PatchNullInBlend
		if (GameTypeJson.contains("UE4PatchNullInBlend")) {
			d3d11GameType.UE4PatchNullInBlend = GameTypeJson["UE4PatchNullInBlend"];
		}

		d3d11GameType.CategoryDrawCategoryMap = GameTypeJson["CategoryDrawCategoryMap"];
		//输出查看测试
		for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
			//LOG.Info("Category:" + pair.first + " DrawCategory:" + pair.second);
		}
		//LOG.NewLine();
		d3d11GameType.OrderedFullElementList = GameTypeJson["OrderedFullElementList"];

		std::vector<nlohmann::json> d3d11JsonList = GameTypeJson["D3D11ElementList"];
		for (nlohmann::json d3d11Json : d3d11JsonList) {
			D3D11Element d3d11Element;
			d3d11Element.SemanticName = d3d11Json["SemanticName"];
			d3d11Element.SemanticIndex = d3d11Json["SemanticIndex"];
			d3d11Element.Format = d3d11Json["Format"];
			d3d11Element.InputSlot = d3d11Json["InputSlot"];
			d3d11Element.InputSlotClass = d3d11Json["InputSlotClass"];
			d3d11Element.InstanceDataStepRate = d3d11Json["InstanceDataStepRate"];
			std::string ByteWidthStr = d3d11Json["ByteWidth"];
			d3d11Element.ByteWidth = std::stoi(ByteWidthStr);
			d3d11Element.ExtractSlot = d3d11Json["ExtractSlot"];
			d3d11Element.ExtractTechnique = d3d11Json["ExtractTechnique"];
			d3d11Element.Category = d3d11Json["Category"];
			std::string ElementName = "";
			if (d3d11Element.SemanticIndex != "0") {
				ElementName = d3d11Element.SemanticName + d3d11Element.SemanticIndex;
			}
			else {
				ElementName = d3d11Element.SemanticName;
			}

			d3d11GameType.ElementNameD3D11ElementMap[ElementName] = d3d11Element;
			d3d11GameType.CategorySlotMap[d3d11Element.Category] = d3d11Element.ExtractSlot;
			d3d11GameType.CategoryTopologyMap[d3d11Element.Category] = d3d11Element.ExtractTechnique;

		}
		d3d11GameType.CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
		d3d11GameType.OrderedCategoryNameList = d3d11GameType.getCategoryList(d3d11GameType.OrderedFullElementList);
		
		D3D11GameTypeList.push_back(d3d11GameType);
	}

	if (this->d3d11GameTypeLv2.CurrentD3D11GameTypeList.size() != 0) {
		std::vector<D3D11GameType> OldD3D11GameTypeList = this->d3d11GameTypeLv2.CurrentD3D11GameTypeList;
		OldD3D11GameTypeList.insert(OldD3D11GameTypeList.end(), D3D11GameTypeList.begin(), D3D11GameTypeList.end());
		D3D11GameTypeLv2 d3d11GameTypeLv2(OldD3D11GameTypeList);
		this->d3d11GameTypeLv2 = d3d11GameTypeLv2;
	}
	else {
		D3D11GameTypeLv2 d3d11GameTypeLv2(D3D11GameTypeList);
		this->d3d11GameTypeLv2 = d3d11GameTypeLv2;
	}

	
}