#pragma once
#include <string>
#include <unordered_map>
#include <set>
#include "D3D11Texture.h"
#include "TextureSlotReplace.h"
#include "DrawIBConfig.h"

void AutoDetectTextureFiles_GI(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning, std::unordered_map<std::string, TextureSlotReplace>& PartName_TextureSlotReplace_Map, std::vector<TextureType> TextureTypeList);
void AutoDetectTextureFiles_HSR(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning, std::unordered_map<std::string, TextureSlotReplace>& PartName_TextureSlotReplace_Map, std::vector<TextureType> TextureTypeList);
void AutoDetectTextureFiles_ZZZ(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning, std::unordered_map<std::string, TextureSlotReplace>& PartName_TextureSlotReplace_Map, std::vector<TextureType> TextureTypeList);
void AutoDetectTextureFilesV2_HI3(DrawIBConfig& drawIBConfig);
void AutoDetectTextureFiles_WW11(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning, std::unordered_map<std::string, TextureSlotReplace>& PartName_TextureSlotReplace_Map, std::vector<TextureType> TextureTypeList);