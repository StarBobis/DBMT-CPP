#pragma once

#include <string>
#include <unordered_map>

#include "GlobalConfigs.h"
#include "D3d11GameType.h"

#include "MigotoIniBuilder.h"

namespace BufferUtils {

	//д��Hash��ͼini HI3
	void WriteHashStyleTextureIni_HI3(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);
	//д��Hash��ͼini �ɵ�
	std::vector<M_IniSection> GenerateHashStyleTextureIni(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);

	//��Ȩ��Ϣ
	M_IniSection GetCreditInfoIniSection();
	void WriteCreditREADME();

	//��������Դģ��
	void ShareSourceModel(DrawIBConfig IBConfig);

	//COLOR�ؼ��㣬ʹ�û�������ƽ����AverageNormal�ؼ��㷽��
	void Unity_COLOR_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//TANGENTʹ�û���Vector��AverageNormal��һ���ؼ��㷽��
	void Unity_TANGENT_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> &finalVBCategoryDataMap, D3D11GameType d3d11GameType);
	//COLORֱֵ�Ӹ���
	void Unity_Reset_COLOR(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//��תNORMAL��TANGENT��ֵ
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
	//����DrawIndexed�ܹ����µķ���
	

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