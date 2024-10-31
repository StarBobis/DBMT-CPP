#pragma once

#include <string>
#include <unordered_map>

#include "GlobalConfigs.h"
#include "D3d11GameType.h"

#include "MigotoIniBuilder.h"

namespace BufferUtils {

	//写出Hash贴图ini HI3
	void WriteHashStyleTextureIni_HI3(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);
	//写出Hash贴图ini 旧的
	std::vector<M_IniSection> GenerateHashStyleTextureIni(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);

	//版权信息
	M_IniSection GetCreditInfoIniSection();
	void WriteCreditREADME();

	//分享导出的源模型
	void ShareSourceModel(DrawIBConfig IBConfig);

	//COLOR重计算，使用基于算数平均的AverageNormal重计算方法
	void Unity_COLOR_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//TANGENT使用基于Vector的AverageNormal归一化重计算方法
	void Unity_TANGENT_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> &finalVBCategoryDataMap, D3D11GameType d3d11GameType);
	//COLOR值直接赋予
	void Unity_Reset_COLOR(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//翻转NORMAL和TANGENT的值
	void Unity_Reverse_NORMAL_TANGENT(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);

	//Deprecated
	uint64_t GetSum_DrawNumber_FromVBFiles( DrawIBConfig IBConfig);
	//Deprecated
	std::unordered_map<std::wstring, std::vector<std::byte>> Read_FinalVBCategoryDataMap( DrawIBConfig IBConfig);
	void CopySlotDDSTextureFromOutputFolder( DrawIBConfig IBConfig);
	void Read_Convert_Output_IBBufferFiles( DrawIBConfig IBConfig);
	void OutputCategoryBufferFiles( DrawIBConfig IBConfig);



}


namespace BufferUtilsLv2 {
	//基于DrawIndexed架构的新的方法
	

	//
	std::unordered_map<std::string, M_DrawIndexed> Get_PartName_DrawIndexed_Map(DrawIBConfig IBConfig);
	//
	void CombineIBToIndexBufFile(DrawIBConfig IBConfig);



	//
	uint64_t GetSum_DrawNumber_FromVBFiles_Lv2(DrawIBConfig IBConfig);
	//
	std::unordered_map<std::wstring, std::vector<std::byte>> Read_FinalVBCategoryDataMap_Lv2(DrawIBConfig IBConfig);
	//
	void CombineIBToIndexBufFile_Lv2(DrawIBConfig IBConfig);
	//
	std::unordered_map<std::string, M_DrawIndexed> Get_ToggleName_DrawIndexed_Map(DrawIBConfig IBConfig);
	//
	void ShareSourceModel_Lv2(DrawIBConfig IBConfig);
}