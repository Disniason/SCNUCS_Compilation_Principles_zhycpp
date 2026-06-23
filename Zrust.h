#ifndef Z_RUST_H
#define Z_RUST_H
#pragma once

#include <filesystem>//解决中文路径无法打开文件的问题
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>//顺序表
#include <deque>//双端队列
#include <unordered_set>//哈希集合
#include <unordered_map>//哈希映射
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cctype>
#include <stdexcept>//异常处理
#include <functional>

namespace zhy{

    enum class rust_token_id{
        NONEID,

        KW_AS,//严格关键字开始
        KW_BREAK,
        KW_CONST,
        KW_CONTINUE,
        KW_CRATE,
        KW_ELSE,
        KW_ENUM,
        KW_EXTERN,
        KW_FALSE,
        KW_FN,
        KW_FOR,
        KW_IF,
        KW_IMPL,
        KW_IN,
        KW_LET,
        KW_LOOP,
        KW_MATCH,
        KW_MOD,
        KW_MOVE,
        KW_MUT,
        KW_PUB,
        KW_REF,
        KW_RETURN,
        KW_SELFVALUE,
        KW_SELFTYPE,
        KW_STATIC,
        KW_STRUCT,
        KW_SUPER,
        KW_TRAIT,
        KW_TRUE,
        KW_TYPE,
        KW_UNSAFE,
        KW_USE,
        KW_WHERE,
        KW_WHILE,

        KW_ASYNC,
        KW_AWAIT,
        KW_DYN,

        KW_ABSTRACT,//保留关键字开始
        KW_BECOME,
        KW_BOX,
        KW_DO,
        KW_FINAL,
        KW_MACRO,
        KW_OVERRIDE,
        KW_PRIV,
        KW_TYPEOF,
        KW_UNSIZED,
        KW_VIRTUAL,
        KW_YIELD,

        KW_TRY,

        KW_UNION,//弱关键字开始
        KW_STATICLIFETIME,

        TP_U8,//基本数据类型开始
        TP_I8,
        TP_U16,
        TP_I16,
        TP_U32,
        TP_I32,
        TP_U64,
        TP_I64,
        TP_U128,
        TP_I128,
        TP_USIZE,
        TP_ISIZE,
        TP_F32,
        TP_F64,
        TP_BOOL,
        TP_CHAR,
        TP_STR,
        TP_NONE,//不用

        OPERATOR,//操作符开始
        OPERATORASSIGNMENT,//赋值操作符
        SEPARATOR,//分隔符

        CHAR,//字面量开始
        STRING,
        STRINGRAW,
        BYTECHAR,
        BYTESTRING,
        BYTESTRINGRAW,

        /*SEVEN,
        EIGHT,
        LINEFEED,//换行符
        RETURN,//回车符
        TABLE,//制表符
        BACKSLASH,//反斜线
        NULLPTR,//Null

        TWENTYFOUR,

        QUOTATIONSINGLE,
        QUOTATIONDOUBLE,*/

        INTEGERD,//10进制
        INTEGERH,//16进制
        INTEGERO,//8进制
        INTEGERB,//2进制
        FLOAT,

        SUFFIX,//后缀

        COMMENTLINE,//注释开始
        COMMENTBLOCK,
        COMMENTITEM,//外部文档注释，项之前
        COMMENTDOCUMENT,//内部文档注释，文件顶部

        ID,//标识符开始

        MACRONAME//宏调用名，规定: 标识符 + '!' = 宏调用名

    };

    inline const std::unordered_map <std::string,zhy::rust_token_id> rust_kw_map = {
        {"as",zhy::rust_token_id::KW_AS},
        {"break",zhy::rust_token_id::KW_BREAK},
        {"const",zhy::rust_token_id::KW_CONST},
        {"continue",zhy::rust_token_id::KW_CONTINUE},
        {"crate",zhy::rust_token_id::KW_CRATE},
        {"else",zhy::rust_token_id::KW_ELSE},
        {"enum",zhy::rust_token_id::KW_ENUM},
        {"extern",zhy::rust_token_id::KW_EXTERN},
        {"false",zhy::rust_token_id::KW_FALSE},
        {"fn",zhy::rust_token_id::KW_FN},
        {"for",zhy::rust_token_id::KW_FOR},
        {"if",zhy::rust_token_id::KW_IF},
        {"impl",zhy::rust_token_id::KW_IMPL},
        {"in",zhy::rust_token_id::KW_IN},
        {"let",zhy::rust_token_id::KW_LET},
        {"loop",zhy::rust_token_id::KW_LOOP},
        {"match",zhy::rust_token_id::KW_MATCH},
        {"mod",zhy::rust_token_id::KW_MOD},
        {"move",zhy::rust_token_id::KW_MOVE},
        {"mut",zhy::rust_token_id::KW_MUT},
        {"pub",zhy::rust_token_id::KW_PUB},
        {"ref",zhy::rust_token_id::KW_REF},
        {"return",zhy::rust_token_id::KW_RETURN},
        {"self",zhy::rust_token_id::KW_SELFVALUE},
        {"Self",zhy::rust_token_id::KW_SELFTYPE},
        {"static",zhy::rust_token_id::KW_STATIC},
        {"struct",zhy::rust_token_id::KW_STRUCT},
        {"super",zhy::rust_token_id::KW_SUPER},
        {"trait",zhy::rust_token_id::KW_TRAIT},
        {"true",zhy::rust_token_id::KW_TRUE},
        {"type",zhy::rust_token_id::KW_TYPE},
        {"unsafe",zhy::rust_token_id::KW_UNSAFE},
        {"use",zhy::rust_token_id::KW_USE},
        {"where",zhy::rust_token_id::KW_WHERE},
        {"while",zhy::rust_token_id::KW_WHILE},

        {"async",zhy::rust_token_id::KW_ASYNC},
        {"await",zhy::rust_token_id::KW_AWAIT},
        {"dyn",zhy::rust_token_id::KW_DYN},

        {"abstract",zhy::rust_token_id::KW_ABSTRACT},
        {"become",zhy::rust_token_id::KW_BECOME},
        {"box",zhy::rust_token_id::KW_BOX},
        {"do",zhy::rust_token_id::KW_DO},
        {"final",zhy::rust_token_id::KW_FINAL},
        {"macro",zhy::rust_token_id::KW_MACRO},
        {"override",zhy::rust_token_id::KW_OVERRIDE},
        {"priv",zhy::rust_token_id::KW_PRIV},
        {"typeof",zhy::rust_token_id::KW_TYPEOF},
        {"unsized",zhy::rust_token_id::KW_UNSIZED},
        {"virtual",zhy::rust_token_id::KW_VIRTUAL},
        {"yield",zhy::rust_token_id::KW_YIELD},

        {"try",zhy::rust_token_id::KW_TRY},

        {"union",zhy::rust_token_id::KW_UNION},
        {"\'static",zhy::rust_token_id::KW_STATICLIFETIME},

        {"u8",zhy::rust_token_id::TP_U8},
        {"i8",zhy::rust_token_id::TP_I8},
        {"u16",zhy::rust_token_id::TP_U16},
        {"i16",zhy::rust_token_id::TP_I16},
        {"u32",zhy::rust_token_id::TP_U32},
        {"i32",zhy::rust_token_id::TP_I32},
        {"u64",zhy::rust_token_id::TP_U64},
        {"i64",zhy::rust_token_id::TP_I64},
        {"u128",zhy::rust_token_id::TP_U128},
        {"i128",zhy::rust_token_id::TP_I128},
        {"usize",zhy::rust_token_id::TP_USIZE},
        {"isize",zhy::rust_token_id::TP_ISIZE},
        {"f32",zhy::rust_token_id::TP_F32},
        {"f64",zhy::rust_token_id::TP_F64},
        {"bool",zhy::rust_token_id::TP_BOOL},
        {"char",zhy::rust_token_id::TP_CHAR},
        {"str",zhy::rust_token_id::TP_STR},
        //{"()",zhy::rust_token_id::TP_NONE},

        {"**",zhy::rust_token_id::OPERATOR},
        {"!=",zhy::rust_token_id::OPERATOR},
        {"==",zhy::rust_token_id::OPERATOR},
        {"<=",zhy::rust_token_id::OPERATOR},
        {">=",zhy::rust_token_id::OPERATOR},
        {"<<",zhy::rust_token_id::OPERATOR},
        {">>",zhy::rust_token_id::OPERATOR},
        {"&&",zhy::rust_token_id::OPERATOR},
        {"||",zhy::rust_token_id::OPERATOR},
        {"->",zhy::rust_token_id::OPERATOR},
        {"=>",zhy::rust_token_id::OPERATOR},
        {"..",zhy::rust_token_id::OPERATOR},
        {"..=",zhy::rust_token_id::OPERATOR},
        {"::",zhy::rust_token_id::OPERATOR},
        {"+",zhy::rust_token_id::OPERATOR},
        {"-",zhy::rust_token_id::OPERATOR},
        {"*",zhy::rust_token_id::OPERATOR},
        {"/",zhy::rust_token_id::OPERATOR},
        {"%",zhy::rust_token_id::OPERATOR},
        {"!",zhy::rust_token_id::OPERATOR},
        {"&",zhy::rust_token_id::OPERATOR},
        {"|",zhy::rust_token_id::OPERATOR},
        {"^",zhy::rust_token_id::OPERATOR},
        {"<",zhy::rust_token_id::OPERATOR},
        {">",zhy::rust_token_id::OPERATOR},
        {"?",zhy::rust_token_id::OPERATOR},
        {"=",zhy::rust_token_id::OPERATOR},//有争议
        //{"~",zhy::rust_token_id::OPERATOR},
        
        {"+=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"-=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"*=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"/=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"%=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"&=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"|=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"^=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {"<<=",zhy::rust_token_id::OPERATORASSIGNMENT},
        {">>=",zhy::rust_token_id::OPERATORASSIGNMENT},
        
        {"(",zhy::rust_token_id::SEPARATOR},
        {")",zhy::rust_token_id::SEPARATOR},
        {"[",zhy::rust_token_id::SEPARATOR},
        {"]",zhy::rust_token_id::SEPARATOR},
        {"{",zhy::rust_token_id::SEPARATOR},
        {"}",zhy::rust_token_id::SEPARATOR},
        {",",zhy::rust_token_id::SEPARATOR},
        {".",zhy::rust_token_id::SEPARATOR},
        {";",zhy::rust_token_id::SEPARATOR},
        {":",zhy::rust_token_id::SEPARATOR},
        {"'",zhy::rust_token_id::SEPARATOR},
        {"\"",zhy::rust_token_id::SEPARATOR},
        {"#",zhy::rust_token_id::SEPARATOR},
        //{"_",zhy::rust_token_id::SEPARATOR},

        {"//",zhy::rust_token_id::COMMENTLINE},
        {"/*",zhy::rust_token_id::COMMENTBLOCK},
        {"///",zhy::rust_token_id::COMMENTITEM},
        {"//!",zhy::rust_token_id::COMMENTDOCUMENT}

    };

    inline const std::unordered_set <char> rust_literal_start = {
        '\'',
        '\"',
        'r',
        'b',
        '0','1','2','3','4','5','6','7','8','9'
        //注意：正负号是一个单独的操作符token
    };

    enum class rust_suffix{
        nonesuffix,

        suffix,
        u8,
        i8,
        u16,
        i16,
        u32,
        i32,
        u64,
        i64,
        u128,
        i128,
        usize,
        isize,
        f32,
        f64
    };

    inline const std::unordered_map <std::string,zhy::rust_suffix> rust_suffix_map = {
        {"suffix",zhy::rust_suffix::suffix},
        {"u8",zhy::rust_suffix::u8},
        {"i8",zhy::rust_suffix::i8},
        {"u16",zhy::rust_suffix::u16},
        {"i16",zhy::rust_suffix::i16},
        {"u32",zhy::rust_suffix::u32},
        {"i32",zhy::rust_suffix::i32},
        {"u64",zhy::rust_suffix::u64},
        {"i64",zhy::rust_suffix::i64},
        {"u128",zhy::rust_suffix::u128},
        {"i128",zhy::rust_suffix::i128},
        {"usize",zhy::rust_suffix::usize},
        {"isize",zhy::rust_suffix::isize},
        {"f32",zhy::rust_suffix::f32},
        {"f64",zhy::rust_suffix::f64}
    };

    inline const std::unordered_map <zhy::rust_suffix,std::string> rust_suffix_string_map = {
        {zhy::rust_suffix::suffix,"suffix"},
        {zhy::rust_suffix::u8,"u8"},
        {zhy::rust_suffix::i8,"i8"},
        {zhy::rust_suffix::u16,"u16"},
        {zhy::rust_suffix::i16,"i16"},
        {zhy::rust_suffix::u32,"u32"},
        {zhy::rust_suffix::i32,"i32"},
        {zhy::rust_suffix::u64,"u64"},
        {zhy::rust_suffix::i64,"i64"},
        {zhy::rust_suffix::u128,"u128"},
        {zhy::rust_suffix::i128,"i128"},
        {zhy::rust_suffix::usize,"usize"},
        {zhy::rust_suffix::isize,"isize"},
        {zhy::rust_suffix::f32,"f32"},
        {zhy::rust_suffix::f64,"f64"}
    };

    class rust_token{
        public:
        zhy::rust_token_id tokenId;
        long long tokenInteger;//存储整数
        size_t tokenUnsignedInteger;//存储无符号整数
        long double tokenFloat;//存储浮点数
        char tokenCharacter;//存储字符/字节/转义字符
        std::string tokenString;//存储字符串
        std::string tokenOperator;//存储运算符
        std::string tokenWord;//存储token字符串，以及标识符和关键字本身
        zhy::rust_suffix tokenSuffix;//存储后缀

        rust_token(){
            tokenId = zhy::rust_token_id::NONEID;
            tokenInteger = 0;
            tokenUnsignedInteger = 0;
            tokenFloat = 0.0;
            tokenCharacter = '\0';
            tokenString = "";
            tokenOperator = "";
            tokenWord = "";
            tokenSuffix = zhy::rust_suffix::nonesuffix;
        }
        rust_token(const zhy::rust_token & other){
            tokenId = other.tokenId;
            tokenInteger = other.tokenInteger;
            tokenUnsignedInteger = other.tokenUnsignedInteger;
            tokenFloat = other.tokenFloat;
            tokenCharacter = other.tokenCharacter;
            tokenString = other.tokenString;
            tokenOperator = other.tokenOperator;
            tokenWord = other.tokenWord;
            tokenSuffix = other.tokenSuffix;
        }
        ~rust_token(){}
        rust_token & operator =(const zhy::rust_token & other){
            if (this != &other){
                tokenId = other.tokenId;
                tokenInteger = other.tokenInteger;
                tokenUnsignedInteger = other.tokenUnsignedInteger;
                tokenFloat = other.tokenFloat;
                tokenCharacter = other.tokenCharacter;
                tokenString = other.tokenString;
                tokenOperator = other.tokenOperator;
                tokenWord = other.tokenWord;
                tokenSuffix = other.tokenSuffix;
            }
            return *this;
        }

        inline static bool isRustKeyWord(const std::string & word = ""){//判断关键字
            return zhy::rust_kw_map.find(word) != zhy::rust_kw_map.end();
        }

        inline static bool isRustSuffix(const std::string & word = ""){//判断字面量后缀
            return zhy::rust_suffix_map.find(word) != zhy::rust_suffix_map.end();
        }

        inline static bool isCommentStart(const std::string & word = ""){//判断注释起始符
            return {
                word == "//" ||
                word == "/*" ||
                word == "///" ||
                word == "//!"
            };
        }

        inline static bool isXIdStart(char input = ' '){//判断标识符起始符
            return std::isalpha(static_cast <unsigned char>(input)) || input == '_';
        }

        inline static bool isXIdContinue(char input = ' '){//判断标识符中间符
            return std::isalnum(static_cast <unsigned char>(input)) || input == '_';
        }

        inline static bool isWhiteSpaceChar(char input = ' '){//判断空白符
            return (
                std::isspace(static_cast <unsigned char>(input)) ||
                input == ' ' ||
                input == '\t' ||
                input == '\n' ||
                input == '\v' ||
                input == '\f' ||
                input == '\r'
            );
        }

        inline static void codepointToUtf8(uint32_t codepoint,std::string & utf8_str){
            utf8_str = "";
            if (codepoint <= 0x7F) utf8_str.push_back(static_cast <char>(codepoint));
            else if (codepoint <= 0x7FF){
                utf8_str.push_back(static_cast <char>(0xC0 | ((codepoint >> 6) & 0x1F)));
                utf8_str.push_back(static_cast <char>(0x80 | (codepoint & 0x3F)));
            }
            else if (codepoint <= 0xFFFF){
                utf8_str.push_back(static_cast <char>(0xE0 | ((codepoint >> 12) & 0x0F)));
                utf8_str.push_back(static_cast <char>(0x80 | ((codepoint >> 6) & 0x3F)));
                utf8_str.push_back(static_cast <char>(0x80 | (codepoint & 0x3F)));
            }
            else if (codepoint <= 0x10FFFF){
                utf8_str.push_back(static_cast <char>(0xF0 | ((codepoint >> 18) & 0x07)));
                utf8_str.push_back(static_cast <char>(0x80 | ((codepoint >> 12) & 0x3F)));
                utf8_str.push_back(static_cast <char>(0x80 | ((codepoint >> 6) & 0x3F)));
                utf8_str.push_back(static_cast <char>(0x80 | (codepoint & 0x3F)));
            }
            else throw std::invalid_argument("Unicode 码点超出合法范围(0~0x10FFFF)");
            return;
        }

        inline static void handleEscapeChar(std::string & input){//处理字符串中的转义字符
            for(size_t i=0;i<input.size();i++){
                if (input[i] == '\\'){
                    std::string slice2 = "";
                    if (input.size() >= (i+2)) slice2 = input.substr(i,2);
                    if (slice2 == "\\u"){//处理24-bit Unicode 字符编码
                        if (input.size() >= (i+4) && input[i+2] == '{'){
                            size_t start = i + 3;
                            size_t end = input.find_first_of('}',start);
                            if (end == std::string::npos) continue;
                            std::string hex_str = input.substr(start, end - start);
                            hex_str.erase(std::remove(hex_str.begin(),hex_str.end(),'_'),hex_str.end());
                            if (hex_str.empty() || hex_str.size() > 6){
                                i = end;
                                continue;
                            }
                            uint32_t codepoint = 0;
                            try{
                                codepoint = std::stoul(hex_str,nullptr,16);
                            }
                            catch (const std::exception & e){
                                i = end;
                                continue;
                            }
                            if (codepoint > 0x10FFFF){
                                i = end;
                                continue;
                            }
                            try{
                                std::string utf8_char = "";
                                zhy::rust_token::codepointToUtf8(codepoint,utf8_char);
                                input.erase(i,(end + 1 - i));
                                input.insert(i,utf8_char);
                                i += utf8_char.size() - 1;
                                continue;
                            }
                            catch (const std::exception & e){
                                i = end;
                                continue;
                            }
                        }
                        else continue;
                    }
                    else if (slice2 == "\\x"){//处理7或8-bit 字符编码
                        if (input.size() >= (i+4)){
                            char escape = static_cast <char>(std::stoi(input.substr(i+2,2),nullptr,16));
                            input.erase(i,4);
                            input.insert(i,1,escape);
                        }
                    }
                    else if (slice2 == "\\n"){
                        input.erase(i,2);
                        input.insert(i,1,'\n');
                    }
                    else if (slice2 == "\\r"){
                        input.erase(i,2);
                        input.insert(i,1,'\r');
                    }
                    else if (slice2 == "\\t"){
                        input.erase(i,2);
                        input.insert(i,1,'\t');
                    }
                    else if (slice2 == "\\\\"){
                        input.erase(i,2);
                        input.insert(i,1,'\\');
                    }
                    else if (slice2 == "\\0"){
                        input.erase(i,2);
                        input.insert(i,1,'\0');
                    }
                    else if (slice2 == "\\'"){
                        input.erase(i,2);
                        input.insert(i,1,'\'');
                    }
                    else if (slice2 == "\\\""){
                        input.erase(i,2);
                        input.insert(i,1,'\"');
                    }
                    else continue;
                }
            }
            return;
        }

        inline static char returnhandledEscapeChar(const std::string & input){//处理字符(串)中的转义字符，专门针对字节字面量使用
            std::string slice2 = "";
            if (input.size() >= 2) slice2 = input.substr(0,2);
            if (slice2 == "\\x"){
                if (input.size() >= 4) return static_cast <char>(std::stoi(input.substr(2,2),nullptr,16));
            }
            else if (slice2 == "\\n") return '\n';
            else if (slice2 == "\\r") return '\r';
            else if (slice2 == "\\t") return '\t';
            else if (slice2 == "\\\\") return '\\';
            else if (slice2 == "\\0") return '\0';
            else if (slice2 == "\\'") return '\'';
            else if (slice2 == "\\\"") return '\"';
            return input[0];
        }

        inline static bool isEscapeChar(char input = ' '){
            switch (input){
                case '\n':return true;
                case '\r':return true;
                case '\t':return true;
                case '\\':return true;
                case '\0':return true;
                case '\'':return true;
                case '\"':return true;
                default:return false;
            }
            return false;
        }

        inline static bool isSymbol(char input = ' '){//判断标点和操作符
            unsigned char uc = static_cast<unsigned char>(input);
            return std::isprint(uc) && !std::isalnum(uc) && !std::isspace(uc) && input != '_';
        }

        inline static bool isLiteralStart(char input = ' '){//判断字面量起始符
            return zhy::rust_literal_start.find(input) != zhy::rust_literal_start.end();
        }

        inline static bool isHexadecimalChar(char input = ' '){//判断是否为16进制字符
            return std::isdigit(static_cast <unsigned char>(input)) || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F');
        }

        inline static long long hexadecimalToLL(const std::string & str){//16进制转十进制ll
            long long val = 0;
            std::istringstream iss(str);
            iss>>std::hex>>val;
            return val;
        }

        inline static size_t hexadecimalToULL(const std::string & str){//16进制转十进制ull
            size_t val = 0;
            std::istringstream iss(str);
            iss>>std::hex>>val;
            return val;
        }

        inline static bool isOctalChar(char input = ' '){//判断是否为8进制字符
            return input >= '0' && input <= '7';
        }

        inline static long double octalToLD(const std::string & str){//8进制浮点数转long double
            size_t dot = str.find('.');
            if (dot == std::string::npos) dot = str.size();
            long double val = 0;
            for(size_t i=0;i<dot;i++){
                val = val * 8 + (str[i] - '0');
            }
            long double dec = 1 / 8;
            for(size_t i=(dot + 1);i<str.size();i++){
                val += dec * (str[i] - '0');
                dec /= 8;
            }
            return val;
        }

        inline static bool isBinaryChar(char input = ' '){//判断是否为2进制字符
            return input == '0' || input == '1';
        }

        inline static long double binaryToLD(const std::string & str){//2进制浮点数转long double
            size_t dot = str.find('.');
            if (dot == std::string::npos) dot = str.size();
            long double val = 0;
            for(size_t i=0;i<dot;i++){
                val = val * 2 + (str[i] - '0');
            }
            long double dec = 1 / 2;
            for(size_t i=(dot + 1);i<str.size();i++){
                val += dec * (str[i] - '0');
                dec /= 2;
            }
            return val;
        }

        inline static bool isFloatContinue(char input = ' '){//判断是否为浮点数字符
            return std::isdigit(static_cast <unsigned char>(input)) || input == 'e' || input == 'E' || input == '.';
        }

        inline static bool isExponentStart(char input = ' '){
            return(
                std::isdigit(static_cast <unsigned char>(input)) ||
                input == '+' ||
                input == '-'
            );
        }

        inline static bool isDecimalContinue(char input = ' '){//不包括小数点、e和后缀
            return !(
                zhy::rust_token::isWhiteSpaceChar(input) ||
                zhy::rust_token::isSymbol(input)
            );
        }
        
        inline static size_t findNotEscape(const std::string & str,char input = ' ',size_t start = 0){
            if (!zhy::rust_token::isEscapeChar(input)) return str.find_first_of(input,start);
            for(size_t i=start;i<str.size();i++){
                if (str[i] == input){
                    if (i == start) return i;
                    else if (str[i-1] != '\\') return i;
                    else if (str[i-1] == '\\'){//处理符号之前有连续多个反斜杠
                        size_t slashNum = 1;
                        for(size_t j=(i-2);j>=start;j--){
                            if (str[j] == '\\') slashNum += 1;
                            else break;
                        }
                        if (slashNum % 2 == 0) return i;
                        else continue;
                    }
                    else continue;
                }
            }
            return std::string::npos;
        }

        inline static size_t findNotEscape(const std::string & str,const std::string & input = "",size_t start = 0){
            if (input == "") return std::string::npos;
            if (!zhy::rust_token::isEscapeChar(input[0])) return str.find(input,start);
            size_t pos = start;
            while ((pos = str.find(input,pos)) != std::string::npos){
                pos = str.find(input,start);
                if (pos == start || str[pos-1] != '\\') return pos;
                else if (str[pos-1] == '\\'){
                    if ((pos-1) == start){
                        pos += 1;
                        continue;
                    }
                    else if ((pos-2) >= start && str[pos-2] == '\\') return pos;
                    else{
                        pos += 1;
                        continue;
                    }
                }
                else{
                    pos += 1;
                    continue;
                }
            }
            return std::string::npos;
        }

        void displayToken(std::ostream & out = std::cout) const{
            if (tokenId == zhy::rust_token_id::NONEID) out<<tokenWord<<" : 未知的token"<<std::endl;
            else if (tokenId >= zhy::rust_token_id::KW_AS && tokenId <= zhy::rust_token_id::KW_DYN) out<<tokenWord<<" : 严格关键字"<<std::endl;
            else if (tokenId >= zhy::rust_token_id::KW_ABSTRACT && tokenId <= zhy::rust_token_id::KW_TRY) out<<tokenWord<<" : 保留关键字"<<std::endl;
            else if (tokenId >= zhy::rust_token_id::KW_UNION && tokenId <= zhy::rust_token_id::KW_STATICLIFETIME) out<<tokenWord<<" : 弱关键字"<<std::endl;
            else if (tokenId >= zhy::rust_token_id::TP_U8 && tokenId <= zhy::rust_token_id::TP_NONE) out<<tokenWord<<" : 基本类型说明符"<<std::endl;
            else if (tokenId >= zhy::rust_token_id::OPERATOR && tokenId <= zhy::rust_token_id::SEPARATOR){
                if (tokenId == zhy::rust_token_id::OPERATOR) out<<tokenWord<<" : 操作符"<<std::endl;
                else if (tokenId == zhy::rust_token_id::OPERATORASSIGNMENT) out<<tokenWord<<" : 赋值操作符"<<std::endl;
                else if (tokenId == zhy::rust_token_id::SEPARATOR) out<<tokenWord<<" : 分隔符"<<std::endl;
                else out<<tokenWord<<" : 未知的符号"<<std::endl;
            }
            else if (tokenId >= zhy::rust_token_id::CHAR && tokenId <= zhy::rust_token_id::SUFFIX){
                if (tokenId == zhy::rust_token_id::CHAR) out<<tokenWord<<" : 字面量(字符)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::STRING) out<<tokenWord<<" : 字面量(字符串)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::STRINGRAW) out<<tokenWord<<" : 字面量(原生字符串)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::BYTECHAR) out<<tokenWord<<" : 字面量(字节)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::BYTESTRING) out<<tokenWord<<" : 字面量(字节串)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::BYTESTRINGRAW) out<<tokenWord<<" : 字面量(原生字节串)"<<std::endl;
                /*else if (tokenId == zhy::rust_token_id::SEVEN) out<<tokenWord<<" : 字面量(7-bit 字符编码)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::EIGHT) out<<tokenWord<<" : 字面量(8-bit 字符编码)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::LINEFEED) out<<tokenWord<<" : 字面量(换行符)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::RETURN) out<<tokenWord<<" : 字面量(回车符)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::TABLE) out<<tokenWord<<" : 字面量(制表符)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::BACKSLASH) out<<tokenWord<<" : 字面量(反斜线)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::NULLPTR) out<<tokenWord<<" : 字面量(Null)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::TWENTYFOUR) out<<tokenWord<<" : 字面量(24-bit Unicode字符编码)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::QUOTATIONSINGLE) out<<tokenWord<<" : 字面量(单引号)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::QUOTATIONDOUBLE) out<<tokenWord<<" : 字面量(双引号)"<<std::endl;*/
                else if (tokenId == zhy::rust_token_id::INTEGERD) out<<tokenWord<<" : 字面量(十进制整数)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::INTEGERH) out<<tokenWord<<" : 字面量(十六进制整数)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::INTEGERO) out<<tokenWord<<" : 字面量(八进制整数)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::INTEGERB) out<<tokenWord<<" : 字面量(二进制整数)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::FLOAT) out<<tokenWord<<" : 字面量(浮点数)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::SUFFIX) out<<tokenWord<<" : 字面量(后缀)"<<std::endl;
                else out<<tokenWord<<" : 字面量(未知字面量)"<<std::endl;
            }
            else if (tokenId >= zhy::rust_token_id::COMMENTLINE && tokenId <= zhy::rust_token_id::COMMENTDOCUMENT){
                if (tokenId == zhy::rust_token_id::COMMENTLINE) out<<tokenWord<<" : 注释(行注释)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::COMMENTBLOCK) out<<tokenWord<<" : 注释(块注释)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::COMMENTITEM) out<<tokenWord<<" : 注释(外部文档注释)"<<std::endl;
                else if (tokenId == zhy::rust_token_id::COMMENTDOCUMENT) out<<tokenWord<<" : 注释(内部文档注释)"<<std::endl;
                else out<<tokenWord<<" : 注释(未知型注释)"<<std::endl;
            }
            else if (tokenId == zhy::rust_token_id::ID) out<<tokenWord<<" : 标识符"<<std::endl;
            else if (tokenId == zhy::rust_token_id::MACRONAME) out<<tokenWord<<" : 宏调用名"<<std::endl;
            else out<<tokenWord<<" : 未知的token"<<std::endl;
            return;
        }

        void clear(){
            tokenId = zhy::rust_token_id::NONEID;
            tokenInteger = 0;
            tokenUnsignedInteger = 0;
            tokenFloat = 0.0;
            tokenCharacter = '\0';
            tokenString = "";
            tokenOperator = "";
            tokenWord = "";
            tokenSuffix = zhy::rust_suffix::nonesuffix;
            return;
        }
    };

    inline bool operator ==(const zhy::rust_token & a,const zhy::rust_token & b){
        return (
            a.tokenId == b.tokenId &&
            a.tokenInteger == b.tokenInteger &&
            a.tokenUnsignedInteger == b.tokenUnsignedInteger &&
            a.tokenFloat == b.tokenFloat &&
            a.tokenCharacter == b.tokenCharacter &&
            a.tokenString == b.tokenString &&
            a.tokenOperator == b.tokenOperator &&
            a.tokenWord == b.tokenWord &&
            a.tokenSuffix == b.tokenSuffix
        );
    }

    class rust_lexer{
        private:
        rust_lexer() = delete;//纯静态类(即命名空间类)
        ~rust_lexer() = delete;
        rust_lexer(const zhy::rust_lexer & other) = delete;
        zhy::rust_lexer & operator =(const zhy::rust_lexer & other) = delete;
        public:

        inline static void trimLine(std::string & line){//去除行首尾空字符
            line.erase(0,line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            return;
        }

        template <template <typename> class container>
        inline static bool getLineToken(//词法分析核心函数，按行读取
            std::istream & in,//输入流引用，可适配标准输入流、文件输入流、字符串输入流
            container <zhy::rust_token> & tokens,//token数组或token队列
            long long & commentNum,//段注释起始符/*的数量
            zhy::rust_token & lastLineCommentToken,//上一行最后一个token，一般为段注释
            std::ostream & out = std::cout//输出流引用，可适配标准输出流、文件输出流、字符串输出流
        ){
            std::string line;
            if (!std::getline(in,line)) return false;//直接输入一整行
            zhy::rust_lexer::trimLine(line);
            if (line == "") return true;
            out<<std::endl<<line<<std::endl;
            size_t scanStart = 0;//扫描起始位置
            if (commentNum > 0){//处理如果本行的起始部分为上一行的注释的情况
                size_t end = std::string::npos;
                for(size_t i=scanStart;i<(line.size()-1);i++){
                    if (line.substr(i,2) == "/*") commentNum += 1;
                    else if (line.substr(i,2) == "*/") commentNum -= 1;
                    if (commentNum <= 0){
                        end = i;
                        break;
                    }
                }
                if (end == std::string::npos){//本行一整行都是上一行的注释
                    lastLineCommentToken.tokenWord += line.substr(0,line.size());
                    lastLineCommentToken.tokenId = zhy::rust_token_id::COMMENTBLOCK;
                    tokens.push_back(lastLineCommentToken);
                    return true;
                }
                else{//上一行的注释在本行结束
                    end += 2;
                    lastLineCommentToken.tokenWord += line.substr(0,end);
                    lastLineCommentToken.tokenId = zhy::rust_token_id::COMMENTBLOCK;
                    tokens.push_back(lastLineCommentToken);
                    lastLineCommentToken.clear();
                    scanStart = end;
                }
            }
            else if (line[0] == '!'){
                if (lastLineCommentToken.tokenId == zhy::rust_token_id::ID){
                    lastLineCommentToken.tokenWord += "!";
                    lastLineCommentToken.tokenId = zhy::rust_token_id::MACRONAME;
                    tokens.push_back(lastLineCommentToken);
                    lastLineCommentToken.clear();
                    scanStart = 1;
                }
            }
            zhy::rust_token token;//临时token
            bool isLiteral = true;//如果已经确定不是字面量可以直接跳过字面量的判断逻辑
            for(size_t i = scanStart;i < line.size();i++){
                if (zhy::rust_token::isWhiteSpaceChar(line[i])) continue;//跳过空白字符，如果是字符串中的空白符会被单独处理
                else if (isLiteral && zhy::rust_token::isLiteralStart(line[i])){//处理字面量
                    if (line.substr(i,2) == "br"){//原生字节串
                        size_t start = i + 2;
                        if (line.size() > start && zhy::rust_token::isXIdContinue(line[start])){//防止标识符被误判为字面量
                            i -= 1;
                            isLiteral = false;
                            continue;
                        }
                        size_t end = line.find_first_of("\"",start);
                        if (end == std::string::npos) end = line.size() - 1;
                        size_t num = end - start;//计算#号的数量
                        start = end + 1;
                        end = line.find("\"" + std::string(num,'#'),start);//注意：rust规定原生字符/节串禁用任何转义
                        if (end == std::string::npos) end = line.size() - 1 - num;
                        token.tokenString = line.substr(start,(end - start));
                        //zhy::rust_token::handleEscapeChar(token.tokenString);//注意：rust规定原生字符/节串禁用任何转义
                        token.tokenId = zhy::rust_token_id::BYTESTRINGRAW;
                        start = end + 1 + num;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 + num - i));
                            i = end + num;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,2) == "b\'"){//字节
                        size_t start = i + 2;
                        size_t end = zhy::rust_token::findNotEscape(line,'\'',start);
                        if (end == std::string::npos) end = line.size() - 1;
                        std::string charString = line.substr(start,(end - start));
                        token.tokenCharacter = zhy::rust_token::returnhandledEscapeChar(charString);
                        token.tokenId = zhy::rust_token_id::BYTECHAR;
                        start = end + 1;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 - i));
                            i = end;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,2) == "b\""){//字节串
                        size_t start = i + 2;
                        size_t end = zhy::rust_token::findNotEscape(line,'\"',start);
                        if (end == std::string::npos) end = line.size() - 1;
                        token.tokenString = line.substr(start,(end - start));
                        zhy::rust_token::handleEscapeChar(token.tokenString);
                        token.tokenId = zhy::rust_token_id::BYTESTRING;
                        start = end + 1;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 - i));
                            i = end;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,2) == "0x" || line.substr(i,2) == "0X"){//16进制整数
                        size_t start = i + 2;
                        size_t end = line.size();
                        for(size_t j=start;j<line.size();j++){
                            if (!(zhy::rust_token::isHexadecimalChar(line[j]) || line[j] == '_')){
                                end = j;
                                break;
                            }
                        }
                        std::string hexadecimal = line.substr(start,(end - start));
                        hexadecimal.erase(std::remove(hexadecimal.begin(),hexadecimal.end(),'_'),hexadecimal.end());
                        start = end;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                        }
                        token.tokenWord = line.substr(i,(end - i));
                        token.tokenId = zhy::rust_token_id::INTEGERH;
                        i = end - 1;
                        std::string suffix = line.substr(start,(end - start));
                        auto suffixId = zhy::rust_suffix_map.find(suffix);
                        if (suffixId != zhy::rust_suffix_map.end()){
                            token.tokenSuffix = suffixId->second;
                            if (token.tokenSuffix == zhy::rust_suffix::u8) token.tokenUnsignedInteger = std::stoull(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::i8) token.tokenInteger = std::stoll(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::u16) token.tokenUnsignedInteger = std::stoull(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::i16) token.tokenInteger = std::stoll(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::u32) token.tokenUnsignedInteger = std::stoull(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::i32) token.tokenInteger = std::stoll(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::u64) token.tokenUnsignedInteger = std::stoull(hexadecimal,nullptr,16);
                            else if (token.tokenSuffix == zhy::rust_suffix::i64) token.tokenInteger = std::stoll(hexadecimal,nullptr,16);
                            else token.tokenFloat = std::strtold(("0x" + hexadecimal).c_str(),nullptr);//过大的数字用long double存储
                        }
                        else std::stoll(hexadecimal,nullptr,16);
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,2) == "0o" || line.substr(i,2) == "0O"){//8进制整数
                        size_t start = i + 2;
                        size_t end = line.size();
                        for(size_t j=start;j<line.size();j++){
                            if (!(zhy::rust_token::isOctalChar(line[j]) || line[j] == '_')){
                                end = j;
                                break;
                            }
                        }
                        std::string octal = line.substr(start,(end - start));
                        octal.erase(std::remove(octal.begin(),octal.end(),'_'),octal.end());
                        start = end;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                        }
                        token.tokenWord = line.substr(i,(end - i));
                        token.tokenId = zhy::rust_token_id::INTEGERO;
                        i = end - 1;
                        std::string suffix = line.substr(start,(end - start));
                        auto suffixId = zhy::rust_suffix_map.find(suffix);
                        if (suffixId != zhy::rust_suffix_map.end()){
                            token.tokenSuffix = suffixId->second;
                            if (token.tokenSuffix == zhy::rust_suffix::u8) token.tokenUnsignedInteger = std::stoull(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::i8) token.tokenInteger = std::stoll(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::u16) token.tokenUnsignedInteger = std::stoull(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::i16) token.tokenInteger = std::stoll(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::u32) token.tokenUnsignedInteger = std::stoull(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::i32) token.tokenInteger = std::stoll(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::u64) token.tokenUnsignedInteger = std::stoull(octal,nullptr,8);
                            else if (token.tokenSuffix == zhy::rust_suffix::i64) token.tokenInteger = std::stoll(octal,nullptr,8);
                            else token.tokenFloat = zhy::rust_token::octalToLD(octal);//过大的数字用long double存储
                        }
                        else token.tokenInteger = std::stoll(octal,nullptr,8);
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,2) == "0b" || line.substr(i,2) == "0B"){//2进制整数
                        size_t start = i + 2;
                        size_t end = line.size();
                        for(size_t j=start;j<line.size();j++){
                            if (!(zhy::rust_token::isBinaryChar(line[j]) || line[j] == '_')){
                                end = j;
                                break;
                            }
                        }
                        std::string binary = line.substr(start,(end - start));
                        binary.erase(std::remove(binary.begin(),binary.end(),'_'),binary.end());
                        start = end;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                        }
                        token.tokenWord = line.substr(i,(end - i));
                        token.tokenId = zhy::rust_token_id::INTEGERB;
                        i = end - 1;
                        std::string suffix = line.substr(start,(end - start));
                        auto suffixId = zhy::rust_suffix_map.find(suffix);
                        if (suffixId != zhy::rust_suffix_map.end()){
                            token.tokenSuffix = suffixId->second;
                            if (token.tokenSuffix == zhy::rust_suffix::u8) token.tokenUnsignedInteger = std::stoull(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::i8) token.tokenInteger = std::stoll(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::u16) token.tokenUnsignedInteger = std::stoull(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::i16) token.tokenInteger = std::stoll(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::u32) token.tokenUnsignedInteger = std::stoull(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::i32) token.tokenInteger = std::stoll(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::u64) token.tokenUnsignedInteger = std::stoull(binary,nullptr,2);
                            else if (token.tokenSuffix == zhy::rust_suffix::i64) token.tokenInteger = std::stoll(binary,nullptr,2);
                            else token.tokenFloat = zhy::rust_token::binaryToLD(binary);//过大的数字用long double存储
                        }
                        else token.tokenInteger = std::stoll(binary,nullptr,2);
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,1) == "r"){//原生字符串
                        size_t start = i + 1;
                        if (line.size() > start && zhy::rust_token::isXIdContinue(line[start])){//防止标识符被误判为字面量
                            i -= 1;
                            isLiteral = false;
                            continue;
                        }
                        size_t end = zhy::rust_token::findNotEscape(line,'\"',start);
                        if (end == std::string::npos){
                            if (line.size() > (start+1) && line[start] == '#' && zhy::rust_token::isXIdStart(line[start+1])){//单独处理原生标识符
                                end = line.size();
                                for(size_t j=(start+2);j<line.size();j++){
                                    if (!zhy::rust_token::isXIdContinue(line[j])){
                                        end = j;
                                        break;
                                    }
                                }
                                token.tokenWord = line.substr(i,(end - i));
                                i = end - 1;
                                token.tokenId = zhy::rust_token_id::ID;
                                tokens.push_back(token);
                                token.clear();
                                continue;
                            }
                            else end = line.size() - 1;
                        }
                        size_t num = end - start;//计算#号的数量
                        start = end + 1;
                        end = line.find("\"" + std::string(num,'#'),start);//注意：rust规定原生字符/节串禁用任何转义
                        if (end == std::string::npos) end = line.size() - 1 - num;
                        token.tokenString = line.substr(start,(end - start));
                        //zhy::rust_token::handleEscapeChar(token.tokenString);//注意：rust规定原生字符/节串禁用任何转义
                        token.tokenId = zhy::rust_token_id::STRINGRAW;
                        start = end + 1 + num;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 + num - i));
                            i = end + num;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,1) == "\'"){//字符
                        if (line.size() >= (i + 7) && line.substr(i,7) == "\'static"){
                            token.tokenId = zhy::rust_token_id::KW_STATICLIFETIME;
                            token.tokenWord = line.substr(i,7);
                            tokens.push_back(token);
                            i += 6;
                            isLiteral = true;
                            token.clear();
                            continue;
                        }
                        size_t start = i + 1;
                        size_t end = zhy::rust_token::findNotEscape(line,'\'',start);
                        if (end == std::string::npos) end = line.size() - 1;
                        token.tokenString = line.substr(start,(end - start));
                        zhy::rust_token::handleEscapeChar(token.tokenString);
                        token.tokenId = zhy::rust_token_id::CHAR;
                        start = end + 1;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 - i));
                            i = end;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (line.substr(i,1) == "\""){//字符串
                        size_t start = i + 1;
                        size_t end = zhy::rust_token::findNotEscape(line,'\"',start);
                        if (end == std::string::npos) end = line.size() - 1;
                        token.tokenString = line.substr(start,(end - start));
                        zhy::rust_token::handleEscapeChar(token.tokenString);
                        token.tokenId = zhy::rust_token_id::STRING;
                        start = end + 1;//处理字面量后缀
                        if (line.size() > start && zhy::rust_token::isXIdStart(line[start])){
                            end = line.size();
                            for(size_t j=(start+1);j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                            token.tokenWord = line.substr(i,(end - i));
                            i = end - 1;
                        }
                        else{
                            token.tokenWord = line.substr(i,(end + 1 - i));
                            i = end;
                        }
                        tokens.push_back(token);
                        token.clear();
                    }
                    else if (std::isdigit(line[i])){//10进制整数或浮点数
                        size_t start = i;
                        size_t end = line.size();
                        for(size_t j=(start+1);j<line.size();j++){
                            if (line[j] == '.'){
                                if (j > 0 && std::isdigit(line[j-1]) && line.size() > (j+1) && std::isdigit(line[j+1])) continue;
                                else{
                                    end = j;
                                    break;
                                }
                            }
                            else if (line[j] == 'e' || line[j] == 'E'){
                                if (j > 0 && std::isdigit(line[j-1]) && line.size() > (j+1) && zhy::rust_token::isExponentStart(line[j+1])){
                                    if (line[j+1] == '-' || line[j+1] == '+') j += 1;
                                    continue;
                                }
                                else{
                                    end = j;
                                    break;
                                }
                            }
                            else if (line[j] == '_'){
                                if (j > 0 && std::isdigit(line[j-1]) && line.size() > (j+1) && std::isdigit(line[j+1])) continue;
                                else{
                                    end = j;
                                    break;
                                }
                            }
                            else if (!std::isdigit(line[j]) && line[j] != '.' && line[j] != 'e' && line[j] != 'E' && line[j] != '_'){
                                end = j;
                                break;
                            }
                        }
                        if (zhy::rust_token::isXIdStart(line[end])){
                            size_t endStart = end + 1;
                            end = line.size();
                            for(size_t j=endStart;j<line.size();j++){
                                if (!zhy::rust_token::isXIdContinue(line[j])){
                                    end = j;
                                    break;
                                }
                            }
                        }
                        std::string number_str = line.substr(start,(end - start));
                        bool isInteger = true;
                        bool exponentPart = false;
                        std::string base_str = "";
                        std::string exponent_str = "";
                        std::string suffix = "";
                        for(size_t j=i;j<end;j++){
                            if (line[j] == '.'){
                                isInteger = false;
                                continue;
                            }
                            else if (line[j] == 'e' || line[j] == 'E'){
                                if (exponentPart){
                                    suffix = line.substr(j,(end - j));
                                    exponent_str = line.substr(start,(j - start));
                                    break;
                                }
                                else{
                                    base_str = line.substr(start,(j - start));
                                    if (line.size() > (j+1) && zhy::rust_token::isExponentStart(line[j+1])){
                                        isInteger = false;
                                        exponentPart = true;
                                        start = j + 1;
                                        continue;
                                    }
                                    else{
                                        suffix = line.substr(j,(end - j));
                                        break;
                                    }
                                }
                            }
                            else if (line[j] == '_'){
                                if (j > 0 && std::isdigit(line[j-1]) && line.size() > (j+1) && std::isdigit(line[j+1])) continue;
                                else{
                                    suffix = line.substr(j,(end - j));
                                    if (exponentPart) exponent_str = line.substr(start,(j - start));
                                    else base_str = line.substr(start,(j - start));
                                    break;
                                }
                            }
                            else if (line[j] != '.' && line[j] != 'e' && line[j] != 'E' && line[j] != '_' && zhy::rust_token::isXIdStart(line[j])){
                                suffix = line.substr(j,(end - j));
                                if (exponentPart) exponent_str = line.substr(start,(j - start));
                                else base_str = line.substr(start,(j - start));
                                break;
                            }
                        }
                        token.tokenWord = number_str;
                        if (isInteger) token.tokenId = zhy::rust_token_id::INTEGERD;
                        else token.tokenId = zhy::rust_token_id::FLOAT;
                        base_str.erase(std::remove(base_str.begin(),base_str.end(),'_'),base_str.end());
                        exponent_str.erase(std::remove(exponent_str.begin(),exponent_str.end(),'_'),exponent_str.end());
                        long double base = 0.0;
                        if (!base_str.empty()) base = std::stold(base_str);
                        size_t exponent = 0;
                        if (!exponent_str.empty()) exponent = std::stoull(exponent_str);
                        long double number = base * std::pow(10,exponent);
                        auto suffixId = zhy::rust_suffix_map.find(suffix);
                        if (suffixId != zhy::rust_suffix_map.end()){
                            token.tokenSuffix = suffixId->second;
                            if (token.tokenSuffix == zhy::rust_suffix::u8) token.tokenUnsignedInteger = static_cast <size_t>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::i8) token.tokenInteger = static_cast <long long>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::u16) token.tokenUnsignedInteger = static_cast <size_t>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::i16) token.tokenInteger = static_cast <long long>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::u32) token.tokenUnsignedInteger = static_cast <size_t>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::i32) token.tokenInteger = static_cast <long long>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::u64) token.tokenUnsignedInteger = static_cast <size_t>(number);
                            else if (token.tokenSuffix == zhy::rust_suffix::i64) token.tokenInteger = static_cast <long long>(number);
                            else token.tokenFloat = number;//过大的数字用long double存储
                        }
                        tokens.push_back(token);
                        token.clear();
                        i = end - 1;
                    }
                    else{//把标识符当成了字面量，重新扫描
                        i -= 1;
                        isLiteral = false;
                        continue;
                    }
                    continue;
                }
                else if (zhy::rust_token::isXIdStart(line[i])){//处理标识符、关键字、保留关键字、基本数据类型等
                    size_t start = i;
                    size_t end = line.size();
                    for(size_t j=(i+1);j<line.size();j++){
                        if (!zhy::rust_token::isXIdContinue(line[j])){
                            end = j;
                            break;
                        }
                    }
                    token.tokenWord = line.substr(start,(end - start));
                    if (zhy::rust_token::isRustKeyWord(token.tokenWord)){
                        token.tokenId = zhy::rust_kw_map.find(token.tokenWord)->second;
                    }
                    else{
                        token.tokenId = zhy::rust_token_id::ID;
                    }
                    tokens.push_back(token);
                    token.clear();
                    i = end - 1;
                    isLiteral = true;
                    continue;
                }
                else if (zhy::rust_token::isSymbol(line[i])){//处理操作符、分隔符、标点符号等
                    if (line.size() >= (i+3) && zhy::rust_token::isRustKeyWord(line.substr(i,3))){
                        token.tokenWord = line.substr(i,3);
                        token.tokenId = zhy::rust_kw_map.find(token.tokenWord)->second;
                        i += 2;
                    }
                    else if (line.size() >= (i+2) && zhy::rust_token::isRustKeyWord(line.substr(i,2))){
                        token.tokenWord = line.substr(i,2);
                        token.tokenId = zhy::rust_kw_map.find(token.tokenWord)->second;
                        i += 1;
                    }
                    else if (line.size() >= (i+1) && zhy::rust_token::isRustKeyWord(line.substr(i,1))){
                        token.tokenWord = line.substr(i,1);
                        token.tokenId = zhy::rust_kw_map.find(token.tokenWord)->second;
                    }
                    else{
                        token.tokenWord = line.substr(i,1);
                        token.tokenId = zhy::rust_token_id::NONEID;
                    }
                    if (token.tokenId == zhy::rust_token_id::OPERATOR && token.tokenWord == "!"){//合并行内宏调用，规定标识符 + '!' = 宏调用
                        if (tokens.size() > 0 && tokens.back().tokenId == zhy::rust_token_id::ID){
                            tokens.back().tokenId = zhy::rust_token_id::MACRONAME;
                            tokens.back().tokenWord += "!";
                            token.clear();
                            continue;
                        }
                    }
                    else if (zhy::rust_token::isCommentStart(token.tokenWord)){//如果分割符为注释起始符号
                        if (token.tokenWord == "//"){//处理行注释
                            size_t start = i - 1;
                            //std::cerr<<start<<std::endl;
                            token.tokenWord = line.substr(start,(line.size() - start));
                            token.tokenId = zhy::rust_token_id::COMMENTLINE;
                            tokens.push_back(token);
                            break;
                        }
                        else if (token.tokenWord == "/*"){//处理段注释
                            commentNum += 1;
                            size_t start = i - 1;
                            size_t end = std::string::npos;
                            for(size_t j=i;j<(line.size()-1);j++){
                                if (line.substr(j,2) == "/*") commentNum += 1;
                                else if (line.substr(j,2) == "*/") commentNum -= 1;
                                if (commentNum <= 0){
                                    end = j;
                                    break;
                                }
                            }
                            if (end == std::string::npos){//本行后面所有内容均为段注释
                                token.tokenWord = line.substr(start,(line.size() - start));
                                token.tokenId = zhy::rust_token_id::COMMENTBLOCK;
                                lastLineCommentToken = token;
                                tokens.push_back(token);
                                break;
                            }
                            else{//段注释存在终点
                                end += 2;
                                token.tokenWord = line.substr(start,(end - start));
                                token.tokenId = zhy::rust_token_id::COMMENTBLOCK;
                                i = end - 1;
                            }
                        }
                        else if (token.tokenWord == "///"){//处理文档注释
                            size_t start = i - 2;
                            token.tokenWord = line.substr(start,(line.size() - start));
                            token.tokenId = zhy::rust_token_id::COMMENTITEM;
                            tokens.push_back(token);
                            break;
                        }
                        else{
                            size_t start = i - 2;
                            token.tokenWord = line.substr(start,(line.size() - start));
                            token.tokenId = zhy::rust_token_id::COMMENTDOCUMENT;
                            tokens.push_back(token);
                            break;
                        }
                    }
                    tokens.push_back(token);
                    token.clear();
                    continue;
                }



            }


            lastLineCommentToken = tokens.back();
            return true;
        }

        inline static void unionLineToken(std::vector <zhy::rust_token> & tokens){//合并行内token，比如标识符 + '!'应改为宏调用，不过暂时不用
            for(size_t i=1;i<tokens.size();i++){
                if (tokens[i].tokenId == zhy::rust_token_id::OPERATOR && tokens[i].tokenWord == "!"){
                    if (tokens[i-1].tokenId == zhy::rust_token_id::ID){
                        tokens[i-1].tokenId = zhy::rust_token_id::MACRONAME;
                        tokens[i-1].tokenWord += "!";
                        tokens.erase(tokens.begin() + i);
                        i -= 1;
                    }
                }
            }
        }

        inline static bool processFileLine(const std::string & filePath,std::ostream & out = std::cout,std::ostream & err = std::cerr){
            std::ifstream in(std::filesystem::path(filePath),std::ios::in);
            if (!in){
                err<<"文件打开失败: "<<filePath<<std::endl;
                return false;
            }
            bool succ = true;
            long long commentNum = 0;
            zhy::rust_token lastLineCommentToken;
            while (true){
                try{
                    std::vector <zhy::rust_token> tokens;
                    if (!zhy::rust_lexer::getLineToken <std::vector>(in,tokens,commentNum,lastLineCommentToken,out)) break;
                    //unionLineToken(tokens);
                    for(const zhy::rust_token & token : tokens){
                        out<<"    ";
                        token.displayToken(out);
                    }
                }
                catch (const std::exception & e){
                    err<<"词法分析器在分析代码时出现了未知的问题，我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    succ = false;
                    continue;
                }
            }
            in.close();
            return succ;
        }

        inline static bool outputFileProcessFileLine(
            const std::string & inputPath,//输入文件路径
            const std::string & outputPath,//输出文件所在文件夹
            const std::string & outputName = "untitled",//输出文件名
            const std::string & outputType = ".txt",//输出文件类型
            std::ostream & err = std::cerr//错误流，可适配标准错误流、文件输出流、字符串输出流
        ){
            std::string outputFile = outputName + outputType;
            std::filesystem::path output = std::filesystem::path(outputPath) / std::filesystem::path(outputFile);
            std::ofstream out(output,std::ios::out);
            if (!out){
                err<<"文件打开失败: "<<outputPath<<std::endl;
                return false;
            }
            bool succ = processFileLine(inputPath,out,err);
            out.close();
            return succ;
        }

    };

    class file_token_getter{

        protected:

        std::deque <zhy::rust_token> tokenDeque;
        long long commentNum = 0;
        zhy::rust_token lastLineCommentToken;

        std::string filePath;
        std::ifstream in;
        std::ostream & out;
        std::ostream & err;
        bool openSuccess;

        bool pushLineToken(){
            return zhy::rust_lexer::getLineToken <std::deque>(in,tokenDeque,commentNum,lastLineCommentToken,out);
        }

        public:
        file_token_getter(
            const std::string & fp,
            std::ostream & o = std::cout,
            std::ostream & e = std::cerr
        ):filePath(fp),out(o),err(e){
            in.open(std::filesystem::path(filePath),std::ios::in);
            if (in) openSuccess = true;
            else{
                err<<"文件打开失败: "<<filePath<<std::endl;
                openSuccess = false;
            }
        }

        ~file_token_getter(){
            if (openSuccess) in.close(); 
        }

        bool getNextToken(zhy::rust_token & token){
            while (tokenDeque.empty()){
                try{
                    if (!pushLineToken()) return false;
                }
                catch (const std::exception & e){
                    err<<"词法分析器在分析代码时出现了未知的问题，我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    continue;
                }
            }
            if (!tokenDeque.empty()){
                token = tokenDeque.front();
                tokenDeque.pop_front();
            }
            else return false;
            return true;
        }

        friend bool operator ==(const file_token_getter & lhs,const file_token_getter & rhs);
        friend struct std::hash <zhy::file_token_getter>;
        




    };

    inline bool operator ==(const file_token_getter & lhs,const file_token_getter & rhs){
        return {
            lhs.filePath == rhs.filePath &&
            lhs.tokenDeque == rhs.tokenDeque &&
            lhs.commentNum == rhs.commentNum &&
            lhs.lastLineCommentToken == rhs.lastLineCommentToken &&
            lhs.openSuccess == rhs.openSuccess
        };
    }










    class rust_command{

        private:

        inline static const std::unordered_map <
            std::string,
            std::unordered_map <
                std::string,
                std::function <void(
                    const std::vector <std::string> & parts,
                    std::ostream & out,
                    std::ostream & err
                )>
            >
        > commandMap = {
            {
                "lexer", {
                    {"processFileLine", [](const std::vector <std::string> & parts,std::ostream & out,std::ostream & err){
                        //Zrust lexer processFileLine <filePath>
                        if (parts.size() >= 4){
                            if (!zhy::rust_lexer::processFileLine(parts[3],out,err)) err<<"对文件: "<<parts[3]<<" 进行词法分析时出现错误!"<<std::endl;
                            else out<<"对文件: "<<parts[3]<<" 进行词法分析成功!"<<std::endl;
                        }
                        else std::cerr<<"指令格式错误!"<<std::endl;
                        return;
                    }},
                    {"outputFileProcessFileLine", [](const std::vector <std::string> & parts,std::ostream & out,std::ostream & err){
                        //Zrust lexer outputFileProcessFileLine <inputPath> <outputPath> <outputName> <outputType>
                        if (parts.size() >= 6){
                            std::string outputType = ".txt";
                            if (parts.size() >= 7) outputType = parts[6];
                            if (!zhy::rust_lexer::outputFileProcessFileLine(parts[3],parts[4],parts[5],outputType,err)) err<<"对文件: "<<parts[3]<<" 进行词法分析时出现错误!"<<std::endl;
                            else out<<"对文件: "<<parts[3]<<" 进行词法分析成功!"<<std::endl;
                        }
                        else std::cerr<<"指令格式错误!"<<std::endl;
                        return;
                    }}
                }
            }


            
        };

        rust_command() = delete;
        ~rust_command() = delete;
        rust_command(const rust_command & other) = delete;
        rust_command & operator =(const rust_command & other) = delete;

        inline static void cmdSplit(const std::string & cmd,std::vector <std::string> & parts){
            parts.clear();
            std::istringstream iss(cmd);
            std::string part;
            while(iss>>part){
                parts.push_back(part);
            }
            return;
        }

        public:

        inline static void execute(std::istream & in = std::cin,std::ostream & out = std::cout,std::ostream & err = std::cerr){
            std::string cmd;
            while (std::getline(in,cmd)){
                try{
                    std::vector <std::string> parts;
                    cmdSplit(cmd,parts);
                    if (parts.size() < 2){
                        if (parts.size() == 1 && parts[0] == "exit") break;
                        err<<"指令格式错误!"<<std::endl;
                        continue;
                    }
                    else if (parts[0] != "Zrust"){
                        err<<"未知的指令系统: "<<parts[0]<<std::endl;
                        continue;
                    }
                    else{
                        if (parts[1] == "exit") break;
                        auto commandIt = commandMap.find(parts[1]);
                        if (commandIt == commandMap.end()){
                            err<<"未知的指令: "<<parts[1]<<std::endl;
                            continue;
                        }
                        else{
                            auto cmdIt = commandIt->second.find(parts[2]);
                            if (cmdIt == commandIt->second.end()){
                                err<<"未知的指令: "<<parts[2]<<std::endl;
                                continue;
                            }
                            else{
                                cmdIt->second(parts,out,err);
                            }
                        }
                    }
                } catch (const std::exception & e){
                    err<<e.what()<<std::endl;
                    continue;
                }
            }
            return;
        }












    };


}

namespace std{

    template <>
    struct hash <zhy::rust_token>{//重载std::hash以支持zhy::rust_token作为std::unordered_set的元素或std::unordered_map的键
        inline size_t operator()(const zhy::rust_token & token) const{
            return static_cast <size_t>(token.tokenId) ^ (std::hash <std::string>()(token.tokenWord) << 1);
        }
    };

    template <>
    struct hash <zhy::file_token_getter>{//重载std::hash以支持zhy::file_token_getter作为std::unordered_set的元素或std::unordered_map的键
        inline size_t operator()(const zhy::file_token_getter & ftg) const{
            return {
                std::hash <std::string>()(ftg.filePath) ^
                (std::hash <zhy::rust_token>()(ftg.tokenDeque.front()) << 1) ^
                (std::hash <zhy::rust_token>()(ftg.tokenDeque.back()) << 2) ^
                (std::hash <long long>()(ftg.commentNum) << 3) ^
                (std::hash <zhy::rust_token>()(ftg.lastLineCommentToken) << 4) ^
                (std::hash <bool>()(ftg.openSuccess) << 5)
            };
        }
    };

}



#endif