#include "Functions_Basic.h"

#include "MMTLogUtils.h"
#include "MMTConstants.h"
#include "MMTStringUtils.h"

void Functions_Basic::InitializeGameType(DrawIBConfig& In_DrawIBConfig) {
    LOG.Info(L"开始设置自动数据类型：");
    if (In_DrawIBConfig.GameType == L"Auto") {
        In_DrawIBConfig.GameType = AutoGameType(In_DrawIBConfig.DrawIB);
        if (In_DrawIBConfig.GameType == L"") {
            LOG.Error(MMT_Tips::TIP_AutoGameType_NoGameTypeDetected);
        }
    }
    In_DrawIBConfig.WorkGameType = MMTString::ToByteString(In_DrawIBConfig.GameType);
    LOG.Info("Auto GameType Detected:" + In_DrawIBConfig.WorkGameType);
    LOG.NewLine();
}
