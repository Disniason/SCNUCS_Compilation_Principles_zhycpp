#ifndef Z_TINY_TUPLE_H
#define Z_TINY_TUPLE_H
#pragma once

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <cstddef>

namespace zhy{

    class tiny_token_getter{

        public:

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
            OP_ADDASSIGN,
            OP_SUBASSIGN,

            LT_UNSIGNEDINTEGER,

            COMMENT,

            ID,

            ERROR,

            FUNC_JUMP,
            FUNC_JTRUE,
            FUNC_JFALSE,
            FUNC_EXIT

        };

        inline static const std::unordered_set <char> operator_start = {
                ':','+','-','<','>','*','/',
                '=','(',')',';','%','^',
                '|','&','#','?'
        };

        inline static const std::unordered_map <std::string,tiny_token_id> keyword_id_map = {
            {"if",tiny_token_id::KW_IF},
            {"then",tiny_token_id::KW_THEN},
            {"else",tiny_token_id::KW_ELSE},
            {"endif",tiny_token_id::KW_ENDIF},

            {"repeat",tiny_token_id::KW_REPEAT},
            {"until",tiny_token_id::KW_UNTIL},
            {"read",tiny_token_id::KW_READ},
            {"write",tiny_token_id::KW_WRITE},

            {"while",tiny_token_id::KW_WHILE},
            {"endwhile",tiny_token_id::KW_ENDWHILE},
            {"for",tiny_token_id::KW_FOR},
            {"endfor",tiny_token_id::KW_ENDFOR}
        };

        inline static const std::unordered_map <std::string,tiny_token_id> operator_id_map = {
            
            {":=",tiny_token_id::OP_ASSIGNMENT},
            {"++",tiny_token_id::OP_INCREMENT},
            {"--",tiny_token_id::OP_DECREMENT},
            {"<=",tiny_token_id::OP_LESSEQUAL},
            {">=",tiny_token_id::OP_OVEREQUAL},
            {"<>",tiny_token_id::OP_NOTEQUAL},
            {"+=",tiny_token_id::OP_ADDASSIGN},
            {"-=",tiny_token_id::OP_SUBASSIGN},

            {"+",tiny_token_id::OP_ADDITION},
            {"-",tiny_token_id::OP_SUBTRACTION},
            {"*",tiny_token_id::OP_MULTIPLICATION},
            {"/",tiny_token_id::OP_DIVISION},
            {"=",tiny_token_id::OP_EQUAL},
            {"<",tiny_token_id::OP_LESS},
            {"(",tiny_token_id::OP_BRACKETLEFT},
            {")",tiny_token_id::OP_BRACKETRIGHT},
            {";",tiny_token_id::OP_SEMICOLON},
            {"%",tiny_token_id::OP_MODULUS},
            {"^",tiny_token_id::OP_POWER},
            {">",tiny_token_id::OP_OVER},

        };

        inline static const std::unordered_map <tiny_token_id,std::string> displayStringWord = {
            {tiny_token_id::KW_IF,"关键字(if)"},
            {tiny_token_id::KW_THEN,"关键字(then)"},
            {tiny_token_id::KW_ELSE,"关键字(else)"},
            {tiny_token_id::KW_ENDIF,"关键字(endif)"},

            {tiny_token_id::KW_REPEAT,"关键字(repeat)"},
            {tiny_token_id::KW_UNTIL,"关键字(until)"},
            {tiny_token_id::KW_READ,"关键字(read)"},
            {tiny_token_id::KW_WRITE,"关键字(write)"},

            {tiny_token_id::KW_WHILE,"关键字(while)"},
            {tiny_token_id::KW_ENDWHILE,"关键字(endwhile)"},
            {tiny_token_id::KW_FOR,"关键字(for)"},
            {tiny_token_id::KW_ENDFOR,"关键字(endfor)"},

            {tiny_token_id::OP_ADDITION,"运算符或分隔符(+)"},
            {tiny_token_id::OP_SUBTRACTION,"运算符或分隔符(-)"},
            {tiny_token_id::OP_MULTIPLICATION,"运算符或分隔符(*)"},
            {tiny_token_id::OP_DIVISION,"运算符或分隔符(/)"},
            {tiny_token_id::OP_EQUAL,"运算符或分隔符(=)"},
            {tiny_token_id::OP_LESS,"运算符或分隔符(<)"},
            {tiny_token_id::OP_BRACKETLEFT,"运算符或分隔符(()"},
            {tiny_token_id::OP_BRACKETRIGHT,"运算符或分隔符())"},
            {tiny_token_id::OP_SEMICOLON,"运算符或分隔符(;)"},
            {tiny_token_id::OP_ASSIGNMENT,"运算符或分隔符(:=)"},

            {tiny_token_id::OP_INCREMENT,"运算符或分隔符(++)"},
            {tiny_token_id::OP_DECREMENT,"运算符或分隔符(--)"},
            {tiny_token_id::OP_MODULUS,"运算符或分隔符(%)"},
            {tiny_token_id::OP_POWER,"运算符或分隔符(^)"},
            {tiny_token_id::OP_LESSEQUAL,"运算符或分隔符(<=)"},
            {tiny_token_id::OP_OVER,"运算符或分隔符(>)"},
            {tiny_token_id::OP_OVEREQUAL,"运算符或分隔符(>=)"},
            {tiny_token_id::OP_NOTEQUAL,"运算符或分隔符(<>)"},
            {tiny_token_id::OP_ADDASSIGN,"运算符或分隔符(+=)"},
            {tiny_token_id::OP_SUBASSIGN,"运算符或分隔符(-=)"},

            {tiny_token_id::LT_UNSIGNEDINTEGER,"字面量(正整数)"},

            {tiny_token_id::COMMENT,"注释"},

            {tiny_token_id::ID,"标识符"},

            {tiny_token_id::ERROR,"未知的token"}
        };

        class tiny_token{

            public:

            tiny_token_id tokenId;
            std::variant <std::string,size_t> value;

            tiny_token():tokenId(tiny_token_id::ERROR),value(){}
            ~tiny_token(){}

            tiny_token(const tiny_token & other){
                tokenId = other.tokenId;
                value = other.value;
            }

            tiny_token & operator =(const tiny_token & other){
                if (this != &other){
                    tokenId = other.tokenId;
                    value = other.value;
                }
                return *this;
            }

            void clear(){
                tokenId = tiny_token_id::ERROR;
                value = {};
                return;
            }

            template <class T>
            void setValue(const T & v){
                value = v;
                return;
            }

            template <class T>
            const T & getValue() const{
                static_assert(
                    std::is_same_v<T, std::string> || std::is_same_v<T, size_t>,
                    "tiny_token::value 仅支持 std::string / size_t"
                );
                const T* ptr = std::get_if <T>(&value);
                if (ptr != nullptr) return *ptr;
                static const T default_val{};
                return default_val;
            }

            void setId(){
                if (std::holds_alternative <size_t>(value)){
                    tokenId = tiny_token_id::LT_UNSIGNEDINTEGER;
                    return;
                }
                else if (std::holds_alternative <std::string>(value)){
                    const std::string & tokenWord = std::get <std::string>(value);
                    if (isKeyword(tokenWord)) tokenId = keyword_id_map.at(tokenWord);
                    else if (isOperator(tokenWord)) tokenId = operator_id_map.at(tokenWord);
                    else if (tokenWord[0] == '{' && tokenWord.back() == '}') tokenId = tiny_token_id::COMMENT;
                    else tokenId = tiny_token_id::ID;
                }
                return;
            }

            std::string getWord() const{
                if (tokenId == tiny_token_id::LT_UNSIGNEDINTEGER) return std::to_string(getValue <size_t>());
                else return getValue <std::string>();
            }

            const std::string & getDisplayWord() const{
                auto it = zhy::tiny_token_getter::displayStringWord.find(tokenId);
                if (it == zhy::tiny_token_getter::displayStringWord.end()) return zhy::tiny_token_getter::displayStringWord.at(tiny_token_id::ERROR);
                else return it->second;
            }

            void display(std::ostream & out = std::cout) const{
                auto it = zhy::tiny_token_getter::displayStringWord.find(tokenId);
                if (it == zhy::tiny_token_getter::displayStringWord.end()) return;
                const std::string & displayWord = it->second;
                if (tokenId == tiny_token_id::LT_UNSIGNEDINTEGER) out<<getValue <size_t>();
                else out<<getValue <std::string>();
                out<<" : "<<displayWord<<std::endl;
                return;
            }

            void getTable(std::vector <std::string> & table) const{
                table.resize(2);
                table[0] = getWord();
                table[1] = getDisplayWord();
                return;
            }

            bool isUnaryOperator() const{
                return (
                    tokenId == tiny_token_id::OP_INCREMENT ||
                    tokenId == tiny_token_id::OP_DECREMENT
                );
            }

            bool isAddOperator() const{
                return (
                    tokenId == tiny_token_id::OP_ADDITION ||
                    tokenId == tiny_token_id::OP_SUBTRACTION
                );
            }

            bool isMulOperator() const{
                return (
                    tokenId == tiny_token_id::OP_MULTIPLICATION ||
                    tokenId == tiny_token_id::OP_DIVISION ||
                    tokenId == tiny_token_id::OP_MODULUS
                );
            }

            bool isComparisonOp() const{
                return (
                    tokenId == tiny_token_id::OP_EQUAL ||
                    tokenId == tiny_token_id::OP_LESS ||
                    tokenId == tiny_token_id::OP_OVER ||
                    tokenId == tiny_token_id::OP_LESSEQUAL ||
                    tokenId == tiny_token_id::OP_OVEREQUAL ||
                    tokenId == tiny_token_id::OP_NOTEQUAL
                );
            }

            bool isAssignOp() const{
                return (
                    tokenId == tiny_token_id::OP_ASSIGNMENT ||
                    tokenId == tiny_token_id::OP_ADDASSIGN ||
                    tokenId == tiny_token_id::OP_SUBASSIGN
                );
            }

            bool isStatementFirst(){
                return (
                    tokenId == tiny_token_id::KW_IF ||
                    tokenId == tiny_token_id::KW_REPEAT ||
                    tokenId == tiny_token_id::ID ||
                    tokenId == tiny_token_id::KW_READ ||
                    tokenId == tiny_token_id::KW_WRITE ||
                    tokenId == tiny_token_id::KW_FOR ||
                    tokenId == tiny_token_id::KW_WHILE ||
                    tokenId == tiny_token_id::OP_INCREMENT ||
                    tokenId == tiny_token_id::OP_DECREMENT
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
                return zhy::tiny_token_getter::operator_start.find(input) != zhy::tiny_token_getter::operator_start.end();
            }

            inline static bool isSymbol(char input = ' '){
                unsigned char sym = static_cast <unsigned char>(input);
                return (
                    std::isprint(sym) &&
                    !tiny_token::isWhiteSpaceChar(input) &&
                    !std::isalnum(sym)
                );
            }

            inline static bool isKeyword(const std::string & input){
                return zhy::tiny_token_getter::keyword_id_map.find(input) != zhy::tiny_token_getter::keyword_id_map.end();
            }

            inline static bool isOperator(const std::string & input){
                return zhy::tiny_token_getter::operator_id_map.find(input) != zhy::tiny_token_getter::operator_id_map.end();
            }

            /*inline static bool isEscapeChar(char input = ' '){
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
            }*/


        };

        private:

        tiny_token_getter(const zhy::tiny_token_getter & other) = delete;
        tiny_token_getter & operator =(const zhy::tiny_token_getter & other) = delete;

        std::string buffer;
        char tempChar;
        tiny_token tempToken;

        public:

        std::string filePath;
        std::ifstream in;
        std::deque <tiny_token> tokenDeque;

        tiny_token_getter(const std::string & fp,std::ostream & err = std::cerr):
            filePath(fp),tokenDeque(),
            buffer(),tempChar(0)
        {
            in.open(std::filesystem::path(filePath),std::ios::in);
            if (!in.is_open()){
                err<<"文件打开失败: "<<filePath<<std::endl;
            }
        }
        ~tiny_token_getter(){
            if (in.is_open()) in.close();
        }

        bool getNextChar(){
            return static_cast <bool>(in.get(tempChar));
        }

        void restart(){
            in.clear();
            in.seekg(0);
            buffer.clear();
            tempChar = 0;
            tokenDeque.clear();
            return;
        }

        void bufferAppendChar(bool ignoreSpace = true){
            if (ignoreSpace){
                if (!tiny_token::isWhiteSpaceChar(tempChar)) buffer.push_back(tempChar);
            }
            else buffer.push_back(tempChar);
            return;
        }

        void clearBuffer(){
            buffer.clear();
            tempToken.clear();
            return;
        }

        void rollbackBuffer(){
            in.unget();
            buffer.clear();
            return;
        }

        void pushBackToken(){
            //tempToken.display(std::cout);
            tokenDeque.emplace_back(tempToken);
            clearBuffer();
            return;
        }

        bool getToken(){
            if (!getNextChar()) return false;
            if (tiny_token::isWhiteSpaceChar(tempChar)) return true;
            bufferAppendChar();
            if (tempChar == '{'){
                while (getNextChar()){
                    bufferAppendChar(false);
                    if (tempChar == '}'){
                        tempToken.tokenId = tiny_token_id::COMMENT;
                        tempToken.setValue <std::string>(buffer);
                        pushBackToken();
                        return true;
                    }
                    else continue;
                }
                return true;
            }
            else if (tiny_token::isDigit(tempChar)){
                while (getNextChar()){
                    if (tiny_token::isDigit(tempChar)) bufferAppendChar();
                    else{
                        tempToken.tokenId = tiny_token_id::LT_UNSIGNEDINTEGER;
                        size_t ui = std::stoull(buffer);
                        tempToken.setValue <std::size_t>(ui);
                        pushBackToken();
                        rollbackBuffer();
                        return true;
                    }
                }
                return true;
            }
            else if (tiny_token::isIdStart(tempChar)){
                while (getNextChar()){
                    if (tiny_token::isIdContinue(tempChar)) bufferAppendChar();
                    else{
                        tempToken.setValue <std::string>(buffer);
                        tempToken.setId();
                        pushBackToken();
                        rollbackBuffer();
                        return true;
                    }
                }
                return true;
            }
            else if (tiny_token::isOperatorStart(tempChar)){
                std::string op1 = buffer;
                buffer.clear();
                if (getNextChar()){
                    bufferAppendChar();
                    if (tiny_token::isSymbol(tempChar)){
                        std::string op2 = op1 + buffer;
                        //std::cout<<op2<<std::endl;
                        if (tiny_token::isOperator(op2)){
                            tempToken.setValue <std::string>(op2);
                            tempToken.setId();
                            pushBackToken();
                            return true;
                        }
                        else if (tiny_token::isOperator(op1)){
                            tempToken.setValue <std::string>(op1);
                            tempToken.setId();
                            pushBackToken();
                            rollbackBuffer();
                            return true;
                        }
                        else{
                            tempToken.tokenId = tiny_token_id::ERROR;
                            tempToken.setValue <std::string>(op1);
                            pushBackToken();
                            rollbackBuffer();
                            return true;
                        }
                    }
                    else{
                        if (tiny_token::isOperator(op1)){
                            //std::cout<<op1<<std::endl;
                            tempToken.setValue <std::string>(op1);
                            tempToken.setId();
                            pushBackToken();
                            rollbackBuffer();
                            return true;
                        }
                        else{
                            tempToken.tokenId = tiny_token_id::ERROR;
                            tempToken.setValue <std::string>(op1);
                            pushBackToken();
                            rollbackBuffer();
                            return true;
                        }
                    }
                }
                if (tiny_token::isOperator(op1)){
                    tempToken.setValue <std::string>(op1);
                    tempToken.setId();
                    pushBackToken();
                    return true;
                }
                else{
                    tempToken.tokenId = tiny_token_id::ERROR;
                    tempToken.setValue <std::string>(op1);
                    pushBackToken();
                    return true;
                }
            }
            else{
                tempToken.tokenId = tiny_token_id::ERROR;
                tempToken.setValue <std::string>(buffer);
                pushBackToken();
                return true;
            }
            return true;
        }

        void getAllTokens(std::ostream & err = std::cerr){
            restart();
            while (true){
                try{
                    if (!getToken()) break;
                }
                catch (const std::exception & e){
                    err<<"在进行tiny扩充语言的词法分析时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    continue;
                }
            }
            /*
            while (true){
                if (!getToken()) break;
            }
            */
            return;
        }

        size_t getTokenNum() const{
            return tokenDeque.size();
        }

        void display(std::ostream & out = std::cout) const{
            out<<"文件: "<<filePath<<"的词法分析结果:"<<std::endl;
            for(const auto & token : tokenDeque){
                token.display(out);
            }
            return;
        }

        void getTable(std::vector <std::vector <std::string>> & table) const{
            table.resize(getTokenNum());
            for(size_t i=0;i<tokenDeque.size();i++){
                tokenDeque[i].getTable(table[i]);
            }
            return;
        }

    };

/*
program -> stmt-sequence
stmt-sequence -> stmt-sequence statement | statement
statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt | for-stmt | while-stmt | unary-stmt
if-stmt -> if exp then stmt-sequence endif | if exp then stmt-sequence else stmt-sequence endif
repeat-stmt -> repeat stmt-sequence until exp ;
assign-stmt -> identifier assign-op exp ;
assign-op -> := | += | -=
read-stmt -> read identifier ;
write-stmt -> write exp ;
for-stmt -> for ( assign-stmt exp ; unary-exp ) stmt-sequence endfor
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
*/

    class tiny_quadruple_getter{

        public:

        class operands_node;
        class tiny_quadruple;

        private:

        tiny_quadruple_getter(const zhy::tiny_quadruple_getter & other) = delete;
        tiny_quadruple_getter & operator =(const zhy::tiny_quadruple_getter & other) = delete;

        inline static std::string grammarRule = R"(
program -> stmt-sequence
stmt-sequence -> stmt-sequence statement | statement
statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt | for-stmt | while-stmt | unary-stmt
if-stmt -> if exp then stmt-sequence endif | if exp then stmt-sequence else stmt-sequence endif
repeat-stmt -> repeat stmt-sequence until exp ;
assign-stmt -> identifier assign-op exp ;
assign-op -> := | += | -=
read-stmt -> read identifier ;
write-stmt -> write exp ;
for-stmt -> for ( assign-stmt exp ; unary-exp ) stmt-sequence endfor
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
        )";

        using tiny_token_id = zhy::tiny_token_getter::tiny_token_id;
        using tiny_token = zhy::tiny_token_getter::tiny_token;

        zhy::tiny_token_getter tokenGetter;
        tiny_token tempToken;
        size_t position;
        std::vector <std::string> errorWords;
        std::vector <std::string> warningWords;
        std::unordered_set <std::string> identifiers;
        //size_t trueExit;
        //size_t falseExit;
        size_t incrementalId;

        bool getToken(){
            if (position >= tokenGetter.getTokenNum()) return false;
            while (tokenGetter.tokenDeque[position].tokenId == tiny_token_id::COMMENT){
                position += 1;
            }
            tempToken = tokenGetter.tokenDeque[position];
            position += 1;
            return true;
        }

        bool identifierExists(const std::string & id) const{
            auto it = identifiers.find(id);
            return it != identifiers.end();
        }

        bool error(){
            std::string errorWord = "\"" + tempToken.getWord() + "\"附近存在语法错误";
            errorWords.emplace_back(errorWord);
            return getToken();
        }

        void warning(size_t id = 0){
            if (id == 0){
                std::string warningWord = "\"" + tempToken.getWord() + "\"附近存在警告";
                warningWords.emplace_back(warningWord);
            }
            else{
                id %= warning_word_map.size();
                std::string warningWord = warning_word_map.at(id);
                warningWords.emplace_back(warningWord);
            }
            return;
        }

        bool match(tiny_token_id tokenId){
            if (tempToken.tokenId == tokenId) return getToken();
            else return error();
        }

        const std::string & getFunctionCode(tiny_token_id tokenId) const{
            auto it = function_code_map.find(tokenId);
            if (it == function_code_map.end()) return function_code_map.at(tiny_token_id::ERROR);
            else return it->second;
        }

        std::string getIncrementalId(){
            std::string id = "t" + std::to_string(incrementalId);
            incrementalId += 1;
            return id;
        }

        template <class T>
        void setForthValue(size_t position,const T & value){
            quadruples[position].forth.setValue <T>(value);
            return;
        }

        void generate(tiny_token_id id,const operands_node & x,const operands_node & y,const operands_node & z){
            std::string func = getFunctionCode(id);
            quadruples.emplace_back(tiny_quadruple(func,x,y,z));
            return;
        }

        bool tokenEnd() const{
            return position >= tokenGetter.getTokenNum();
        }

        void generateExit(){
            generate(tiny_token_id::FUNC_EXIT,operands_node(),operands_node(),operands_node());
            return;
        }

        operands_node factor(){
            operands_node x;
            switch (tempToken.tokenId){
                case tiny_token_id::OP_BRACKETLEFT:{
                    match(tiny_token_id::OP_BRACKETLEFT);
                    x = expression();
                    match(tiny_token_id::OP_BRACKETRIGHT);
                    break;
                }
                case tiny_token_id::LT_UNSIGNEDINTEGER:{
                    x.flag = 0;
                    x.setValue <size_t>(tempToken.getValue <size_t>());
                    match(tiny_token_id::LT_UNSIGNEDINTEGER);
                    break;
                }
                case tiny_token_id::ID:{
                    x.flag = 1;
                    std::string id = tempToken.getValue <std::string>();
                    if (!identifierExists(id)) error();
                    x.setValue <std::string>(id);
                    match(tiny_token_id::ID);
                    break;
                }
                case tiny_token_id::OP_INCREMENT:{
                    x = unary_exp();
                    break;
                }
                case tiny_token_id::OP_DECREMENT:{
                    x = unary_exp();
                    break;
                }
                default:error();
            }
            return x;
        }

        operands_node exponent(){
            operands_node x = factor();
            if (tempToken.tokenId == tiny_token_id::OP_POWER){
                tiny_token_id tokenId = tempToken.tokenId;
                match(tiny_token_id::OP_POWER);
                operands_node y = exponent();
                operands_node z;
                /*
                z.flag = 0;
                z.setValue <size_t>(static_cast <size_t>(std::pow(x.getValue <size_t>(),y.getValue <size_t>())));
                */
                z.flag = 1;
                z.setValue <std::string>(getIncrementalId());
                generate(tokenId,x,y,z);
                return z;
            }
            else return x;
        }

        operands_node term(){
            operands_node x = exponent();
            while (tempToken.isMulOperator()){
                tiny_token_id tokenId = tempToken.tokenId;
                match(tempToken.tokenId);
                operands_node y = exponent();
                //size_t xV = x.getValue <size_t>();
                size_t yV = y.getValue <size_t>();
                operands_node z;
                /*z.flag = 0;
                if (tokenId == tiny_token_id::OP_MULTIPLICATION) z.setValue <size_t>(xV * yV);
                else if (tokenId == tiny_token_id::OP_DIVISION){
                    if (yV == 0) warning(1);
                    z.setValue <size_t>(xV / yV);
                }
                else if (tokenId == tiny_token_id::OP_MODULUS) z.setValue <size_t>(xV % yV);
                else error();*/
                if (tokenId == tiny_token_id::OP_DIVISION && y.flag == 0 && yV == 0) warning(1);
                z.flag = 1;
                z.setValue <std::string>(getIncrementalId());
                generate(tokenId,x,y,z);
                std::swap(x,z);
            }
            return x;
        }

        operands_node simple(){
            operands_node x = term();
            while (tempToken.isAddOperator()){
                tiny_token_id tokenId = tempToken.tokenId;
                match(tempToken.tokenId);
                operands_node y = term();
                size_t xV = x.getValue <size_t>();
                size_t yV = y.getValue <size_t>();
                operands_node z;
                /*z.flag = 0;
                if (tokenId == tiny_token_id::OP_ADDITION) z.setValue <size_t>(xV + yV);
                else if (tokenId == tiny_token_id::OP_SUBTRACTION) z.setValue <size_t>(xV - yV);
                else error();*/
                z.flag = 1;
                z.setValue <std::string>(getIncrementalId());
                generate(tokenId,x,y,z);
                std::swap(x,z);
            }
            return x;
        }

        operands_node expression(){
            operands_node x = simple();
            if (tempToken.isComparisonOp()){
                tiny_token_id tokenId = tempToken.tokenId;
                match(tempToken.tokenId);
                operands_node y = simple();
                operands_node z;
                /*z.flag = 0;
                bool opResult;
                size_t xV = x.getValue <size_t>();
                size_t yV = x.getValue <size_t>();
                switch (tokenId){
                    case tiny_token_id::OP_LESS:{opResult = xV < yV;break;}
                    case tiny_token_id::OP_OVER:{opResult = xV > yV;break;}
                    case tiny_token_id::OP_EQUAL:{opResult = xV == yV;break;}
                    case tiny_token_id::OP_LESSEQUAL:{opResult = xV <= yV;break;}
                    case tiny_token_id::OP_OVEREQUAL:{opResult = xV >= yV;break;}
                    case tiny_token_id::OP_NOTEQUAL:{opResult = xV != yV;break;}
                    default:{opResult = false;break;}
                }
                z.setValue <size_t>(static_cast <size_t>(opResult));*/
                z.flag = 1;
                z.setValue <std::string>(getIncrementalId());
                generate(tokenId,x,y,z);
                return z;
            }
            else return x;
        }
        
        operands_node unary_exp(){
            switch (tempToken.tokenId){
                case tiny_token_id::OP_INCREMENT:{
                    tiny_token_id tokenId = tempToken.tokenId;
                    match(tiny_token_id::OP_INCREMENT);
                    std::string iden = tempToken.getValue <std::string>();
                    match(tiny_token_id::ID);
                    operands_node x;
                    x.flag = 1;
                    x.setValue <std::string>(iden);
                    //std::cout<<iden<<std::endl;
                    operands_node y;
                    y.flag = 0;
                    y.setValue <size_t>(1);
                    operands_node z;
                    z.flag = 1;
                    z.setValue <std::string>(getIncrementalId());
                    generate(tokenId,x,y,z);
                    generate(tiny_token_id::OP_ASSIGNMENT,z,operands_node(),x);
                    return x;
                }
                case tiny_token_id::OP_DECREMENT:{
                    tiny_token_id tokenId = tempToken.tokenId;
                    match(tiny_token_id::OP_DECREMENT);
                    std::string iden = tempToken.getValue <std::string>();
                    match(tiny_token_id::ID);
                    operands_node x;
                    x.flag = 1;
                    x.setValue <std::string>(iden);
                    operands_node y;
                    y.flag = 0;
                    y.setValue <size_t>(1);
                    operands_node z;
                    z.flag = 1;
                    z.setValue <std::string>(getIncrementalId());
                    generate(tokenId,x,y,z);
                    generate(tiny_token_id::OP_ASSIGNMENT,z,operands_node(),x);
                    return z;
                }
                default:error();
            }
            return operands_node();
        }

        void unary(){
            operands_node u = unary_exp();
            match(tiny_token_id::OP_SEMICOLON);
            return;
        }

        size_t _if(){
            match(tiny_token_id::KW_IF);
            operands_node e = expression();
            operands_node j;
            j.flag = 0;
            generate(tiny_token_id::FUNC_JTRUE,e,operands_node(),j);
            size_t trueQuadruple = getQuadrupleNum() - 1;
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),j);
            size_t falseQuadruple = getQuadrupleNum() - 1;
            match(tiny_token_id::KW_THEN);
            size_t trueExit = getQuadrupleNum();
            sequence();
            setForthValue <size_t>(trueQuadruple,trueExit);
            if (tempToken.tokenId == tiny_token_id::KW_ENDIF){
                match(tiny_token_id::KW_ENDIF);
                return falseQuadruple;
            }
            else{
                generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),j);
                trueQuadruple = getQuadrupleNum() - 1;
                match(tiny_token_id::KW_ELSE);
                size_t falseExit = getQuadrupleNum();
                sequence();
                setForthValue <size_t>(falseQuadruple,falseExit);
                match(tiny_token_id::KW_ENDIF);
                return trueQuadruple;
            }
        }

        size_t repeat(){
            match(tiny_token_id::KW_REPEAT);
            size_t seqPosition = getQuadrupleNum();
            sequence();
            match(tiny_token_id::KW_UNTIL);
            operands_node e = expression();
            operands_node j;
            j.flag = 0;
            j.setValue <size_t>(seqPosition);
            generate(tiny_token_id::FUNC_JTRUE,e,operands_node(),j);
            size_t trueQuadruple = getQuadrupleNum() - 1;
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),j);
            match(tiny_token_id::OP_SEMICOLON);
            return trueQuadruple;
        }

        void assign(){
            std::string iden = tempToken.getValue <std::string>();
            identifiers.emplace(iden);
            operands_node x;
            x.flag = 1;
            x.setValue <std::string>(iden);
            match(tiny_token_id::ID);
            switch (tempToken.tokenId){
                case tiny_token_id::OP_ASSIGNMENT:{
                    tiny_token_id tokenId = tempToken.tokenId;
                    match(tiny_token_id::OP_ASSIGNMENT);
                    operands_node e = expression();
                    generate(tokenId,e,operands_node(),x);
                    break;
                }
                case tiny_token_id::OP_ADDASSIGN:{
                    match(tiny_token_id::OP_ADDASSIGN);
                    if (!identifierExists(iden)) error();
                    operands_node e = expression();
                    operands_node z;
                    z.flag = 1;
                    z.setValue <std::string>(getIncrementalId());
                    generate(tiny_token_id::OP_ADDITION,x,e,z);
                    generate(tiny_token_id::OP_ASSIGNMENT,z,operands_node(),x);
                    break;
                }
                case tiny_token_id::OP_SUBASSIGN:{
                    match(tiny_token_id::OP_SUBASSIGN);
                    if (!identifierExists(iden)) error();
                    operands_node e = expression();
                    operands_node z;
                    z.flag = 1;
                    z.setValue <std::string>(getIncrementalId());
                    generate(tiny_token_id::OP_SUBTRACTION,x,e,z);
                    generate(tiny_token_id::OP_ASSIGNMENT,z,operands_node(),x);
                    break;
                }
                default:{error();break;}
            }
            match(tiny_token_id::OP_SEMICOLON);
            return;
        }

        void read(){
            tiny_token_id tokenId = tempToken.tokenId;
            match(tiny_token_id::KW_READ);
            std::string iden = tempToken.getValue <std::string>();
            operands_node x;
            x.flag = 1;
            x.setValue <std::string>(iden);
            identifiers.emplace(iden);
            match(tiny_token_id::ID);
            generate(tokenId,operands_node(),operands_node(),x);
            match(tiny_token_id::OP_SEMICOLON);
            return;
        }

        void write(){
            tiny_token_id tokenId = tempToken.tokenId;
            match(tiny_token_id::KW_WRITE);
            operands_node e = expression();
            generate(tokenId,e,operands_node(),operands_node());
            match(tiny_token_id::OP_SEMICOLON);
            return;
        }

        size_t _for(){
            match(tiny_token_id::KW_FOR);
            match(tiny_token_id::OP_BRACKETLEFT);
            assign();
            size_t expPosition = getQuadrupleNum();
            operands_node e = expression();
            match(tiny_token_id::OP_SEMICOLON);
            operands_node j;
            j.flag = 0;
            generate(tiny_token_id::FUNC_JTRUE,e,operands_node(),j);
            size_t trueQuadruple = getQuadrupleNum() - 1;
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),j);
            size_t falseQuadruple = getQuadrupleNum() - 1;
            size_t unaryPosition = getQuadrupleNum();
            operands_node u = unary_exp();
            operands_node ej;
            ej.flag = 0;
            ej.setValue <size_t>(expPosition);
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),ej);
            match(tiny_token_id::OP_BRACKETRIGHT);
            size_t seqPosition = getQuadrupleNum();
            sequence();
            setForthValue <size_t>(trueQuadruple,seqPosition);
            operands_node uj;
            uj.flag = 0;
            uj.setValue <size_t>(unaryPosition);
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),uj);
            match(tiny_token_id::KW_ENDFOR);
            return falseQuadruple;
        }

        size_t _while(){
            match(tiny_token_id::KW_WHILE);
            match(tiny_token_id::OP_BRACKETLEFT);
            size_t expPosition = getQuadrupleNum();
            operands_node e = expression();
            operands_node j;
            j.flag = 0;
            generate(tiny_token_id::FUNC_JTRUE,e,operands_node(),j);
            size_t trueQuadruple = getQuadrupleNum() - 1;
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),j);
            size_t falseQuadruple = getQuadrupleNum() - 1;
            match(tiny_token_id::OP_BRACKETRIGHT);
            size_t seqPosition = getQuadrupleNum();
            sequence();
            setForthValue <size_t>(trueQuadruple,seqPosition);
            operands_node ej;
            ej.flag = 0;
            ej.setValue <size_t>(expPosition);
            generate(tiny_token_id::FUNC_JUMP,operands_node(),operands_node(),ej);
            match(tiny_token_id::KW_ENDWHILE);
            return falseQuadruple;
        }

        size_t statement(){
            if (!tempToken.isStatementFirst()) return std::numeric_limits <size_t>::max();
            switch (tempToken.tokenId){
                case tiny_token_id::KW_IF:return _if();
                case tiny_token_id::KW_REPEAT:return repeat();
                case tiny_token_id::ID:{
                    assign();
                    return std::numeric_limits <size_t>::max();
                }
                case tiny_token_id::KW_READ:{
                    read();
                    return std::numeric_limits <size_t>::max();
                }
                case tiny_token_id::KW_WRITE:{
                    write();
                    return std::numeric_limits <size_t>::max();
                }
                case tiny_token_id::KW_FOR:return _for();
                case tiny_token_id::KW_WHILE:return _while();
                case tiny_token_id::OP_INCREMENT:{
                    unary();
                    return std::numeric_limits <size_t>::max();
                }
                case tiny_token_id::OP_DECREMENT:{
                    unary();
                    return std::numeric_limits <size_t>::max();
                }
                default:{
                    error();
                    return std::numeric_limits <size_t>::max();
                }
            }
        }

        void sequence(){
            size_t oldNum = getQuadrupleNum();
            while (!tokenEnd() && tempToken.isStatementFirst()){
                size_t falseQuadruple = statement();
                if (falseQuadruple < std::numeric_limits <size_t>::max()){
                    size_t quadruplePosition = getQuadrupleNum();
                    setForthValue <size_t>(falseQuadruple,quadruplePosition);
                }
            }
            //tempToken.display();
            size_t newNum = getQuadrupleNum();
            if (tokenGetter.getTokenNum() > 0 && oldNum >= newNum){
                error();
                //generate(tiny_token_id::ERROR,operands_node(),operands_node(),operands_node());
            }
            return;
        }

        public:

        class operands_node{

            public:

            unsigned char flag;//0: 值, 1: 变量, 2: bool 3: 空
            std::variant <size_t,std::string> value;

            operands_node():flag(3),value(){}
            ~operands_node(){}

            operands_node(const operands_node & other){
                flag = other.flag;
                value = other.value;
            }

            operands_node & operator =(const operands_node & other){
                if (this != &other){
                    flag = other.flag;
                    value = other.value;
                }
                return *this;
            }

            template <class T>
            void setValue(const T & v){
                value = v;
                return;
            }

            template <class T>
            const T & getValue() const{
                static_assert(
                    std::is_same_v <T,std::string> || std::is_same_v <T,size_t>,
                    "tiny_token::value 仅支持 std::string / size_t"
                );
                const T* ptr = std::get_if <T>(&value);
                if (ptr != nullptr) return *ptr;
                static const T default_val{};
                return default_val;
            }

            std::string getWord() const{
                if (flag == 0) return std::to_string(getValue <size_t>());
                else return getValue <std::string>();
            }

        };

        class tiny_quadruple{

            public:

            std::string first;
            operands_node second;
            operands_node third;
            operands_node forth;

            tiny_quadruple():first(),second(),third(),forth(){}
            ~tiny_quadruple(){}

            tiny_quadruple(const std::string & func,const operands_node & x,const operands_node & y,const operands_node & z):
                first(func),second(x),third(y),forth(z){}

            tiny_quadruple(const tiny_quadruple & other):
                first(other.first),
                second(other.second),
                third(other.third),
                forth(other.forth)
            {}

            tiny_quadruple & operator =(const tiny_quadruple & other){
                if (this != &other){
                    first = other.first;
                    second = other.second;
                    third = other.third;
                    forth = other.forth;
                }
                return *this;
            }

            friend std::ostream & operator <<(std::ostream & out,const tiny_quadruple & self){
                out<<"("<<self.first<<","<<self.second.getWord()<<","<<self.third.getWord()<<","<<self.forth.getWord()<<")";
                return out;
            }

        };

        inline static const std::unordered_map <tiny_token_id,std::string> function_code_map = {
            {tiny_token_id::KW_READ,"read"},
            {tiny_token_id::KW_WRITE,"write"},

            {tiny_token_id::OP_ASSIGNMENT,":="},
            //{tiny_token_id::OP_INCREMENT,"++"},
            {tiny_token_id::OP_INCREMENT,"+"},
            //{tiny_token_id::OP_DECREMENT,"--"},
            {tiny_token_id::OP_DECREMENT,"-"},
            {tiny_token_id::OP_LESSEQUAL,"<="},
            {tiny_token_id::OP_OVEREQUAL,">="},
            {tiny_token_id::OP_NOTEQUAL,"<>"},
            //{tiny_token_id::OP_ADDASSIGN,"+="},
            //{tiny_token_id::OP_SUBASSIGN,"-="},

            {tiny_token_id::OP_ADDITION,"+"},
            {tiny_token_id::OP_SUBTRACTION,"-"},
            {tiny_token_id::OP_MULTIPLICATION,"*"},
            {tiny_token_id::OP_DIVISION,"/"},
            {tiny_token_id::OP_EQUAL,"="},
            {tiny_token_id::OP_LESS,"<"},
            //{tiny_token_id::OP_BRACKETLEFT,"("},
            //{tiny_token_id::OP_BRACKETRIGHT,")"},
            {tiny_token_id::OP_SEMICOLON,";"},
            {tiny_token_id::OP_MODULUS,"%"},
            {tiny_token_id::OP_POWER,"^"},
            {tiny_token_id::OP_OVER,">"},

            {tiny_token_id::ERROR,"error"},

            {tiny_token_id::FUNC_JUMP,"jump"},
            {tiny_token_id::FUNC_JTRUE,"jTrue"},
            {tiny_token_id::FUNC_JFALSE,"jFalse"},
            {tiny_token_id::FUNC_EXIT,"exit"}
        };

        inline static const std::vector <std::string> warning_word_map = {
            "警告",
            "警告: 0不能作为除数"
        };

        std::deque <tiny_quadruple> quadruples;
        std::ostream & err;

        tiny_quadruple_getter(const std::string & filePath,std::ostream & e = std::cerr):
            tokenGetter(filePath,e),tempToken(),position(0),errorWords(),
            warningWords(),identifiers(),incrementalId(0),quadruples(),err(e)
        {
            tokenGetterGetTokens();
        }
        ~tiny_quadruple_getter(){}

        void restart(){
            tempToken.clear();
            position = 0;
            errorWords.clear();
            warningWords.clear();
            identifiers.clear();
            incrementalId = 0;
            return;
        }

        void clear(){
            restart();
            quadruples.clear();
            return;
        }

        void start(){
            try{
                clear();
                getToken();
                sequence();
                generateExit();
            }
            catch (const std::exception & e){
                err<<"在进行tiny扩充语言的中间代码生成时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void tokenGetterGetTokens(){
            tokenGetter.getAllTokens(err);
            return;
        }

        const std::string & getFilePath() const{
            return tokenGetter.filePath;
        }

        void display(std::ostream & out = std::cout,bool outputTitle = false,bool outputIndex = false) const{
            if (outputTitle) out<<"文件: "<<getFilePath()<<"的四元组中间代码:"<<std::endl;
            for(size_t i=0;i<quadruples.size();i++){
                if (outputIndex) out<<std::setw(5)<<std::setfill(' ')<<i<<" ";
                out<<quadruples[i]<<std::endl;
            }
            return;
        }

        void outputFile(const std::string & filePath,std::ostream & err = std::cerr) const{
            std::ofstream out(std::filesystem::path(filePath),std::ios::out);
            if (!out){
                err<<"文件打开失败: "<<filePath<<std::endl;
                return;
            }
            display(out,false,false);
            out.close();
            return;
        }

        void displayAllTokens(std::ostream & out = std::cout) const{
            tokenGetter.display(out);
            return;
        }

        inline static const std::string & getGrammarRule(){
            return grammarRule;
        }

        const std::vector <std::string> & getErrorWords() const{
            return errorWords;
        }

        const std::vector <std::string> & getWarningWords() const{
            return warningWords;
        }

        void displayErrors(std::ostream & out = std::cout) const{
            for(const auto & errorWord : errorWords){
                out<<errorWord<<std::endl;
            }
            return;
        }

        void displayWarnings(std::ostream & out = std::cout) const{
            for(const auto & warningWord : warningWords){
                out<<warningWord<<std::endl;
            }
            return;
        }

        size_t getQuadrupleNum() const{
            return quadruples.size();
        }

        size_t getErrorNum() const{
            return errorWords.size();
        }

        size_t getWarningNum() const{
            return warningWords.size();
        }

    };

    class tiny_tuple_tester{

        private:

        tiny_tuple_tester() = delete;
        ~tiny_tuple_tester() = delete;
        tiny_tuple_tester(const tiny_tuple_tester & other) = delete;
        tiny_tuple_tester & operator =(const tiny_tuple_tester & other) = delete;

        public:

        inline static void tester(const std::string & filePath){
            zhy::tiny_quadruple_getter test(filePath,std::cerr);
            test.displayAllTokens(std::cout);
            test.start();
            test.display(std::cout,true,true);
            test.displayErrors(std::cout);
            test.displayWarnings(std::cout);
            return;
        }

        inline static void test(){
            std::vector <std::string> filePaths = {
                "normal_test1.tny",
                "normal_test2.tny",
                "normal_test3.tny",
                "normal_test4.tny",
                "normal_test5.tny",
            };
            for(const auto & filePath : filePaths){
                zhy::tiny_tuple_tester::tester(filePath);
            }

            return;
        }


    };

}

#endif