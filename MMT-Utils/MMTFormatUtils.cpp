#include "MMTStringUtils.h"
#include "MMTFormatUtils.h"

#include <cmath>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>

#include <random>

namespace MMTFormat {

    std::uint32_t STDByteVector_To_UINT32T(std::vector<std::byte> STDByteVector) {
        uint32_t value;
        std::memcpy(&value, STDByteVector.data(), sizeof(uint32_t));
        return value;
    }



    std::uint32_t CharArray_To_UINT32(char* data) {
        return (static_cast<unsigned char>(data[3]) << 24) |
            (static_cast<unsigned char>(data[2]) << 16) |
            (static_cast<unsigned char>(data[1]) << 8) |
            static_cast<unsigned char>(data[0]);
    }

    std::uint16_t CharArray_To_UINT16(char* data) {
        return (static_cast<unsigned char>(data[1]) << 8) |
            static_cast<unsigned char>(data[0]);

    }

    //模仿python里的[i:i+1) 但是不包含i+1的效果，最易于理解
    std::vector<std::byte> GetRange_Byte(const std::vector<std::byte>& vec, std::size_t startIndex, std::size_t endIndex)
    {
        // 起始索引超出范围或大于结束索引，返回空向量
        if (startIndex >= vec.size() || startIndex > endIndex)
        {
            return {};
        }

        // 创建一个新的向量来存储范围内的元素
        std::vector<std::byte> rangeVec(endIndex - startIndex);
        // 复制范围内的元素到新向量中
        std::copy(vec.begin() + startIndex, vec.begin() + endIndex, rangeVec.begin());

        return rangeVec;
    }

    std::vector<uint32_t> GetRange_UINT32(const std::vector<std::uint32_t>& vec, std::size_t startIndex, std::size_t endIndex)
    {
        // 起始索引超出范围或大于结束索引，返回空向量
        if (startIndex >= vec.size() || startIndex > endIndex)
        {
            return {};
        }

        // 创建一个新的向量来存储范围内的元素
        std::vector<std::uint32_t> rangeVec(endIndex - startIndex);
        // 复制范围内的元素到新向量中
        std::copy(vec.begin() + startIndex, vec.begin() + endIndex, rangeVec.begin());

        return rangeVec;
    }



    //用-1.0f乘以当前值再返回，就得到了Flip后的值
    std::vector<std::byte> Reverse32BitFloatValue(std::vector<std::byte> floatValue) {
        //进行Y轴翻转
        std::vector<std::byte> floatValueReversed = floatValue;

        // 读取float值
        float y_value;
        std::memcpy(&y_value, floatValueReversed.data(), sizeof(float));

        // 翻转Y值
        y_value *= -1.0f;

        // 将翻转后的值写回vector
        std::memcpy(floatValueReversed.data(), &y_value, sizeof(float));
        return floatValueReversed;
    }


    std::vector<std::byte> Reverse16BitShortValue(std::vector<std::byte> shortValue) {
        // 进行翻转
        std::vector<std::byte> shortValueReversed = shortValue;

        // 读取 short 值
        short y_value;
        std::memcpy(&y_value, shortValueReversed.data(), sizeof(short));

        // 翻转 Y 值
        y_value *= -1;

        // 将翻转后的值写回 vector
        std::memcpy(shortValueReversed.data(), &y_value, sizeof(short));
        return shortValueReversed;
    }





    std::wstring NearestFormatedFloat(std::wstring originalValue, int reserveCount) {
        //LOG.LogOutput(L"Calculate nearestFormatedFloat");
        std::vector<std::wstring> split_results;
        boost::split(split_results, originalValue, boost::is_any_of(","));
        //LOG.LogOutput(L"Split originalValue over, size: " + std::to_wstring(split_results.size()));
        std::vector<std::wstring> str_results;

        for (std::wstring value : split_results) {
            boost::algorithm::trim(value);
            std::wstring valueDoubleStr;
            if (value == L"-nan(ind)") {
                valueDoubleStr = L"0";
            }
            else {
                //解除这段数字来启用近似值
                double valueDouble = boost::lexical_cast<double>(value);
                if (std::fabs(valueDouble) < 0.0001) {
                    valueDouble = 0.0;
                }

                std::ostringstream out_stream_value;
                out_stream_value << std::fixed << std::setprecision(reserveCount) << valueDouble;
                valueDoubleStr = MMTString::ToWideString(out_stream_value.str());

                if (valueDouble == 0.0) {
                    valueDoubleStr = L"0";
                }
            }


            str_results.push_back(valueDoubleStr);
        }
        std::wstring finalStr = L"";
        for (size_t i = 0; i < str_results.size(); i++)
        {
            std::wstring str = str_results[i];
            if (i != str_results.size() - 1) {
                finalStr = finalStr + str + L"_";
            }
            else {
                finalStr = finalStr + str;
            }
        }

        return finalStr;
    }


    // 计算向量的长度
    double VectorLength(const Vector3D& vector) {
        return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    }


    // 计算向量的单位向量（即归一化向量）
    Vector3D NormalizeVector(const Vector3D vector) {
        double length = VectorLength(vector);
        Vector3D normalizedVector;
        normalizedVector.x = vector.x / length;
        normalizedVector.y = vector.y / length;
        normalizedVector.z = vector.z / length;
        return normalizedVector;
    }


    //相加
    Vector3D AddVectors(const Vector3D vector1, const Vector3D vector2) {
        Vector3D result;
        result.x = vector1.x + vector2.x;
        result.y = vector1.y + vector2.y;
        result.z = vector1.z + vector2.z;
        return result;
    }



    std::wstring FormatedFloat(double originalValue, int reserveCount) {
        std::wstring valueDoubleStr;

        double valueDouble = originalValue;
        if (std::fabs(valueDouble) < 0.0001) {
            valueDouble = 0.0;
        }

        std::ostringstream out_stream_value;
        out_stream_value << std::fixed << std::setprecision(reserveCount) << valueDouble;
        valueDoubleStr = MMTString::ToWideString(out_stream_value.str());

        if (valueDouble == 0.0) {
            valueDoubleStr = L"0";
        }
        return valueDoubleStr;
    }


    //只能0到255
    std::byte PackNumberToByte(std::uint32_t number)
    {
        std::byte packedByte = std::byte(number & 0xFF);
        return packedByte;
    }


    //改进后的方法
    std::vector<std::byte> PackNumberR32_FLOAT_littleIndian(float number)
    {
        std::vector<std::byte> packedBytes(sizeof(float));
        std::memcpy(packedBytes.data(), &number, sizeof(float));
        return packedBytes;
    }


    std::vector<std::vector<double>> NormalNormalizeStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals) {

        // key为position，value为normal
        std::unordered_map<std::wstring, std::wstring> position_normalSum_map;
        std::unordered_map<std::wstring, int> position_normalNumber_map;

        // 相加
        for (int i = 0; i < positions.size(); i++) {
            std::vector<double> position_double = positions[i];
            std::vector<double> normal_double = normals[i];

            //分别转换为wstring
            std::wstring formated_position = FormatedFloat(position_double[0], 10) + L"_" + FormatedFloat(position_double[1], 10) + L"_" + FormatedFloat(position_double[2], 10);
            std::wstring formated_normal = FormatedFloat(normal_double[0], 10) + L"_" + FormatedFloat(normal_double[1], 10) + L"_" + FormatedFloat(normal_double[2], 10);

            if (position_normalSum_map.contains(formated_position)) {
                std::wstring old_formated_normal = position_normalSum_map[formated_position];
                std::vector<std::wstring> split_results;
                boost::split(split_results, old_formated_normal, boost::is_any_of("_"));

                double sum_x = (std::stod(split_results[0]) + normal_double[0]);
                double sum_y = (std::stod(split_results[1]) + normal_double[1]);
                double sum_z = (std::stod(split_results[2]) + normal_double[2]);
                //然后再转换成wstring，放回
                std::wstring result_formated_normal = FormatedFloat(sum_x, 10) + L"_" + FormatedFloat(sum_y, 10) + L"_" + FormatedFloat(sum_z, 10);
                position_normalSum_map[formated_position] = result_formated_normal;
                int number = position_normalNumber_map[formated_position];
                position_normalNumber_map[formated_position] = number + 1;
            }
            else {
                position_normalSum_map[formated_position] = formated_normal;
                position_normalNumber_map[formated_position] = 1;
            }

        }

        std::unordered_map<std::wstring, std::wstring> position_normal_final_map;

        //拿出每一个来，然后做除法，然后从[-1,1]归一化到[0,1] ,放到最终的里面
        for (const auto& pair : position_normalSum_map) {
            std::wstring position = pair.first;
            std::wstring normal_val = pair.second;

            std::vector<std::wstring> split_results;
            boost::split(split_results, normal_val, boost::is_any_of("_"));

            double avg_val_x = std::stod(split_results[0]) / position_normalNumber_map[position];
            double avg_val_y = std::stod(split_results[1]) / position_normalNumber_map[position];
            double avg_val_z = std::stod(split_results[2]) / position_normalNumber_map[position];

            double normalize_val_x = (avg_val_x + 1) / 2;
            double normalize_val_y = (avg_val_y + 1) / 2;
            double normalize_val_z = (avg_val_z + 1) / 2;


            std::wstring result_formated_normal = FormatedFloat(normalize_val_x, 10) + L"_" + FormatedFloat(normalize_val_y, 10) + L"_" + FormatedFloat(normalize_val_z, 10);
            position_normal_final_map[position] = result_formated_normal;
        }

        //这里返回的是每个position对应的tangent
        std::vector<std::vector<double>> tangents;
        //接下来再次遍历
        for (int i = 0; i < positions.size(); i++) {
            std::vector<double> position_double = positions[i];
            //分别转换为wstring
            std::wstring formated_position = FormatedFloat(position_double[0], 10) + L"_" + FormatedFloat(position_double[1], 10) + L"_" + FormatedFloat(position_double[2], 10);
            //然后根据那个map获取对应的tangent信息
            std::wstring formated_normal = position_normal_final_map[formated_position];
            std::vector<std::wstring> split_results;
            boost::split(split_results, formated_normal, boost::is_any_of("_"));
            std::vector<double> OldNormalVector = { std::stod(split_results[0]),std::stod(split_results[1]) ,std::stod(split_results[2]) ,0 };
            tangents.push_back(OldNormalVector);
        }

        return tangents;
    }




    // 计算平均法线，这里只是计算平均法线，其实也就是Normal值，还需要根据Normal值来计算TANGENT才行。
    std::vector<std::vector<double>> CalculateAverageNormalsAndStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals) {


        // key为position，value为normal
        std::unordered_map<std::wstring, std::wstring> position_normal_map;

        //逻辑如下：
        //如果已存在此position的string，则拿出wstring解析，解析后与当前normal相加，结果归一化，再转换为wstring放回去
        // 如果不存在，则直接添加

        for (int i = 0; i < positions.size(); i++) {
            std::vector<double> position_double = positions[i];
            std::vector<double> normal_double = normals[i];

            //分别转换为wstring
            std::wstring formated_position = FormatedFloat(position_double[0], 10) + L"_" + FormatedFloat(position_double[1], 10) + L"_" + FormatedFloat(position_double[2], 10);
            std::wstring formated_normal = FormatedFloat(normal_double[0], 10) + L"_" + FormatedFloat(normal_double[1], 10) + L"_" + FormatedFloat(normal_double[2], 10);

            if (position_normal_map.contains(formated_position)) {
                std::wstring old_formated_normal = position_normal_map[formated_position];
                std::vector<std::wstring> split_results;
                boost::split(split_results, old_formated_normal, boost::is_any_of("_"));

                Vector3D OldNormalVector = { std::stod(split_results[0]),std::stod(split_results[1]) ,std::stod(split_results[2]) };
                Vector3D NowNormalVector = { normal_double[0], normal_double[1], normal_double[2] };
                Vector3D newVector = NormalizeVector(AddVectors(OldNormalVector, NowNormalVector));
                //然后再转换成wstring，放回
                std::wstring result_formated_normal = FormatedFloat(newVector.x, 10) + L"_" + FormatedFloat(newVector.y, 10) + L"_" + FormatedFloat(newVector.z, 10);
                position_normal_map[formated_position] = result_formated_normal;
            }
            else {
                Vector3D NowNormalVector = { normal_double[0], normal_double[1], normal_double[2] };
                Vector3D newVector = NormalizeVector(NowNormalVector);
                //然后再转换成wstring，放回
                std::wstring result_formated_normal = FormatedFloat(newVector.x, 10) + L"_" + FormatedFloat(newVector.y, 10) + L"_" + FormatedFloat(newVector.z, 10);
                position_normal_map[formated_position] = result_formated_normal;
            }

        }
        //LOG.LogOutput(L"Size of position_normal_map: " + std::to_wstring(position_normal_map.size()));

        //这里返回的是每个position对应的tangent
        std::vector<std::vector<double>> tangents;
        //接下来再次遍历
        for (int i = 0; i < positions.size(); i++) {
            std::vector<double> position_double = positions[i];
            //分别转换为wstring
            std::wstring formated_position = FormatedFloat(position_double[0], 10) + L"_" + FormatedFloat(position_double[1], 10) + L"_" + FormatedFloat(position_double[2], 10);
            //然后根据那个map获取对应的tangent信息
            std::wstring formated_normal = position_normal_map[formated_position];
            std::vector<std::wstring> split_results;
            boost::split(split_results, formated_normal, boost::is_any_of("_"));
            std::vector<double> OldNormalVector = { std::stod(split_results[0]),std::stod(split_results[1]) ,std::stod(split_results[2]) ,0 };
            tangents.push_back(OldNormalVector);
        }

        return tangents;
    }




    float ByteVectorToFloat(const std::vector<std::byte>& bytes) {
        // 检查字节数组的长度
        if (bytes.size() != sizeof(float) && bytes.size() != 2) {
            throw std::runtime_error("字节数组的长度与float类型的大小或长度为2不匹配");
        }

        if (bytes.size() == 2) {
            // 将字节数组的数据复制到16位整数类型，考虑小端序
            std::uint16_t value = 0;
            for (std::size_t i = 0; i < 2; ++i) {
                value |= static_cast<std::uint16_t>(static_cast<std::uint8_t>(bytes[i])) << (8 * i);
            }

            // 将整数解释为float类型的值
            float result;
            std::memcpy(&result, &value, 2);
            return result;
        }
        else {
            // 将字节数组的数据复制到32位整数类型，考虑小端序
            std::uint32_t value = 0;
            for (std::size_t i = 0; i < sizeof(float); ++i) {
                value |= static_cast<std::uint32_t>(static_cast<std::uint8_t>(bytes[i])) << (8 * i);
            }

            // 将整数解释为float类型的值
            float result;
            std::memcpy(&result, &value, sizeof(float));
            return result;
        }
    }



    std::wstring NearestFloat(std::wstring originalValue) {
        std::vector<std::wstring> split_results;
        boost::split(split_results, originalValue, boost::is_any_of(","));

        std::vector<std::wstring> str_results;
        for (std::wstring value : split_results) {
            boost::algorithm::trim(value);
            std::wstring valueDoubleStr;
            if (value == L"-nan(ind)") {
                valueDoubleStr = L"0";
            }
            else {
                // //解除这段数字来启用近似值
                //double valueDouble = boost::lexical_cast<double>(value);
                //if (std::fabs(valueDouble) < 0.0001) {
                //    valueDouble = 0.0;
                //}

                //std::ostringstream out_stream_value;
                //out_stream_value << std::fixed << std::setprecision(10) << valueDouble;
                //valueDoubleStr = out_stream_value.str();

                //if (valueDouble == 0.0) {
                //    valueDoubleStr = "0";
                //}
                valueDoubleStr = value;
            }


            str_results.push_back(valueDoubleStr);
        }

        std::wstring finalStr = L"";
        for (size_t i = 0; i < str_results.size(); i++)
        {
            std::wstring str = str_results[i];
            if (i != str_results.size() - 1) {
                finalStr = finalStr + str + L", ";
            }
            else {
                finalStr = finalStr + str;
            }
        }

        return finalStr;
    }


    // 因为我们UE4的NORMAL值是SNORM类型，所以不需要4个长度，只要1个长度，也就是补一个就行了。
    //这个和PackNumberToByte一样，但是算法不一样，到底哪个是正确的？？好像都正确，哈哈哈
    std::vector<std::byte> PackNumberOneByte(int packNumber)
    {
        std::vector<std::byte> packedByte(1);
        packedByte[0] = std::byte(packNumber);  
        // 0x7F 为直接存储数值127，这是因为类型如果是int8，那么1.0f * 127就得到了0x7F，也就是表示1.0f
        // 这里不是0x3F而是0x7F，说明Normal和Tangent都不是原本的Normal和Tangent而是Float类型强制转换为int8类型即 -126-127 之间的值得到的。
        // 转换算法为 float * 127
        // NORMAL 值应该用了某种平均法线算法，距离相近或相同的顶点共享相同法线？
        // TANGENT 应该用了AverageNormal再归一化算法
        return packedByte;
    }

    ////用-1.0f乘以当前值再返回，就得到了Flip后的值
    std::vector<std::byte> ReverseSNORMValue(std::vector<std::byte>& snormValue) {
        // 确认我们正在处理单字节数据
        if (snormValue.size() != 1) {
            return snormValue;
        }

        // 读取snorm值，注意 static_cast 是为了转换 std::byte 到整数
        int8_t value = static_cast<int8_t>(snormValue[0]);

        // 翻转snorm值
        // 直接使用字面量而不是 std::numeric_limits
        if (value == -128) {
            // 特殊情况，-128的相反数不是SNORM表示范围内的值，所以我们使用127
            value = 127;
        }
        else {
            // 取相反数
            value = -value;
        }

        // 存储翻转后的值
        snormValue[0] = static_cast<std::byte>(value);

        return snormValue;
    }


    std::byte ReverseSNORMValueSingle(std::byte Value) {

        // 读取snorm值，注意 static_cast 是为了转换 std::byte 到整数
        int8_t value = static_cast<int8_t>(Value);

        // 翻转snorm值
        // 直接使用字面量而不是 std::numeric_limits
        if (value == -128) {
            // 特殊情况，-128的相反数不是SNORM表示范围内的值，所以我们使用127
            value = 127;
        }
        else {
            // 取相反数
            value = -value;
        }

        return static_cast<std::byte>(value);
    }


    std::byte UnormToSNORMValueSingle(std::byte Value) {
        // UNORM range: [0, 255]
        // SNORM range: [-127, 127]

        // Normalize the UNORM value to the range [0.0, 1.0]
        float fValue = static_cast<float>(static_cast<unsigned int>(Value)) / 255.0f;

        // Convert the normalized value to the SNORM range [-1.0, 1.0]
        float fSNORMValue = fValue * 2.0f - 1.0f;

        // Clamp the SNORM value to the valid range [-1.0, 1.0]
        if (fSNORMValue < -1.0f) {
            fSNORMValue = -1.0f;
        }
        else if (fSNORMValue > 1.0f) {
            fSNORMValue = 1.0f;
        }

        // Convert the SNORM value to a std::byte
        return static_cast<std::byte>(static_cast<int>(fSNORMValue * 127.0f));
    }


    std::vector<std::byte> Int_To_ByteVector(int value) {
        std::vector<std::byte> bytes(4);
        bytes[3] = static_cast<std::byte>((value >> 24) & 0xFF);
        bytes[2] = static_cast<std::byte>((value >> 16) & 0xFF);
        bytes[1] = static_cast<std::byte>((value >> 8) & 0xFF);
        bytes[0] = static_cast<std::byte>(value & 0xFF);
        return bytes;
    }


    int GetRandomNumber(int number) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(0, number - 1);
        return distribution(gen);
    }

    std::vector<std::byte> Get_Default_BLENDWEIGHTS_BufData(uint32_t VertexNumber) {
        std::vector<std::byte> BLENDWEIGHTS_DATA;
        std::vector<std::byte> patchValue = {
                std::byte{0x00}, std::byte{0x00}, std::byte{0x80}, std::byte{0x3f} ,
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00} };

        for (uint32_t i = 0; i < VertexNumber; i++) {
            BLENDWEIGHTS_DATA.insert(BLENDWEIGHTS_DATA.end(), patchValue.begin(), patchValue.end());
        }
        return BLENDWEIGHTS_DATA;
    }



    std::unordered_map<uint64_t, std::vector<std::byte>> PatchBlendBuf_BLENDWEIGHT_1000(std::unordered_map<uint64_t, std::vector<std::byte>> BlendBufMap) {
        std::unordered_map<uint64_t, std::vector<std::byte>> patchedFileBuf;
        //TODO 这里效率是不是低了？到时候如果真影响效率再说吧。
        for (const auto& pair : BlendBufMap) {
            //1,0,0,0
            std::vector<std::byte> patchValue = {
                std::byte{0x00}, std::byte{0x00}, std::byte{0x80}, std::byte{0x3f} ,
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00} };
            patchValue.insert(patchValue.end(), pair.second.begin(), pair.second.end());
            patchedFileBuf[pair.first] = patchValue;
        }
        return patchedFileBuf;
    }


    std::vector<std::byte> Remove_BLENDWEIGHTS(std::vector<std::byte> BlendCategoryData, uint32_t BLENDWEIGHTS_Width, uint32_t BLENDINDICES_Width) {
        std::vector<std::byte> newBlendCategoryData;

        //遍历并只把BLENDINDICES添加到元素列表
        uint32_t blendWeightCount = 1;
        uint32_t blendIndicesCount = 1;
        for (std::byte singleByte : BlendCategoryData) {
            if (blendWeightCount <= BLENDWEIGHTS_Width) {
                blendWeightCount += 1;
            }
            else {
                //然后开始添加到newBlendCategoryData
                if (blendIndicesCount <= BLENDINDICES_Width) {
                    newBlendCategoryData.push_back(singleByte);
                    blendIndicesCount += 1;


                    //放入完之后就需要立刻归零，所以这里判断
                    if ((blendWeightCount == BLENDWEIGHTS_Width + 1) && (blendIndicesCount == BLENDINDICES_Width + 1)) {
                        blendWeightCount = 1;
                        blendIndicesCount = 1;
                    }
                }
                else {
                    blendWeightCount = 1;
                    blendIndicesCount = 1;

                }
            }
        }

        return newBlendCategoryData;

    }



    uint32_t GetByteWidthFromFormat(std::string InputFormatStr) {
        //通过统计数字个数来获取宽度
        std::string formatStr = MMTString::ToLowerCase(InputFormatStr);
        uint32_t totalBytes = 0;
        // 处理通道数
        for (size_t i = 0; i < formatStr.length() - 1; i++) {
            // 根据通道数的类型，假设每个通道都是相同类型
            if (formatStr[i + 1] == '8') { // e.g., r8
                totalBytes += 1;
            }
            else if (formatStr[i + 1] == '3' && formatStr[i + 2] == '2') { // e.g., r32
                totalBytes += 4;
            }
            else if (formatStr[i + 1] == '1' && formatStr[i + 2] == '6') { // e.g., r16
                totalBytes += 2;
            }
        }
        //LOG.Info("Calculate ByteWidth from format: " + formatStr + " Length:" + std::to_string(totalBytes));
        return totalBytes;
    }

}


