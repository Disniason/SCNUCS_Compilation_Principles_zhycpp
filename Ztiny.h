#ifndef Z_TINY_H
#define Z_TINY_H
#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <memory>//智能指针
#include <type_traits>

namespace zhy{

    enum class tiny_token_id{
        KW_IF,
        KW_THEN,
        KW_ELSE,
        KW_ENDIF,

        KW_REPEAT,
        KW_UNTIL,
        KW_READ,
        KW_WRITE,

        KW_WHILE,
        KW_ENDWHILE,
        KW_FOR,
        KW_ENDFOR,

        OP_ADDITION,
        OP_SUBTRACTION,
        OP_MULTIPLICATION,
        OP_DIVISION,
        OP_EQUAL,
        OP_LESS,
        OP_BRACKETLEFT,
        OP_BRACKETRIGHT,
        OP_SEMICOLON,
        OP_ASSIGNMENT,

        OP_INCREMENT,
        OP_DECREMENT,
        OP_MODULUS,
        OP_POWER,
        OP_LESSEQUAL,
        OP_OVER,
        OP_OVEREQUAL,
        OP_NOTEQUAL,
        OP_ASSIGNMENTREGEXP,

        REGOP_OR,
        REGOP_CONNECT,
        REGOP_CLOSURE,
        REGOP_OPTIONAL,

        LT_UNSIGNEDINTEGER,

        COMMENT,

        ID,

        ERROR
    };

    class tiny_token{

        public:
        inline static const std::unordered_map <std::string,zhy::tiny_token_id> keyword_id_map = {
            {"if",zhy::tiny_token_id::KW_IF},
            {"then",zhy::tiny_token_id::KW_THEN},
            {"else",zhy::tiny_token_id::KW_ELSE},
            {"endif",zhy::tiny_token_id::KW_ENDIF},

            {"repeat",zhy::tiny_token_id::KW_REPEAT},
            {"until",zhy::tiny_token_id::KW_UNTIL},
            {"read",zhy::tiny_token_id::KW_READ},
            {"write",zhy::tiny_token_id::KW_WRITE},

            {"while",zhy::tiny_token_id::KW_WHILE},
            {"endwhile",zhy::tiny_token_id::KW_ENDWHILE},
            {"for",zhy::tiny_token_id::KW_FOR},
            {"endfor",zhy::tiny_token_id::KW_ENDFOR}
        };

        inline static const std::unordered_map <std::string,zhy::tiny_token_id> operator_id_map = {
            {"::=",zhy::tiny_token_id::OP_ASSIGNMENTREGEXP},
            
            {":=",zhy::tiny_token_id::OP_ASSIGNMENT},
            {"++",zhy::tiny_token_id::OP_INCREMENT},
            {"--",zhy::tiny_token_id::OP_DECREMENT},
            {"<=",zhy::tiny_token_id::OP_LESSEQUAL},
            {">=",zhy::tiny_token_id::OP_OVEREQUAL},
            {"<>",zhy::tiny_token_id::OP_NOTEQUAL},

            {"+",zhy::tiny_token_id::OP_ADDITION},
            {"-",zhy::tiny_token_id::OP_SUBTRACTION},
            {"*",zhy::tiny_token_id::OP_MULTIPLICATION},
            {"/",zhy::tiny_token_id::OP_DIVISION},
            {"=",zhy::tiny_token_id::OP_EQUAL},
            {"<",zhy::tiny_token_id::OP_LESS},
            {"(",zhy::tiny_token_id::OP_BRACKETLEFT},
            {")",zhy::tiny_token_id::OP_BRACKETRIGHT},
            {";",zhy::tiny_token_id::OP_SEMICOLON},
            {"%",zhy::tiny_token_id::OP_MODULUS},
            {"^",zhy::tiny_token_id::OP_POWER},
            {">",zhy::tiny_token_id::OP_OVER},

            {"|",zhy::tiny_token_id::REGOP_OR},
            {"&",zhy::tiny_token_id::REGOP_CONNECT},
            {"#",zhy::tiny_token_id::REGOP_CLOSURE},
            {"?",zhy::tiny_token_id::REGOP_OPTIONAL}
        };

        inline static const std::unordered_set <char> operator_start = {
            ':','+','-','<','>','*','/',
            '=','(',')',';','%','^',
            '|','&','#','?'
        };

        zhy::tiny_token_id tokenId;
        std::string tokenWord;
        size_t tokenUnsignedInteger;

        size_t linePosition;
        size_t columnPosition;

        tiny_token()
            :tokenWord(),tokenUnsignedInteger(0),tokenId(zhy::tiny_token_id::ERROR),
            linePosition(0),columnPosition(0)
        {}
        ~tiny_token(){}
        tiny_token(const zhy::tiny_token & other):
            tokenId(other.tokenId),
            tokenWord(other.tokenWord),
            tokenUnsignedInteger(other.tokenUnsignedInteger),
            linePosition(other.linePosition),
            columnPosition(other.columnPosition)
        {}
        zhy::tiny_token & operator =(const zhy::tiny_token & other){
            if (this != &other){
                tokenId = other.tokenId;
                tokenWord = other.tokenWord;
                tokenUnsignedInteger = other.tokenUnsignedInteger;
                linePosition = other.linePosition;
                columnPosition = other.columnPosition;
            }
            return *this;
        }

        void clear(){
            tokenId = zhy::tiny_token_id::ERROR;
            tokenWord.clear();
            tokenUnsignedInteger = 0;
            linePosition = 0;
            columnPosition = 0;
            return;
        }

        bool empty(){
            return (
                tokenId == zhy::tiny_token_id::ERROR &&
                tokenWord.empty() &&
                tokenUnsignedInteger == 0 &&
                linePosition == 0 &&
                columnPosition == 0
            );
        }

        template <class T>
        const T & getValue() const{
            if constexpr (std::is_same_v <T,size_t>){
                if (tokenId == zhy::tiny_token_id::LT_UNSIGNEDINTEGER) return tokenUnsignedInteger;
                else throw std::runtime_error("token 不是无符号整数类型，无法获取 size_t 值");
            }
            else if constexpr (std::is_same_v <T,std::string>) return tokenWord;
            else static_assert(std::is_same_v <T,void>, "不支持的模板类型 T");
        }

        void getDisplayStr(std::string & str) const{
            switch (tokenId){
                case zhy::tiny_token_id::KW_IF:{str = "关键字(if)";return;}
                case zhy::tiny_token_id::KW_THEN:{str = "关键字(then)";return;}
                case zhy::tiny_token_id::KW_ELSE:{str = "关键字(else)";return;}
                case zhy::tiny_token_id::KW_ENDIF:{str = "关键字(endif)";return;}
                case zhy::tiny_token_id::KW_REPEAT:{str = "关键字(repeat)";return;}
                case zhy::tiny_token_id::KW_UNTIL:{str = "关键字(until)";return;}
                case zhy::tiny_token_id::KW_READ:{str = "关键字(read)";return;}
                case zhy::tiny_token_id::KW_WRITE:{str = "关键字(write)";return;}
                case zhy::tiny_token_id::KW_WHILE:{str = "关键字(while)";return;}
                case zhy::tiny_token_id::KW_ENDWHILE:{str = "关键字(endwhile)";return;}
                case zhy::tiny_token_id::KW_FOR:{str = "关键字(for)";return;}
                case zhy::tiny_token_id::KW_ENDFOR:{str = "关键字(endfor)";return;}
                case zhy::tiny_token_id::OP_ADDITION:{str = "运算符(+)";return;}
                case zhy::tiny_token_id::OP_SUBTRACTION:{str = "运算符(-)";return;}
                case zhy::tiny_token_id::OP_MULTIPLICATION:{str = "运算符(*)";return;}
                case zhy::tiny_token_id::OP_DIVISION:{str = "运算符(/)";return;}
                case zhy::tiny_token_id::OP_EQUAL:{str = "运算符(=)";return;}
                case zhy::tiny_token_id::OP_LESS:{str = "运算符(<)";return;}
                case zhy::tiny_token_id::OP_BRACKETLEFT:{str = "运算符(()";return;}
                case zhy::tiny_token_id::OP_BRACKETRIGHT:{str = "运算符())";return;}
                case zhy::tiny_token_id::OP_SEMICOLON:{str = "运算符(;)";return;}
                case zhy::tiny_token_id::OP_ASSIGNMENT:{str = "运算符(:=)";return;}
                case zhy::tiny_token_id::OP_INCREMENT:{str = "运算符(++)";return;}
                case zhy::tiny_token_id::OP_DECREMENT:{str = "运算符(--)";return;}
                case zhy::tiny_token_id::OP_MODULUS:{str = "运算符(%)";return;}
                case zhy::tiny_token_id::OP_POWER:{str = "运算符(^)";return;}
                case zhy::tiny_token_id::OP_LESSEQUAL:{str = "运算符(<=)";return;}
                case zhy::tiny_token_id::OP_OVER:{str = "运算符(>)";return;}
                case zhy::tiny_token_id::OP_OVEREQUAL:{str = "运算符(>=)";return;}
                case zhy::tiny_token_id::OP_NOTEQUAL:{str = "运算符(<>)";return;}
                case zhy::tiny_token_id::OP_ASSIGNMENTREGEXP:{str = "运算符(::=)";return;}
                case zhy::tiny_token_id::LT_UNSIGNEDINTEGER:{str = "字面量(无符号整数)";return;}
                case zhy::tiny_token_id::COMMENT:{str = "注释";return;}
                case zhy::tiny_token_id::ID:{str = "标识符";return;}
                default:{str = "未知的token id";return;};
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            std::string displayStr = "";
            getDisplayStr(displayStr);
            out<<tokenWord<<" : "<<displayStr<<" "<<linePosition<<" "<<columnPosition<<std::endl;
            return;
        }

        void getTable(size_t tokenPosition,std::vector <std::string> & table) const{
            table.resize(5);
            table[0] = std::to_string(tokenPosition);
            table[1] = tokenWord;
            getDisplayStr(table[2]);
            table[3] = std::to_string(linePosition);
            table[4] = std::to_string(columnPosition);
            return;
        }

        bool isUnaryOperator() const{
            return (
                tokenId == zhy::tiny_token_id::OP_INCREMENT ||
                tokenId == zhy::tiny_token_id::OP_DECREMENT
            );
        }

        bool isAddOperator() const{
            return (
                tokenId == zhy::tiny_token_id::OP_ADDITION ||
                tokenId == zhy::tiny_token_id::OP_SUBTRACTION
            );
        }

        bool isMulOperator() const{
            return (
                tokenId == zhy::tiny_token_id::OP_MULTIPLICATION ||
                tokenId == zhy::tiny_token_id::OP_DIVISION ||
                tokenId == zhy::tiny_token_id::OP_MODULUS
            );
        }

        bool isComparisonOp() const{
            return (
                tokenId == zhy::tiny_token_id::OP_EQUAL ||
                tokenId == zhy::tiny_token_id::OP_LESS ||
                tokenId == zhy::tiny_token_id::OP_OVER ||
                tokenId == zhy::tiny_token_id::OP_LESSEQUAL ||
                tokenId == zhy::tiny_token_id::OP_OVEREQUAL ||
                tokenId == zhy::tiny_token_id::OP_NOTEQUAL
            );
        }

        bool isRegUnaryOp(){
            return (
                tokenId == zhy::tiny_token_id::REGOP_CLOSURE ||
                tokenId == zhy::tiny_token_id::REGOP_OPTIONAL
            );
        }

        bool isStatementFirst(){
            return (
                tokenId == zhy::tiny_token_id::KW_IF ||
                tokenId == zhy::tiny_token_id::KW_REPEAT ||
                tokenId == zhy::tiny_token_id::ID ||
                tokenId == zhy::tiny_token_id::KW_READ ||
                tokenId == zhy::tiny_token_id::KW_WRITE ||
                tokenId == zhy::tiny_token_id::KW_FOR ||
                tokenId == zhy::tiny_token_id::KW_WHILE ||
                tokenId == zhy::tiny_token_id::OP_INCREMENT ||
                tokenId == zhy::tiny_token_id::OP_DECREMENT
            );
        }

        inline static bool isIdStart(char input = ' '){
            return std::isalpha(static_cast <unsigned char>(input));
        }

        inline static bool isIdContinue(char input = ' '){
            return std::isalnum(static_cast <unsigned char>(input));
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

        inline static bool isDigit(char input = ' '){
            return std::isdigit(static_cast <unsigned char>(input));
        }

        inline static bool isOperatorStart(char input = ' '){
            return zhy::tiny_token::operator_start.find(input) != zhy::tiny_token::operator_start.end();
        }

        inline static bool isKeyword(const std::string & input){
            return zhy::tiny_token::keyword_id_map.find(input) != zhy::tiny_token::keyword_id_map.end();
        }

        inline static bool isOperator(const std::string & input){
            return zhy::tiny_token::operator_id_map.find(input) != zhy::tiny_token::operator_id_map.end();
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

        inline static size_t findNotEscape(const std::string & str,char input = ' ',size_t start = 0){
            if (!zhy::tiny_token::isEscapeChar(input)) return str.find_first_of(input,start);
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
            return str.size();
        }

    };

    class tiny_error{

        public:
        std::string description;
        std::string filePath;
        size_t linePosition;
        size_t columnPosition;

        tiny_error(const std::string & d,const std::string & fp,size_t lp,size_t cp):
            description(d),
            filePath(fp),
            linePosition(lp),
            columnPosition(cp)
        {}
        ~tiny_error(){}

        tiny_error(const zhy::tiny_error & other):
            description(other.description),
            filePath(other.filePath),
            linePosition(other.linePosition),
            columnPosition(other.columnPosition)
        {}

        zhy::tiny_error & operator =(const zhy::tiny_error & other){
            if (this != &other){
                description = other.description;
                filePath = other.filePath;
                linePosition = other.linePosition;
                columnPosition = other.columnPosition;
            }
            return *this;
        }

        void clear(){
            description.clear();
            filePath.clear();
            linePosition = 0;
            columnPosition = 0;
            return;
        }

        void display(std::ostream & out = std::cout) const{
            out<<"描述: "<<description<<std::endl;
            out<<"文件: "<<filePath<<std::endl;
            out<<"行: "<<linePosition<<std::endl;
            out<<"列: "<<columnPosition<<std::endl;
            return;
        }

        void getTable(size_t errorPosition,std::vector <std::string> & table) const{
            table.resize(5);
            table[0] = std::to_string(errorPosition);
            table[1] = description;
            table[2] = filePath;
            table[3] = std::to_string(linePosition);
            table[4] = std::to_string(columnPosition);
            return;
        }

        inline static const std::vector <std::string> tiny_lexical_error_description_map = {
            "词法错误: 未知的符号出现在代码中导致出现未知的token",
            "词法错误: 注释未闭合(缺少\'}\'与之对应)"
        };

        inline static const std::vector <std::string> tiny_syntax_error_description_map = {
            "语法错误",//0
            "语法错误: 不存在期望的token\"if\"",//1
            "语法错误: 不存在期望的token\"then\"",//2
            "语法错误: 不存在期望的token\"else\"",//3
            "语法错误: 不存在期望的token\"endif\"",//4
            "语法错误: 不存在期望的token\"repeat\"",//5
            "语法错误: 不存在期望的token\"until\"",//6
            "语法错误: 不存在期望的token\":=\"",//7
            "语法错误: 不存在期望的token\"read\"",//8
            "语法错误: 不存在期望的token\"write\"",//9
            "语法错误: 不存在期望的token\"for\"",//10
            "语法错误: 不存在期望的token\"(\"",//11
            "语法错误: 不存在期望的token\")\"",//12
            "语法错误: 不存在期望的token\";\"",//13
            "语法错误: 不存在期望的token\"while\"",//14
            "语法错误: 不存在期望的token\"endwhile\"",//15
            "语法错误: 不存在期望的token\"::=\"",//16

            "语法错误: 缺少表达式、数字、标识符或单目运算表达式",//17
            "语法错误: 不存在期望的数字",//18
            "语法错误: 不存在期望的标识符",//19
            "语法错误: 缺少单目运算符(\"++\"或\"--\")",//20
            "语法错误: 不存在期望的token\"++\"",//21
            "语法错误: 不存在期望的token\"--\"",//22
            "语法错误: 不存在期望的token\"^\"",//23
            "语法错误: 不存在期望的token\"*\"",//24
            "语法错误: 不存在期望的token\"/\"",//25
            "语法错误: 不存在期望的token\"%\"",//26
            "语法错误: 不存在期望的token\"+\"",//27
            "语法错误: 不存在期望的token\"-\"",//28
            "语法错误: 缺少比较运算符(\"=\" \"<\" \">\" \"<=\" \">=\" \"<>\")",//29
            "语法错误: 不存在期望的token\"endfor\"",//30
            "语法错误: 不存在期望的token\"#\"",//31
            "语法错误: 不存在期望的token\"?\"",//32
            "语法错误: 不存在期望的token\"&\"",//33
            "语法错误: 不存在期望的token\"|\"",//34
            "语法错误: 缺少赋值运算符(\":=\"或\"::=\")",//35
            "语法错误: 缺少statement的起始符(\"if\" \"repeat\" \"read\" \"write\" \"for\" \"while\" 标识符或单目运算符)"//36
        };

    };

    class tiny_tokens{

        private:
        std::string filePath;
        std::ifstream in;
        std::ostream & out;
        std::deque <zhy::tiny_token> tokenDeque;
        std::vector <zhy::tiny_error> tokenErrors;

        tiny_tokens(const zhy::tiny_tokens & other) = delete;
        tiny_tokens & operator =(const zhy::tiny_tokens & other) = delete;

        size_t linePosition;
        size_t columnPosition;
        bool isComment;
        zhy::tiny_token lastLineToken;
        std::tuple <size_t,size_t> commentStartPosition;

        void addError(size_t errorType,size_t line,size_t column){
            tokenErrors.emplace_back(zhy::tiny_error(
                zhy::tiny_error::tiny_lexical_error_description_map.at(errorType),
                filePath,
                line,
                column
            ));
            return;
        }

        bool getLineToken(const std::string & line){
            size_t scanStart = 0;
            if (isComment){
                size_t end = line.find('}');
                if (end == std::string::npos){
                    end = line.size();
                    isComment = true;
                }
                else{
                    end += 1;
                    isComment = false;
                }
                lastLineToken.tokenId = zhy::tiny_token_id::COMMENT;
                lastLineToken.tokenWord += " " + line.substr(0,end);
                if (!isComment) tokenDeque.emplace_back(lastLineToken);
                else return true;
                scanStart = end + 1;
            }
            zhy::tiny_token tempToken;
            for(size_t i=scanStart;i<line.size();i++){
                columnPosition = i + 1;
                tempToken.linePosition = linePosition;
                tempToken.columnPosition = columnPosition;
                if (zhy::tiny_token::isWhiteSpaceChar(line[i])) continue;//跳过空白字符，如果是字符串中的空白符会被单独处理
                else if (zhy::tiny_token::isDigit(line[i])){//处理数字
                    size_t start = i;
                    size_t end = line.size();
                    for(size_t j=(start + 1);j<line.size();j++){
                        if (!zhy::tiny_token::isDigit(line[j])){
                            end = j;
                            break;
                        }
                    }
                    tempToken.tokenId = zhy::tiny_token_id::LT_UNSIGNEDINTEGER;
                    tempToken.tokenWord = line.substr(start,(end - start));
                    tempToken.tokenUnsignedInteger = std::stoull(tempToken.tokenWord);
                    tokenDeque.emplace_back(tempToken);
                    tempToken.clear();
                    i = end - 1;
                }
                else if (line[i] == '{'){//处理注释
                    size_t start = i;
                    size_t end = line.find('}');
                    if (end == std::string::npos){
                        end = line.size();
                        isComment = true;
                        commentStartPosition = std::make_tuple(linePosition,start + 1);
                        lastLineToken.tokenId = zhy::tiny_token_id::COMMENT;
                        lastLineToken.tokenWord = line.substr(start,(end - start));
                        lastLineToken.linePosition = linePosition;
                        lastLineToken.columnPosition = columnPosition;
                        return true;
                    }
                    else{
                        end += 1;
                        isComment = false;
                        tempToken.tokenWord = line.substr(start,(end - start));
                        tempToken.tokenId = zhy::tiny_token_id::COMMENT;
                        tokenDeque.emplace_back(tempToken);
                        tempToken.clear();
                        i = end - 1;
                    }
                }
                else if (zhy::tiny_token::isIdStart(line[i])){//处理标识符、关键字
                    size_t start = i;
                    size_t end = line.size();
                    for(size_t j=(start + 1);j<line.size();j++){
                        if (!zhy::tiny_token::isIdContinue(line[j])){
                            end = j;
                            break;
                        }
                    }
                    tempToken.tokenWord = line.substr(start,(end - start));
                    if (zhy::tiny_token::isKeyword(tempToken.tokenWord)) tempToken.tokenId = zhy::tiny_token::keyword_id_map.find(tempToken.tokenWord)->second;
                    else tempToken.tokenId = zhy::tiny_token_id::ID;
                    tokenDeque.emplace_back(tempToken);
                    tempToken.clear();
                    i = end - 1;
                }
                else if (zhy::tiny_token::isOperatorStart(line[i])){//处理运算符
                    std::string op1 = line.substr(i,1);
                    std::string op2 = line.substr(i,2);
                    std::string op3 = line.substr(i,3);
                    if (line.size() >= (i + 3) && zhy::tiny_token::isOperator(op3)){
                        tempToken.tokenWord = op3;
                        tempToken.tokenId = zhy::tiny_token::operator_id_map.find(tempToken.tokenWord)->second;
                        i += 2;
                    }
                    else if (line.size() >= (i + 2) && zhy::tiny_token::isOperator(op2)){
                        tempToken.tokenWord = op2;
                        tempToken.tokenId = zhy::tiny_token::operator_id_map.find(tempToken.tokenWord)->second;
                        i += 1;
                    }
                    else if (zhy::tiny_token::isOperator(op1)){
                        tempToken.tokenWord = op1;
                        tempToken.tokenId = zhy::tiny_token::operator_id_map.find(tempToken.tokenWord)->second;
                    }
                    else{
                        tempToken.tokenWord = op1;
                        tempToken.tokenId = zhy::tiny_token_id::ERROR;
                        addError(0,linePosition,columnPosition);
                    }
                    tokenDeque.emplace_back(tempToken);
                    tempToken.clear();
                }
                else{
                    tempToken.tokenWord = std::string(1,line[i]);
                    tempToken.tokenId = zhy::tiny_token_id::ERROR;
                    tokenDeque.emplace_back(tempToken);
                    tempToken.clear();
                    addError(0,linePosition,columnPosition);
                }
            }
            lastLineToken = tokenDeque.back();
            return true;
        }

        bool getLineTokens(){
            if (!in.is_open()) return false;
            std::string line;
            if (!std::getline(in,line)){
                if (isComment) addError(1,std::get <0>(commentStartPosition),std::get <1>(commentStartPosition));
                return false;
            }
            linePosition += 1;
            zhy::tiny_tokens::trimLine(line);
            if (line.empty()) return true;
            getLineToken(line);
            return true;
        }

        public:
        tiny_tokens(const std::string & fp,std::ostream & o)
            :filePath(fp),out(o),
            lastLineToken(),
            commentStartPosition(0,0)
        {
            in.open(std::filesystem::path(filePath),std::ios::in);
            tokenDeque.clear();
            tokenErrors.clear();
            linePosition = 0;
            columnPosition = 0;
            isComment = false;
        }
        ~tiny_tokens(){
            if (in.is_open()) in.close();
        }

        void restart(){
            in.clear();
            in.seekg(0);
            tokenDeque.clear();
            tokenErrors.clear();
            linePosition = 0;
            columnPosition = 0;
            isComment = false;
            lastLineToken.clear();
            commentStartPosition = std::make_tuple(0,0);
            return;
        }

        bool isOpen() const{
            return in.is_open();
        }

        static void trimLine(std::string & line){//去除行首尾空字符
            line.erase(0,line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            return;
        }

        bool getNextToken(zhy::tiny_token & nextToken){
            //nextToken.clear();
            while (tokenDeque.empty()){
                if (!getLineTokens()) break;
            }
            if (tokenDeque.empty()) return false;
            nextToken = tokenDeque.front();
            tokenDeque.pop_front();
            return true;
        }

        bool getNextTokenWithoutComment(zhy::tiny_token & nextToken){
            //nextToken.clear();
            if (!getNextToken(nextToken)) return false;
            while (nextToken.empty() || nextToken.tokenId == zhy::tiny_token_id::COMMENT){
                if (!getNextToken(nextToken)) break;
            }
            if (nextToken.empty() || nextToken.tokenId == zhy::tiny_token_id::COMMENT) return false;
            //nextToken.display();
            return true;
        }

        bool displayAllTokens(std::ostream & err = std::cerr){
            try{
                zhy::tiny_token nextToken;
                while (getNextToken(nextToken)){
                    nextToken.display(out);
                }
            }
            catch (const std::exception & e){
                err<<"在进行tiny语言的词法分析时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                err<<e.what()<<std::endl;
                return false;
            }
            return true;
        }

        bool getAllTokensTable(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr){
            table.clear();
            try{
                zhy::tiny_token nextToken;
                while (getNextToken(nextToken)){
                    table.emplace_back(std::vector <std::string>());
                    nextToken.getTable(table.size(),table[table.size() - 1]);
                }
            }
            catch (const std::exception & e){
                err<<"在进行tiny语言的词法分析时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                err<<e.what()<<std::endl;
                return false;
            }
            return true;
        }

        void displayAllErrors() const{
            for(const zhy::tiny_error & error : tokenErrors){
                error.display(out);
            }
            return;
        }

        void getAllErrorsTable(std::vector <std::vector <std::string>> & table) const{
            table.resize(tokenErrors.size());
            for(size_t i=0;i<tokenErrors.size();i++){
                tokenErrors[i].getTable(i + 1,table[i]);
            }
            return;
        }

        const std::string & getFilePath() const{
            return filePath;
        }

        size_t getLinePosition() const{
            return linePosition;
        }

        size_t getColumnPosition() const{
            return columnPosition;
        }

        bool noTokens() const{
            return tokenDeque.empty();
        }

        std::ostream & getOutStream(){
            return out;
        }

        size_t getErrorNum() const{
            return tokenErrors.size();
        }

    };

/*
program -> stmt-sequence
stmt-sequence -> stmt-sequence statement | statement
statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt | for-stmt | while-stmt | assign_regexp-stmt | unary-stmt
if-stmt -> if exp then stmt-sequence endif | if exp then stmt-sequence else stmt-sequence endif
repeat-stmt -> repeat stmt-sequence until exp ;
assign-stmt -> identifier := exp ;
read-stmt -> read identifier ;
write-stmt -> write exp ;
for-stmt -> for ( assign-stmt exp ; unary-stmt ) stmt-sequence endfor
unary-stmt -> unary-exp ;
unary-exp -> unary-op identifier
unary-op -> ++ | --
while-stmt -> while ( exp ) stmt-sequence endwhile
exp -> simple-exp comparison-op simple-exp | simple-exp
comparison-op -> < | = | > | <= | >= | <>
simple-exp -> simple-exp addop term | term 
addop -> + | -
term -> term mulop exponent | exponent
mulop -> * | / | %
exponent -> factor powop exponent | factor
powop -> ^
factor -> ( exp ) | number | identifier | unary-exp
assign_regexp-stmt -> identifier ::= regexp ;
regexp -> regexp select-op connect-exp | connect-exp
select-op -> \|
connect-exp -> connect-exp connect-op regunary-exp | regunary-exp
connect-op -> &
regunary-exp -> atom-exp regunary-op | atom-exp
regunary-op -> # | ?
atom-exp -> ( regexp ) | identifier
*/

    class tiny_program{

        public:
        class sequence_tree;
        class statement_tree;
        class if_tree;
        class repeat_tree;
        class assign_tree;
        class read_tree;
        class write_tree;
        class for_tree;
        class while_tree;
        class unary_tree;
        class unary_statement_tree;
        class expression_tree;
        class simple_expression_tree;
        class term_tree;
        class exponent_tree;
        class factor_tree;
        class assign_regexp_tree;
        class regexp_tree;
        class connect_exp_tree;
        class regunary_exp_tree;
        class atom_tree;

        class sequence_tree{
            public:

            inline static const std::string nodeName = "start";
            std::vector <std::shared_ptr <zhy::tiny_program::statement_tree>> statements;

            sequence_tree():statements(){}
            ~sequence_tree(){}

            const std::string & getName() const{
                return nodeName;
            }

            void addStatement(const std::shared_ptr <zhy::tiny_program::statement_tree> & statement){
                statements.emplace_back(statement);
                return;
            }

            void display(bool outputName,std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (outputName){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    for(const auto & statement : statements){
                        if (statement) statement->display(out,newTab);
                    }
                }
                else{
                    for(const auto & statement : statements){
                        if (statement) statement->display(out,tab);
                    }
                }
                return;
            }

        };

        class statement_tree{
            public:

            unsigned short int id;//0: if 1: repeat 2: assign 3: read 4: write 5: for 6: while 7: regexp

            statement_tree():id(8){}
            virtual ~statement_tree() = default;

            virtual std::string getName() const = 0;

            virtual void display(std::ostream & out = std::cout,const std::string & tab = "    ") const = 0;
        };

        class if_tree : public statement_tree{
            public:

            bool ifId;//0: if exp then stmt-sequence endif ; 1: if exp then stmt-sequence else stmt-sequence endif ;
            inline static const std::string nodeName = "if";
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;
            std::shared_ptr <zhy::tiny_program::sequence_tree> then;
            std::shared_ptr <zhy::tiny_program::sequence_tree> _else;

            if_tree()
                :statement_tree(),
                ifId(0),
                expression(nullptr),
                then(nullptr),
                _else(nullptr)
            {
                id = 0;
            }
            if_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp,
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & t,
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & e
            )
                :statement_tree(),
                ifId(i),
                expression(exp),
                then(t),
                _else(e)
            {
                id = 0;
            }
            if_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp,
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & t
            )
                :statement_tree(),
                ifId(i),
                expression(exp),
                then(t),
                _else(nullptr)
            {
                id = 0;
            }
            ~if_tree() override{}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                std::string newTab = tab + "    ";
                if (expression) expression->display(out,newTab);
                if (then) then->display(false,out,newTab);
                if (ifId && _else) _else->display(true,out,newTab);
                return;
            }

        };

        class repeat_tree : public statement_tree{
            public:

            inline static const std::string nodeName = "repeat"; 
            std::shared_ptr <zhy::tiny_program::sequence_tree> repeat;
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;

            repeat_tree():
                statement_tree(),
                repeat(nullptr),
                expression(nullptr)
            {
                id = 1;
            }
            repeat_tree(
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & re,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp
            ):
                statement_tree(),
                repeat(re),
                expression(exp)
            {
                id = 1;
            }
            ~repeat_tree() override{}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                std::string newTab = tab + "    ";
                if (repeat) repeat->display(false,out,newTab);
                if (expression) expression->display(out,newTab);
                return;
            }

        };

        class assign_tree : public statement_tree{
            public:

            std::string identifier;
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;

            assign_tree():
                statement_tree(),
                identifier(),
                expression(nullptr)
            {
                id = 2;
            }
            assign_tree(
                const std::string & iden,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp
            ):
                statement_tree(),
                identifier(iden),
                expression(exp)
            {
                id = 2;
            }
            ~assign_tree() override{}

            std::string getName() const override{
                return "assign(" + identifier + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                if (expression) expression->display(out,tab + "    ");
                return;
            }
            
        };

        class read_tree : public statement_tree{
            public:

            std::string identifier;

            read_tree():identifier(){}
            read_tree(const std::string & i):statement_tree(),identifier(i){
                id = 3;
            }
            ~read_tree() override{}

            std::string getName() const override{
                return "read(" + identifier + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                return;
            }
            
        };

        class write_tree : public statement_tree{
            public:

            inline static const std::string nodeName = "write";
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;

            write_tree():statement_tree(),expression(nullptr){
                id = 4;
            }
            write_tree(const std::shared_ptr <zhy::tiny_program::expression_tree> & exp):statement_tree(),expression(exp){
                id = 4;
            }
            ~write_tree() override{}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                if (expression) expression->display(out,tab + "    ");
                return;
            }
            
        };

        class for_tree : public statement_tree{
            public:

            inline static const std::string nodeName = "for";
            std::shared_ptr <zhy::tiny_program::assign_tree> assign;
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;
            std::shared_ptr <zhy::tiny_program::unary_statement_tree> unary;
            std::shared_ptr <zhy::tiny_program::sequence_tree> repeat;

            for_tree():
                statement_tree(),
                assign(nullptr),
                expression(nullptr),
                unary(nullptr),
                repeat(nullptr)
            {
                id = 5;
            }
            for_tree(
                const std::shared_ptr <zhy::tiny_program::assign_tree> & ass,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp,
                const std::shared_ptr <zhy::tiny_program::unary_statement_tree> & u,
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & r
            ):
                statement_tree(),
                assign(ass),
                expression(exp),
                unary(u),
                repeat(r)
            {
                id = 5;
            }
            ~for_tree() override{}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                std::string newTab = tab + "    ";
                if (assign) assign->display(out,newTab);
                if (expression) expression->display(out,newTab);
                if (unary) unary->display(out,newTab);
                if (repeat) repeat->display(true,out,newTab);
                return;
            }
            
        };

        class while_tree : public statement_tree{
            public:

            inline static const std::string nodeName = "while";
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;
            std::shared_ptr <zhy::tiny_program::sequence_tree> repeat;

            while_tree():
                statement_tree(),
                expression(nullptr),
                repeat(nullptr)
            {
                id = 6;
            }
            while_tree(
                const std::shared_ptr <zhy::tiny_program::expression_tree> & exp,
                const std::shared_ptr <zhy::tiny_program::sequence_tree> & r
            ):
                statement_tree(),
                expression(exp),
                repeat(r)
            {
                id = 6;
            }
            ~while_tree() override{}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                std::string newTab = tab + "    ";
                if (expression) expression->display(out,newTab);
                if (repeat) repeat->display(true,out,newTab);
                return;
            }
            
        };

        class unary_tree{
            public:

            std::string unaryOperator;
            std::string identifier;

            unary_tree():unaryOperator(),identifier(){}
            unary_tree(const std::string & u,const std::string & i):unaryOperator(u),identifier(i){}
            ~unary_tree(){}

            std::string getName() const{
                return unaryOperator + identifier;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                out<<tab<<getName()<<std::endl;
                return;
            }

        };

        class unary_statement_tree : public statement_tree{
            public:

            inline static const std::string nodeName = "unary";
            std::shared_ptr <zhy::tiny_program::unary_tree> unary;

            unary_statement_tree(const std::shared_ptr <zhy::tiny_program::unary_tree> & u):unary(u){}
            ~unary_statement_tree(){}

            std::string getName() const override{
                return nodeName;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                if (unary) unary->display(out,tab + "    ");
                return;
            }

        };

        class expression_tree{
            public:

            bool id;//0: simple-exp comparison-op simple-exp 1: simple-exp
            std::string comparisonOperator;
            std::shared_ptr <zhy::tiny_program::simple_expression_tree> left;
            std::shared_ptr <zhy::tiny_program::simple_expression_tree> right;

            expression_tree():
                id(0),
                comparisonOperator(),
                left(nullptr),
                right(nullptr)
            {}
            expression_tree(bool i):
                id(i),
                comparisonOperator(),
                left(nullptr),
                right(nullptr)
            {}
            expression_tree(bool i,const std::string & co):
                id(i),
                comparisonOperator(co),
                left(nullptr),
                right(nullptr)
            {}
            expression_tree(
                bool i,const std::string & co,
                const std::shared_ptr <zhy::tiny_program::simple_expression_tree> & ptr
            ):
                id(i),
                comparisonOperator(co)
            {
                if (!id){
                    left = nullptr;
                    right = nullptr;
                }
                else{
                    left = ptr;
                    right = nullptr;
                }
            }
            expression_tree(
                bool i,const std::string & co,
                const std::shared_ptr <zhy::tiny_program::simple_expression_tree> & l,
                const std::shared_ptr <zhy::tiny_program::simple_expression_tree> & r
            ):
                id(i),
                comparisonOperator(co),
                left(l),
                right(r)
            {}
            expression_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::simple_expression_tree> & l
            ):
                id(i),
                comparisonOperator(),
                left(l),
                right(nullptr)
            {}
            ~expression_tree(){}

            const std::string & getName() const{
                return comparisonOperator;
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (left) left->display(out,newTab);
                    if (right) right->display(out,newTab);
                }
                else if (left) left->display(out,tab);
                return;
            }

        };

        class simple_expression_tree{
            public:

            bool id;//0: simple-exp addop term 1: term
            std::string addOperator;
            std::shared_ptr <zhy::tiny_program::simple_expression_tree> simple;
            std::shared_ptr <zhy::tiny_program::term_tree> term;

            simple_expression_tree():
                id(0),
                addOperator(),
                simple(nullptr),
                term(nullptr)
            {}
            simple_expression_tree(bool i):
                id(i),
                addOperator(),
                simple(nullptr),
                term(nullptr)
            {}
            simple_expression_tree(bool i,const std::string & ap):
                id(i),
                addOperator(ap),
                simple(nullptr),
                term(nullptr)
            {}
            simple_expression_tree(
                bool i,const std::string & ap,
                const std::shared_ptr <zhy::tiny_program::simple_expression_tree> & s,
                const std::shared_ptr <zhy::tiny_program::term_tree> & t
            ):
                id(i),
                addOperator(ap)
            {
                if (!id){
                    simple = s;
                    term = t;
                }
                else{
                    simple = nullptr;
                    term = t;
                }
            }
            simple_expression_tree(
                bool i,const std::string & ap,
                const std::shared_ptr <zhy::tiny_program::term_tree> & t
            ):
                id(i),
                addOperator(ap),
                simple(nullptr),
                term(t)
            {}
            simple_expression_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::term_tree> & t
            ):
                id(i),
                addOperator(),
                simple(nullptr),
                term(t)
            {}
            ~simple_expression_tree(){}

            std::string getName() const{
                return "addOp(" + addOperator + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (simple) simple->display(out,newTab);
                    if (term) term->display(out,newTab);
                }
                else{
                    if (term) term->display(out,tab);
                }
                return;
            }

        };

        class term_tree{
            public:

            bool id;//0: term mulop exponent 1: exponent
            std::string mulOperator;
            std::shared_ptr <zhy::tiny_program::term_tree> term;
            std::shared_ptr <zhy::tiny_program::exponent_tree> exponent;

            term_tree():
                id(0),
                mulOperator(),
                term(nullptr),
                exponent(nullptr)
            {}
            term_tree(bool i):
                id(i),
                mulOperator(),
                term(nullptr),
                exponent(nullptr)
            {}
            term_tree(bool i,const std::string & mo):
                id(i),
                mulOperator(mo),
                term(nullptr),
                exponent(nullptr)
            {}
            term_tree(
                bool i,const std::string & mo,
                const std::shared_ptr <zhy::tiny_program::term_tree> & t,
                const std::shared_ptr <zhy::tiny_program::exponent_tree> & e
            ):
                id(i),
                mulOperator(mo)
            {
                if (!id){
                    term = t;
                    exponent = e;
                }
                else{
                    term = nullptr;
                    exponent = e;
                }
            }
            term_tree(
                bool i,const std::string & mo,
                const std::shared_ptr <zhy::tiny_program::exponent_tree> & e
            ):
                id(i),
                mulOperator(mo),
                term(nullptr),
                exponent(e)
            {}
            term_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::exponent_tree> & e
            ):
                id(i),
                mulOperator(),
                term(nullptr),
                exponent(e)
            {}
            ~term_tree(){}

            std::string getName() const{
                return "mulOp(" + mulOperator + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (term) term->display(out,newTab);
                    if (exponent) exponent->display(out,newTab);
                }
                else{
                    if (exponent) exponent->display(out,tab);
                }
                return;
            }

        };

        class exponent_tree{
            public:

            bool id;//0: factor powop exponent 1: factor
            std::shared_ptr <zhy::tiny_program::factor_tree> base;
            std::shared_ptr <zhy::tiny_program::exponent_tree> exponent;
            inline static const std::string nodeName = "powOp(^)";

            exponent_tree():
                id(0),
                base(nullptr),
                exponent(nullptr)
            {}
            exponent_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::factor_tree> & b,
                const std::shared_ptr <zhy::tiny_program::exponent_tree> & e
            ):id(i){
                if (!id){
                    base = b,
                    exponent = e;
                }
                else{
                    base = nullptr;
                    exponent = e;
                }
            }
            exponent_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::factor_tree> & e
            ):id(i),base(e),exponent(nullptr){}
            ~exponent_tree(){}

            const std::string & getName() const{
                return nodeName;
            }
            
            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (base) base->display(out,newTab);
                    if (exponent) exponent->display(out,newTab);
                }
                else{
                    if (base) base->display(out,tab);
                }
                return;
            }

        };

        class factor_tree{
            public:

            unsigned short int id;//0: (exp) 1: 数字 2:标识符 3: 单目运算
            std::shared_ptr <zhy::tiny_program::expression_tree> expression;
            size_t number;
            std::string identifier;
            std::shared_ptr <zhy::tiny_program::unary_tree> unary;

            factor_tree(
                unsigned short int i,
                const std::shared_ptr <zhy::tiny_program::expression_tree> & e
            ):id(i),expression(e),number(0),identifier(),unary(nullptr){}
            factor_tree(
                unsigned short int i,
                size_t n
            ):id(i),expression(nullptr),number(n),identifier(),unary(nullptr){}
            factor_tree(
                unsigned short int i,
                const std::string & iden
            ):id(i),expression(nullptr),number(0),identifier(iden),unary(nullptr){}
            factor_tree(
                unsigned short int i,
                const std::shared_ptr <zhy::tiny_program::unary_tree> & u
            ):id(i),expression(nullptr),number(0),identifier(),unary(u){}

            std::string getName() const{
                switch (id){
                    case 0:return "exp(" + expression->getName() + ")";
                    case 1:return "const(" + std::to_string(number) + ")";
                    case 2:return "id(" + identifier + ")";
                    case 3:return "unary(" + unary->getName() + ")";
                    default:return "";
                }
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                switch (id){
                    case 0:{if (expression) expression->display(out,tab);return;}
                    case 1:{out<<tab<<getName()<<std::endl;return;}
                    case 2:{out<<tab<<getName()<<std::endl;return;}
                    case 3:{if (unary) unary->display(out,tab);return;}
                    default:return;
                }
            }

        };

        class assign_regexp_tree : public statement_tree{
            public:

            std::string identifier;
            std::shared_ptr <zhy::tiny_program::regexp_tree> regexp;

            assign_regexp_tree(
                const std::string & id,
                const std::shared_ptr <zhy::tiny_program::regexp_tree> & r
            ):identifier(id),regexp(r){}
            ~assign_regexp_tree(){}

            std::string getName() const override{
                return "assign(" + identifier + ")";
            }
            
            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const override{
                out<<tab<<getName()<<std::endl;
                if (regexp) regexp->display(out,tab + "    ");
                return;
            }

        };

        class regexp_tree{
            public:

            bool id;//0: regexp select-op connect-exp 1: connect-exp
            std::string nodeName = "selectOp(|)";
            std::shared_ptr <zhy::tiny_program::regexp_tree> regexp;
            std::shared_ptr <zhy::tiny_program::connect_exp_tree> connect;

            regexp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::regexp_tree> & r,
                const std::shared_ptr <zhy::tiny_program::connect_exp_tree> & c
            ):
                id(i),
                regexp(r),
                connect(c)
            {}
            regexp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::connect_exp_tree> & c
            ):
                id(i),
                regexp(nullptr),
                connect(c)
            {}
            ~regexp_tree(){}

            std::string getName() const{
                if (!id) return nodeName;
                else return "connect(" + connect->getName() + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (regexp) regexp->display(out,newTab);
                    if (connect) connect->display(out,newTab);
                }
                else{
                    if (connect) connect->display(out,tab);
                }
                return;
            }

        };

        class connect_exp_tree{
            public:

            bool id;//0: connect-exp connect-op regunary-exp 1: regunary-exp
            std::string nodeName = "connectOp(&)";
            std::shared_ptr <zhy::tiny_program::connect_exp_tree> connect;
            std::shared_ptr <zhy::tiny_program::regunary_exp_tree> regunary;

            connect_exp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::connect_exp_tree> & c,
                const std::shared_ptr <zhy::tiny_program::regunary_exp_tree> & ru
            ):
                id(i),
                connect(c),
                regunary(ru)
            {}
            connect_exp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::regunary_exp_tree> & ru
            ):
                id(i),
                connect(nullptr),
                regunary(ru)
            {}
            ~connect_exp_tree(){}

            std::string getName() const{
                if (!id) return nodeName;
                else return "unary(" + regunary->getName() + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if  (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (connect) connect->display(out,newTab);
                    if (regunary) regunary->display(out,newTab);
                }
                else{
                    if (regunary) regunary->display(out,tab);
                }
                return;
            }

        };

        class regunary_exp_tree{
            public:

            bool id;//0: atom-exp regunary-op 1: atom-exp
            std::shared_ptr <zhy::tiny_program::atom_tree> atom;
            std::string unaryOperator;

            regunary_exp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::atom_tree> & a,
                const std::string & ruOp
            ):
                id(i),
                atom(a),
                unaryOperator(ruOp)
            {}
            regunary_exp_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::atom_tree> & a
            ):
                id(i),
                atom(a),
                unaryOperator()
            {}
            ~regunary_exp_tree(){}

            std::string getName() const{
                if (!id) return "regunaryOp(" + unaryOperator + ")";
                else return "atom(" + atom->getName() + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    out<<tab<<getName()<<std::endl;
                    std::string newTab = tab + "    ";
                    if (atom) atom->display(out,newTab);
                    out<<newTab<<unaryOperator<<std::endl;
                }
                else{
                    if (atom) atom->display(out,tab);
                }
                return;
            }

        };

        class atom_tree{
            public:

            bool id;//0: ( regexp ) 1: identifier
            std::shared_ptr <zhy::tiny_program::regexp_tree> regexp;
            std::string identifier;

            atom_tree(
                bool i,
                const std::shared_ptr <zhy::tiny_program::regexp_tree> & rexp
            ):
                id(i),
                regexp(rexp),
                identifier()
            {}
            atom_tree(
                bool i,
                const std::string & iden
            ):
                id(i),
                regexp(nullptr),
                identifier(iden)
            {}
            ~atom_tree(){}

            std::string getName() const{
                if (!id) return "exp(" + regexp->getName() + ")";
                else return "id(" + identifier + ")";
            }

            void display(std::ostream & out = std::cout,const std::string & tab = "    ") const{
                if (!id){
                    if (regexp) regexp->display(out,tab);
                }
                else out<<tab<<getName()<<std::endl;
                return;
            }

        };

        private:

        tiny_program(const zhy::tiny_program & other) = delete;
        zhy::tiny_program & operator =(const zhy::tiny_program & other) = delete;

        std::shared_ptr <zhy::tiny_program::sequence_tree> program;
        zhy::tiny_tokens tokenGetter;
        zhy::tiny_token tempToken;
        std::vector <zhy::tiny_error> syntaxErrors;

        bool getToken(){
            return tokenGetter.getNextTokenWithoutComment(tempToken);
        }

        inline static const std::unordered_map <zhy::tiny_token_id,size_t> syntax_token_id_error_map = {
            {zhy::tiny_token_id::KW_IF,1},
            {zhy::tiny_token_id::KW_THEN,2},
            {zhy::tiny_token_id::KW_ELSE,3},
            {zhy::tiny_token_id::KW_ENDIF,4},
            {zhy::tiny_token_id::KW_REPEAT,5},
            {zhy::tiny_token_id::KW_UNTIL,6},
            {zhy::tiny_token_id::OP_ASSIGNMENT,7},
            {zhy::tiny_token_id::KW_READ,8},
            {zhy::tiny_token_id::KW_WRITE,9},
            {zhy::tiny_token_id::KW_FOR,10},
            {zhy::tiny_token_id::OP_BRACKETLEFT,11},
            {zhy::tiny_token_id::OP_BRACKETRIGHT,12},
            {zhy::tiny_token_id::OP_SEMICOLON,13},
            {zhy::tiny_token_id::KW_WHILE,14},
            {zhy::tiny_token_id::KW_ENDWHILE,15},
            {zhy::tiny_token_id::OP_ASSIGNMENTREGEXP,16},

            {zhy::tiny_token_id::LT_UNSIGNEDINTEGER,18},
            {zhy::tiny_token_id::ID,19},
            {zhy::tiny_token_id::OP_INCREMENT,21},
            {zhy::tiny_token_id::OP_DECREMENT,22},
            {zhy::tiny_token_id::OP_POWER,23},
            {zhy::tiny_token_id::OP_MULTIPLICATION,24},
            {zhy::tiny_token_id::OP_DIVISION,25},
            {zhy::tiny_token_id::OP_MODULUS,26},
            {zhy::tiny_token_id::OP_ADDITION,27},
            {zhy::tiny_token_id::OP_SUBTRACTION,28},
            {zhy::tiny_token_id::OP_EQUAL,29},
            {zhy::tiny_token_id::OP_LESS,29},
            {zhy::tiny_token_id::OP_OVER,29},
            {zhy::tiny_token_id::OP_LESSEQUAL,29},
            {zhy::tiny_token_id::OP_OVEREQUAL,29},
            {zhy::tiny_token_id::OP_NOTEQUAL,29},
            {zhy::tiny_token_id::KW_ENDFOR,30},
            {zhy::tiny_token_id::REGOP_CLOSURE,31},
            {zhy::tiny_token_id::REGOP_OPTIONAL,32},
            {zhy::tiny_token_id::REGOP_CONNECT,33},
            {zhy::tiny_token_id::REGOP_OR,34},
        };

        bool error(size_t errorPosition){
            syntaxErrors.emplace_back(zhy::tiny_error(
                zhy::tiny_error::tiny_syntax_error_description_map.at(errorPosition),
                tokenGetter.getFilePath(),
                tempToken.linePosition,
                tempToken.columnPosition
            ));
            return getToken();
        }

        bool match(zhy::tiny_token_id tokenId/*,bool & matchSucc*/){
            if (tempToken.tokenId == tokenId){
                //matchSucc = true;
                return getToken();
            }
            else{
                //matchSucc = false;
                size_t errorPosition = 0;
                auto errorIt = syntax_token_id_error_map.find(tokenId);
                if (errorIt != syntax_token_id_error_map.end()) errorPosition = errorIt->second;
                return error(errorPosition);
            }
        }

        std::shared_ptr <zhy::tiny_program::unary_tree> unary(){
            std::shared_ptr <zhy::tiny_program::unary_tree> temp = nullptr;
            switch (tempToken.tokenId){
                case zhy::tiny_token_id::OP_INCREMENT:{
                    std::string unaryOp = tempToken.getValue <std::string>();
                    match(zhy::tiny_token_id::OP_INCREMENT);
                    std::string iden = tempToken.getValue <std::string>();
                    match(zhy::tiny_token_id::ID);
                    temp = std::make_shared <zhy::tiny_program::unary_tree>(unaryOp,iden);
                    break;
                }
                case zhy::tiny_token_id::OP_DECREMENT:{
                    std::string unaryOp = tempToken.getValue <std::string>();
                    match(zhy::tiny_token_id::OP_DECREMENT);
                    std::string iden = tempToken.getValue <std::string>();
                    match(zhy::tiny_token_id::ID);
                    temp = std::make_shared <zhy::tiny_program::unary_tree>(unaryOp,iden);
                    break;
                }
                default:error(20);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::unary_statement_tree> unaryStatement(){
            std::shared_ptr <zhy::tiny_program::unary_tree> u = unary();
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return std::make_shared <zhy::tiny_program::unary_statement_tree>(u);
        }

        std::shared_ptr <zhy::tiny_program::factor_tree> factor(){
            std::shared_ptr <zhy::tiny_program::factor_tree> temp = nullptr;
            switch (tempToken.tokenId){
                case zhy::tiny_token_id::OP_BRACKETLEFT:{
                    match(zhy::tiny_token_id::OP_BRACKETLEFT);
                    std::shared_ptr <zhy::tiny_program::expression_tree> e = expression();
                    match(zhy::tiny_token_id::OP_BRACKETRIGHT);
                    temp = std::make_shared <zhy::tiny_program::factor_tree>(0,e);
                    break;
                }
                case zhy::tiny_token_id::LT_UNSIGNEDINTEGER:{
                    temp = std::make_shared <zhy::tiny_program::factor_tree>(
                        1,
                        tempToken.getValue <size_t>()
                    );
                    match(zhy::tiny_token_id::LT_UNSIGNEDINTEGER);
                    break;
                }
                case zhy::tiny_token_id::ID:{
                    temp = std::make_shared <zhy::tiny_program::factor_tree>(
                        2,
                        tempToken.getValue <std::string>()
                    );
                    match(zhy::tiny_token_id::ID);
                    break;
                }
                case zhy::tiny_token_id::OP_INCREMENT:{
                    std::shared_ptr <zhy::tiny_program::unary_tree> u = unary();
                    temp = std::make_shared <zhy::tiny_program::factor_tree>(3,u);
                    break;
                }
                case zhy::tiny_token_id::OP_DECREMENT:{
                    std::shared_ptr <zhy::tiny_program::unary_tree> u = unary();
                    temp = std::make_shared <zhy::tiny_program::factor_tree>(3,u);
                    break;
                }
                default:error(17);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::exponent_tree> exponent(){
            std::shared_ptr <zhy::tiny_program::exponent_tree> temp = nullptr;
            std::shared_ptr <zhy::tiny_program::factor_tree> tempBase = factor();
            if (tempToken.tokenId == zhy::tiny_token_id::OP_POWER){
                match(zhy::tiny_token_id::OP_POWER);
                std::shared_ptr <zhy::tiny_program::exponent_tree> tempExponent = exponent();
                temp = std::make_shared <zhy::tiny_program::exponent_tree>(0,tempBase,tempExponent);
            }
            else temp = std::make_shared <zhy::tiny_program::exponent_tree>(1,tempBase);
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::term_tree> term(){
            std::shared_ptr <zhy::tiny_program::exponent_tree> tempExpo = exponent();
            std::shared_ptr <zhy::tiny_program::term_tree> temp = std::make_shared <zhy::tiny_program::term_tree>(1,tempExpo);
            while (tempToken.isMulOperator()){
                std::string mulOp = tempToken.getValue <std::string>();
                match(tempToken.tokenId);
                tempExpo = exponent();
                std::shared_ptr <zhy::tiny_program::term_tree> newTemp = std::make_shared <zhy::tiny_program::term_tree>(0,mulOp,temp,tempExpo);
                std::swap(temp,newTemp);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::simple_expression_tree> simple(){
            std::shared_ptr <zhy::tiny_program::term_tree> tempTerm = term();
            std::shared_ptr <zhy::tiny_program::simple_expression_tree> temp = std::make_shared <zhy::tiny_program::simple_expression_tree>(1,tempTerm);
            while (tempToken.isAddOperator()){
                std::string addOp = tempToken.getValue <std::string>();
                match(tempToken.tokenId);
                tempTerm = term();
                std::shared_ptr <zhy::tiny_program::simple_expression_tree> newTemp = std::make_shared <zhy::tiny_program::simple_expression_tree>(0,addOp,temp,tempTerm);
                std::swap(temp,newTemp);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::expression_tree> expression(){
            std::shared_ptr <zhy::tiny_program::simple_expression_tree> left = simple();
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = nullptr;
            if (tempToken.isComparisonOp()){
                std::string comOp = tempToken.getValue <std::string>();
                match(tempToken.tokenId);
                std::shared_ptr <zhy::tiny_program::simple_expression_tree> right = simple();
                exp = std::make_shared <zhy::tiny_program::expression_tree>(0,comOp,left,right);
            }
            else exp = std::make_shared <zhy::tiny_program::expression_tree>(1,left);
            return exp;
        }

        std::shared_ptr <zhy::tiny_program::assign_tree> assign(){
            std::string iden = tempToken.getValue <std::string>();
            match(zhy::tiny_token_id::ID);
            match(zhy::tiny_token_id::OP_ASSIGNMENT);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            std::shared_ptr <zhy::tiny_program::assign_tree> ass = std::make_shared <zhy::tiny_program::assign_tree>(iden,exp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return ass;
        }

        std::shared_ptr <zhy::tiny_program::assign_tree> assign(const std::string & iden){
            match(zhy::tiny_token_id::OP_ASSIGNMENT);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            std::shared_ptr <zhy::tiny_program::assign_tree> ass = std::make_shared <zhy::tiny_program::assign_tree>(iden,exp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return ass;
        }

        std::shared_ptr <zhy::tiny_program::read_tree> read(){
            match(zhy::tiny_token_id::KW_READ);
            std::string iden = tempToken.getValue <std::string>();
            match(zhy::tiny_token_id::ID);
            std::shared_ptr <zhy::tiny_program::read_tree> r = std::make_shared <zhy::tiny_program::read_tree>(iden);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return r;
        }

        std::shared_ptr <zhy::tiny_program::write_tree> write(){
            match(zhy::tiny_token_id::KW_WRITE);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            std::shared_ptr <zhy::tiny_program::write_tree> w = std::make_shared <zhy::tiny_program::write_tree>(exp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return w;
        }

        std::shared_ptr <zhy::tiny_program::if_tree> _if(){
            match(zhy::tiny_token_id::KW_IF);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            match(zhy::tiny_token_id::KW_THEN);
            std::shared_ptr <zhy::tiny_program::sequence_tree> seq = sequence();
            std::shared_ptr <zhy::tiny_program::if_tree> __if = nullptr;
            if (tempToken.tokenId == zhy::tiny_token_id::KW_ENDIF){
                match(zhy::tiny_token_id::KW_ENDIF);
                __if = std::make_shared <zhy::tiny_program::if_tree>(0,exp,seq);
            }
            else{
                match(zhy::tiny_token_id::KW_ELSE);
                std::shared_ptr <zhy::tiny_program::sequence_tree> __else = sequence();
                match(zhy::tiny_token_id::KW_ENDIF);
                __if = std::make_shared <zhy::tiny_program::if_tree>(1,exp,seq,__else);
            }
            return __if;
        }

        std::shared_ptr <zhy::tiny_program::repeat_tree> repeat(){
            match(zhy::tiny_token_id::KW_REPEAT);
            std::shared_ptr <zhy::tiny_program::sequence_tree> seq = sequence();
            match(zhy::tiny_token_id::KW_UNTIL);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            std::shared_ptr <zhy::tiny_program::repeat_tree> re = std::make_shared <zhy::tiny_program::repeat_tree>(seq,exp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return re;
        }

        std::shared_ptr <zhy::tiny_program::for_tree> _for(){
            match(zhy::tiny_token_id::KW_FOR);
            match(zhy::tiny_token_id::OP_BRACKETLEFT);
            std::shared_ptr <zhy::tiny_program::assign_tree> ass = assign();
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            match(zhy::tiny_token_id::OP_SEMICOLON);
            std::shared_ptr <zhy::tiny_program::unary_statement_tree> u = unaryStatement();
            match(zhy::tiny_token_id::OP_BRACKETRIGHT);
            std::shared_ptr <zhy::tiny_program::sequence_tree> seq = sequence();
            match(zhy::tiny_token_id::KW_ENDFOR);
            std::shared_ptr <zhy::tiny_program::for_tree> __for = std::make_shared <zhy::tiny_program::for_tree>(ass,exp,u,seq);
            return __for;
        }

        std::shared_ptr <zhy::tiny_program::while_tree> _while(){
            match(zhy::tiny_token_id::KW_WHILE);
            match(zhy::tiny_token_id::OP_BRACKETLEFT);
            std::shared_ptr <zhy::tiny_program::expression_tree> exp = expression();
            match(zhy::tiny_token_id::OP_BRACKETRIGHT);
            std::shared_ptr <zhy::tiny_program::sequence_tree> seq = sequence();
            match(zhy::tiny_token_id::KW_ENDWHILE);
            std::shared_ptr <zhy::tiny_program::while_tree> __while = std::make_shared <zhy::tiny_program::while_tree>(exp,seq);
            return __while;
        }

        std::shared_ptr <zhy::tiny_program::atom_tree> atom(){
            std::shared_ptr <zhy::tiny_program::atom_tree> temp = nullptr;
            if (tempToken.tokenId == zhy::tiny_token_id::OP_BRACKETLEFT){
                match(zhy::tiny_token_id::OP_BRACKETLEFT);
                std::shared_ptr <zhy::tiny_program::regexp_tree> rexp = regexp();
                match(zhy::tiny_token_id::OP_BRACKETRIGHT);
                temp = std::make_shared <zhy::tiny_program::atom_tree>(0,rexp);
            }
            else{
                std::string iden = tempToken.getValue <std::string>();
                match(zhy::tiny_token_id::ID);
                temp = std::make_shared <zhy::tiny_program::atom_tree>(1,iden);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::regunary_exp_tree> regunary(){
            std::shared_ptr <zhy::tiny_program::regunary_exp_tree> temp = nullptr;
            std::shared_ptr <zhy::tiny_program::atom_tree> a = atom();
            if (tempToken.isRegUnaryOp()){
                std::string ruOp = tempToken.getValue <std::string>();
                match(tempToken.tokenId);
                temp = std::make_shared <zhy::tiny_program::regunary_exp_tree>(0,a,ruOp);
            }
            else temp = std::make_shared <zhy::tiny_program::regunary_exp_tree>(1,a);
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::connect_exp_tree> connect(){
            std::shared_ptr <zhy::tiny_program::regunary_exp_tree> tempRegUnary = regunary();
            std::shared_ptr <zhy::tiny_program::connect_exp_tree> temp = std::make_shared <zhy::tiny_program::connect_exp_tree>(1,tempRegUnary);
            while (tempToken.tokenId == zhy::tiny_token_id::REGOP_CONNECT){
                match(tempToken.tokenId);
                tempRegUnary = regunary();
                std::shared_ptr <zhy::tiny_program::connect_exp_tree> newTemp = std::make_shared <zhy::tiny_program::connect_exp_tree>(0,temp,tempRegUnary);
                std::swap(temp,newTemp);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::regexp_tree> regexp(){
            std::shared_ptr <zhy::tiny_program::connect_exp_tree> tempConnect = connect();
            std::shared_ptr <zhy::tiny_program::regexp_tree> temp = std::make_shared <zhy::tiny_program::regexp_tree>(1,tempConnect);
            while (tempToken.tokenId == zhy::tiny_token_id::REGOP_OR){
                match(tempToken.tokenId);
                tempConnect = connect();
                std::shared_ptr <zhy::tiny_program::regexp_tree> newTemp = std::make_shared <zhy::tiny_program::regexp_tree>(0,temp,tempConnect);
                std::swap(temp,newTemp);
            }
            return temp;
        }

        std::shared_ptr <zhy::tiny_program::assign_regexp_tree> assignRegexp(){
            std::string iden = tempToken.getValue <std::string>();
            match(zhy::tiny_token_id::ID);
            match(zhy::tiny_token_id::OP_ASSIGNMENTREGEXP);
            std::shared_ptr <zhy::tiny_program::regexp_tree> rexp = regexp();
            std::shared_ptr <zhy::tiny_program::assign_regexp_tree> ass = std::make_shared <zhy::tiny_program::assign_regexp_tree>(iden,rexp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return ass;
        }

        std::shared_ptr <zhy::tiny_program::assign_regexp_tree> assignRegexp(const std::string & iden){
            match(zhy::tiny_token_id::OP_ASSIGNMENTREGEXP);
            std::shared_ptr <zhy::tiny_program::regexp_tree> rexp = regexp();
            std::shared_ptr <zhy::tiny_program::assign_regexp_tree> ass = std::make_shared <zhy::tiny_program::assign_regexp_tree>(iden,rexp);
            match(zhy::tiny_token_id::OP_SEMICOLON);
            return ass;
        }

        std::shared_ptr <zhy::tiny_program::statement_tree> statement(){
            switch (tempToken.tokenId){
                case zhy::tiny_token_id::KW_IF:return _if();
                case zhy::tiny_token_id::KW_REPEAT:return repeat();
                case zhy::tiny_token_id::ID:{
                    std::string iden = tempToken.getValue <std::string>();
                    match(zhy::tiny_token_id::ID);
                    if (tempToken.tokenId == zhy::tiny_token_id::OP_ASSIGNMENT) return assign(iden);
                    else if (tempToken.tokenId == zhy::tiny_token_id::OP_ASSIGNMENTREGEXP) return assignRegexp(iden);
                    else{
                        error(35);
                        return nullptr;
                    }
                }
                case zhy::tiny_token_id::KW_READ:return read();
                case zhy::tiny_token_id::KW_WRITE:return write();
                case zhy::tiny_token_id::KW_FOR:return _for();
                case zhy::tiny_token_id::KW_WHILE:return _while();
                case zhy::tiny_token_id::OP_INCREMENT:return unaryStatement();
                case zhy::tiny_token_id::OP_DECREMENT:return unaryStatement();
                default:{
                    error(36);
                    return nullptr;
                }
            }
        }

        std::shared_ptr <zhy::tiny_program::sequence_tree> sequence(){
            std::shared_ptr <zhy::tiny_program::sequence_tree> temp = std::make_shared <zhy::tiny_program::sequence_tree>();
            while (!tempToken.empty() && tempToken.isStatementFirst()){
                std::shared_ptr <zhy::tiny_program::statement_tree> stmt = statement();
                if (stmt != nullptr) temp->addStatement(stmt);
            }
            return temp;
        }



        public:

        tiny_program(const std::string & filePath,std::ostream & out = std::cout):
            program(nullptr),
            tokenGetter(filePath,out),
            tempToken(),
            syntaxErrors()
        {}
        ~tiny_program(){}


        bool isOpen() const{
            return tokenGetter.isOpen();
        }

        const std::string & getFilePath() const{
            return tokenGetter.getFilePath();
        }

        void restart(){
            program = nullptr;
            tokenGetter.restart();
            tempToken.clear();
            syntaxErrors.clear();
            return;
        }

        bool noTokens() const{
            return tokenGetter.noTokens();
        }

        void start(){
            restart();
            getToken();
            //tempToken.display();
            program = sequence();
            return;
        }

        bool displayAllTokens(std::ostream & err = std::cerr){
            tokenGetter.restart();
            return tokenGetter.displayAllTokens(err);
        }

        void displayLexicalErrors() const{
            tokenGetter.displayAllErrors();
            return;
        }

        void displaySyntaxErrors(std::ostream & out = std::cout){
            for(const zhy::tiny_error & error : syntaxErrors){
                error.display(out);
            }
            return;
        }

        bool getAllTokensTable(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr){
            tokenGetter.restart();
            return tokenGetter.getAllTokensTable(table,err);
        }

        void getLexicalErrorsTable(std::vector <std::vector <std::string>> & table) const{
            tokenGetter.getAllErrorsTable(table);
            return;
        }

        void getSyntaxErrorsTable(std::vector <std::vector <std::string>> & table) const{
            table.resize(syntaxErrors.size());
            for(size_t i=0;i<syntaxErrors.size();i++){
                syntaxErrors[i].getTable(i + 1,table[i]);
            }
            return;
        }

        std::shared_ptr <zhy::tiny_program::sequence_tree> getRoot(){
            return program;
        }

        void display(std::ostream & out = std::cout) const{
            program->display(true,out,std::string());
            return;
        }

        size_t getLexicalErrorNum() const{
            return tokenGetter.getErrorNum();
        }

        size_t getSyntaxErrorNum() const{
            return syntaxErrors.size();
        }







    };

    class tiny_tester{
        private:

        tiny_tester() = delete;
        ~tiny_tester() = delete;
        tiny_tester(const tiny_tester & other) = delete;
        tiny_tester & operator =(const tiny_tester & other) = delete;

        public:

        inline static void tiny_lexer_std_test1(){
            zhy::tiny_tokens std_test1("D:\\project\\bianyiyuanli_3\\Backend_test\\std_test1.tny",std::cout);
            std_test1.displayAllTokens(std::cerr);
            std_test1.displayAllErrors();
            return;
        }

        inline static void tiny_lexer_std_test2(){
            zhy::tiny_tokens std_test2("D:\\project\\bianyiyuanli_3\\Backend_test\\std_test2.tny",std::cout);
            std_test2.displayAllTokens(std::cerr);
            std_test2.displayAllErrors();
            std_test2.restart();
            std_test2.displayAllTokens(std::cerr);
            std_test2.displayAllErrors();
            return;
        }

        inline static void tiny_program_std_test1(){
            std::cout<<"std_test1.tny:"<<std::endl;
            zhy::tiny_program std_test1("D:\\project\\bianyiyuanli_3\\Backend_test\\std_test1.tny",std::cout);
            std_test1.start();
            std_test1.display(std::cout);
            std_test1.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_std_test2(){
            std::cout<<"std_test2.tny:"<<std::endl;
            zhy::tiny_program std_test2("D:\\project\\bianyiyuanli_3\\Backend_test\\std_test2.tny",std::cout);
            std_test2.start();
            std_test2.display(std::cout);
            std_test2.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_normal_test1(){
            std::cout<<"normal_test1.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\normal_test1.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_normal_test2(){
            std::cout<<"normal_test2.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\normal_test2.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_normal_test3(){
            std::cout<<"normal_test3.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\normal_test3.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_normal_test4(){
            std::cout<<"normal_test4.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\normal_test4.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_normal_test5(){
            std::cout<<"normal_test5.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\normal_test5.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test1(){
            std::cout<<"error_test1.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test1.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test2(){
            std::cout<<"error_test2.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test2.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test3(){
            std::cout<<"error_test3.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test3.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test4(){
            std::cout<<"error_test4.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test4.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test5(){
            std::cout<<"error_test5.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test5.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test6(){
            std::cout<<"error_test6.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test6.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test7(){
            std::cout<<"error_test7.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test7.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test8(){
            std::cout<<"error_test8.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test8.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test9(){
            std::cout<<"error_test9.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test9.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

        inline static void tiny_program_error_test10(){
            std::cout<<"error_test10.tny:"<<std::endl;
            zhy::tiny_program test("D:\\project\\bianyiyuanli_3\\Backend_test\\error_test10.tny",std::cout);
            test.start();
            test.display(std::cout);
            test.displaySyntaxErrors(std::cout);
            return;
        }

    };


}

#endif