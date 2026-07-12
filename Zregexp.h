#ifndef Z_REGEXP_H
#define Z_REGEXP_H
#pragma once

#include <filesystem>//解决中文路径无法打开文件的问题
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>//顺序表
#include <queue>//队列，用于层序遍历
#include <deque>//双端队列
#include <tuple>//多元组
#include <set>//红黑树集合
#include <map>//红黑树映射
#include <unordered_set>//哈希集合
#include <unordered_map>//哈希映射
#include <algorithm>
#include <cstddef>
#include <cctype>
#include <limits>
#include <stdexcept>//异常处理

#include "Zset.h"

namespace zhy{

    enum class regexp_token_id{
        NONEID,

        OP_SELECT,
        OP_CLOSURE,
        OP_CLOSUREPOSITIVE,
        OP_RANGELEFT,
        OP_RANGERIGHT,
        OP_OPTIONAL,
        OP_BRACKETLEFT,
        OP_BRACKETRIGHT,
        OP_ASSIGNMENT,

        ID,
        IDDFA
    };


    inline const std::unordered_map <std::string,zhy::regexp_token_id> regexp_operator_string_map = {
        {"|",zhy::regexp_token_id::OP_SELECT},
        {"*",zhy::regexp_token_id::OP_CLOSURE},
        {"+",zhy::regexp_token_id::OP_CLOSUREPOSITIVE},
        {"[",zhy::regexp_token_id::OP_RANGELEFT},
        {"]",zhy::regexp_token_id::OP_RANGERIGHT},
        {"?",zhy::regexp_token_id::OP_OPTIONAL},
        {"(",zhy::regexp_token_id::OP_BRACKETLEFT},
        {")",zhy::regexp_token_id::OP_BRACKETRIGHT},
        {"=",zhy::regexp_token_id::OP_ASSIGNMENT}
    };

    inline const std::unordered_map <char,zhy::regexp_token_id> regexp_operator_char_map = {
        {'|',zhy::regexp_token_id::OP_SELECT},
        {'*',zhy::regexp_token_id::OP_CLOSURE},
        {'+',zhy::regexp_token_id::OP_CLOSUREPOSITIVE},
        {'[',zhy::regexp_token_id::OP_RANGELEFT},
        {']',zhy::regexp_token_id::OP_RANGERIGHT},
        {'?',zhy::regexp_token_id::OP_OPTIONAL},
        {'(',zhy::regexp_token_id::OP_BRACKETLEFT},
        {')',zhy::regexp_token_id::OP_BRACKETRIGHT},
        {'=',zhy::regexp_token_id::OP_ASSIGNMENT}
    };

    
    class regexp_token{

        public:
        zhy::regexp_token_id tokenId;
        std::string tokenWord;
        size_t code;

        regexp_token(){
            tokenId = zhy::regexp_token_id::NONEID;
            tokenWord = "";
            code = 0;
        }
        ~regexp_token(){}

        regexp_token(const regexp_token & other){
            tokenId = other.tokenId;
            tokenWord = other.tokenWord;
            code = other.code;
        }

        regexp_token & operator =(const regexp_token & other){
            if (this != &other){
                tokenId = other.tokenId;
                tokenWord = other.tokenWord;
                code = other.code;
            }
            return *this;
        }

        std::string & getTokenWord(){
            return tokenWord;
        }

        zhy::regexp_token_id getTokenId(){
            return tokenId;
        }

        void clear(){
            tokenId = zhy::regexp_token_id::NONEID;
            tokenWord = "";
            return;
        }

        bool setTokenId(){
            if (tokenWord == ""){
                tokenId = zhy::regexp_token_id::NONEID;
                return false;
            }
            auto it = regexp_operator_string_map.find(tokenWord);
            if (it != regexp_operator_string_map.end()){
                tokenId = it->second;
                return false;
            }
            else{
                size_t start = 0;
                while (true){
                    size_t underline = tokenWord.find_first_of('_',start);
                    if (underline == std::string::npos){
                        tokenId = zhy::regexp_token_id::ID;
                        return false;
                    }
                    else{
                        start = underline + 1;
                        if (tokenWord.size() > start && !std::isdigit(tokenWord[start])) continue;
                        auto endIt = std::find_if(tokenWord.begin() + start,tokenWord.end(),[](char c){
                            return !std::isdigit(static_cast <unsigned char>(c));
                        });
                        size_t end = tokenWord.size();
                        if (endIt != tokenWord.end()) end = endIt - tokenWord.begin();
                        if (end > start) code = std::stoull(tokenWord.substr(start,(end - start)));
                        tokenId = zhy::regexp_token_id::IDDFA;
                        if (tokenWord.size() > end && tokenWord[end] == 'L') return true;
                        return false;
                    }
                }
            }
        }

        void display(std::ostream & out = std::cout) const{
            out<<std::setfill(' ')<<std::setw(5)<<code<<" "<<tokenWord<<" : ";
            if (tokenId == zhy::regexp_token_id::NONEID) out<<"未知的tokenId";
            else if (tokenId >= zhy::regexp_token_id::OP_SELECT && tokenId <= zhy::regexp_token_id::OP_ASSIGNMENT) out<<"运算符";
            else if (tokenId == zhy::regexp_token_id::ID) out<<"普通的标识符";
            else out<<"需要生成DFA图的正则表达式名";
            out<<std::endl;
            return;
        }

        bool isId() const{
            return (
                tokenId == zhy::regexp_token_id::ID ||
                tokenId == zhy::regexp_token_id::IDDFA
            );
        }

        bool isUnaryOperator() const{
            return (
                tokenId == zhy::regexp_token_id::OP_CLOSURE ||
                tokenId == zhy::regexp_token_id::OP_CLOSUREPOSITIVE ||
                tokenId == zhy::regexp_token_id::OP_OPTIONAL
            );
        }

        bool isFactorFirst() const{
            return (isId() || tokenId == zhy::regexp_token_id::OP_BRACKETLEFT);
        }

        inline static bool isIdStart(char input = ' '){//判断标识符起始符
            return std::isalpha(static_cast <unsigned char>(input)) || input == '_';
        }

        inline static bool isIdContinue(char input = ' '){//判断标识符中间符
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

        inline static bool isOperator(char input = ' '){
            return (zhy::regexp_operator_char_map.find(input) != zhy::regexp_operator_char_map.end());
        }

        inline static bool isUnaryOperator(zhy::regexp_token_id id){
            return (
                id == zhy::regexp_token_id::OP_CLOSURE ||
                id == zhy::regexp_token_id::OP_CLOSUREPOSITIVE ||
                id == zhy::regexp_token_id::OP_OPTIONAL
            );
        }

        




    };

    inline bool operator ==(const regexp_token & a,const regexp_token & b){
        return {
            a.tokenId == b.tokenId &&
            a.tokenWord == b.tokenWord &&
            a.code == b.code
        };
    }


    class regexp_tokens{

        public:
        std::string regexp_str;
        std::vector <zhy::regexp_token> tokens;
        std::string tableName;
        bool needDfa;
        bool isRange;

        regexp_tokens(){
            regexp_str = "";
            tokens.clear();
            tableName = "";
            needDfa = false;
            isRange = false;
        }
        ~regexp_tokens(){}

        regexp_tokens(const regexp_tokens & other){
            regexp_str = other.regexp_str;
            tokens = other.tokens;
            tableName = other.tableName;
            needDfa = other.needDfa;
            isRange = other.isRange;
        }

        regexp_tokens & operator =(const regexp_tokens & other){
            if (this != &other){
                regexp_str = other.regexp_str;
                tokens = other.tokens;
                tableName = other.tableName;
                needDfa = other.needDfa;
                isRange = other.isRange;
            }
            return *this;
        }

        void clear(){
            regexp_str = "";
            tokens.clear();
            tableName = "";
            needDfa = false;
            isRange = false;
            return;
        }

        void display(std::ostream & out = std::cout) const{
            out<<regexp_str<<std::endl;
            out<<"regexp_name : "<<tableName<<std::endl;
            for(const auto & token : tokens){
                token.display(out);
            }
            return;
        }

        void getHeads(std::unordered_set <std::string> & heads,bool needClear = true) const{
            if (needClear) heads.clear();
            for(size_t i=2;i<tokens.size();i++){
                if (tokens[i].isId()) heads.emplace(tokens[i].tokenWord);
            }
            return;
        }

        size_t getCode() const{
            if (tokens.size() > 0) return tokens[0].code;
            else return 0;
        }

        const zhy::regexp_token & operator [](size_t index) const{
            if (index < tokens.size()) return tokens[index];
            else throw std::out_of_range("zhy::regexp_tokens[] : index out of range");
        }

        size_t size() const{
            return tokens.size();
        }

    };


    class regexp_lexer{

        private:
        regexp_lexer() = delete;
        ~regexp_lexer() = delete;
        regexp_lexer(const zhy::regexp_lexer & other) = delete;
        regexp_lexer & operator =(const zhy::regexp_lexer & other) = delete;

        public:

        inline static void trimLine(std::string & line){//去除行首尾空字符
            line.erase(0,line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            return;
        }

        inline static void removeWhiteSpaceChar(std::string & input){
            input.erase(
                std::remove_if(
                    input.begin(),
                    input.end(),
                    zhy::regexp_token::isWhiteSpaceChar
                ),
                input.end()
            );
            return;
        }

        inline static void splitString(
            const std::string & input,
            const std::vector <std::string> & lastRegexpNames,
            std::vector <std::string> & output
        ){
            output.clear();
            size_t start = 0;
            size_t n = input.size();
            while (start < n){
                size_t minPos = std::string::npos;
                std::string foundKey;
                for(const auto & name : lastRegexpNames){
                    if (name.empty()) continue;
                    size_t pos = input.find(name,start);
                    if (pos != std::string::npos && pos < minPos){
                        minPos = pos;
                        foundKey = name;
                    }
                }
                if (minPos == std::string::npos){
                    std::string part = input.substr(start);
                    if (!part.empty()) output.emplace_back(part);
                    break;
                }
                size_t len = minPos - start;
                if (len > 0) output.emplace_back(input.substr(start,len));
                output.emplace_back(foundKey);
                start = minPos + foundKey.size();
            }
            return;
        }

        inline static void rangeCharToString(char start,char end,std::string & str){
            if (start > end){
                str = "";
                return;
            }
            str += start;
            for(char i=(start+1);i<=end;i++){
                if (i != '[' && i != ']'){
                    str += "|";
                    if (i == '\\') str += "\\\\";
                    else str += i;
                }
            }
            return;
        }

        inline static bool unfoldRange(std::string & line){
            bool isRange = false;
            size_t start = 0;
            size_t end = line.size();
            std::string newLine = "";
            for(size_t i=0;i<line.size();i++){
                if (line[i] == '['){
                    if (i > 0 && line[i - 1] == '=') isRange = true;
                    end = i;
                    size_t rangeEnd = line.find(']',(i + 1));
                    if (rangeEnd == std::string::npos) rangeEnd = line.size();
                    newLine += line.substr(start,(end - start));
                    size_t rangeNum = 0;
                    std::string unfoldStr = "";
                    for(size_t j=(i+1);j<rangeEnd;j++){
                        if (line[j] == '-'){
                            char charStart = line[j - 1];
                            char charEnd = line[j + 1];
                            if (rangeNum > 0) unfoldStr += "|(";
                            else unfoldStr += "(";
                            zhy::regexp_lexer::rangeCharToString(charStart,charEnd,unfoldStr);
                            unfoldStr += ")";
                            rangeNum += 1;
                        }
                        else if ((j - 1) >= i && line[j - 1] != '-' && (j + 1) <= rangeEnd && line[j + 1] != '-'){
                            if (rangeNum > 0) unfoldStr += "|";
                            unfoldStr += line[j];
                            rangeNum += 1;
                        }
                    }
                    if (rangeNum > 1) unfoldStr = "(" + unfoldStr + ")";
                    newLine += unfoldStr;
                    if (rangeEnd < line.size()) start = rangeEnd + 1;
                    i = rangeEnd;
                }
            }
            newLine += line.substr(start,(line.size()) - start);
            line = newLine;
            return isRange;
        }

        inline static void pretreatmentLine(std::string & line,bool & isRange){
            zhy::regexp_lexer::removeWhiteSpaceChar(line);
            isRange = zhy::regexp_lexer::unfoldRange(line);
            return;
        }

        inline static bool getRegexpTokens(
            const std::string & line,
            std::vector <zhy::regexp_token> & tokens,
            const std::vector <std::string> & lastRegexpNames,
            bool & needDFA,
            std::string & tableName,
            std::ostream & err = std::cerr
        ){
            size_t start = 0;
            size_t end = line.size();
            zhy::regexp_token token;
            size_t tempCode = 0;
            bool alreadyAssign = false;
            tokens.clear();
            for(size_t i=0;i<line.size();i++){
                if (zhy::regexp_token::isOperator(line[i])){
                    if (alreadyAssign && line[i] == '=') continue;
                    if (i > 0 && line[i - 1] == '\\'){//处理转义
                        size_t escapeNum = 1;
                        for(long long j=(i-2);j>=0;j--){
                            if (line[j] == '\\') escapeNum += 1;
                            else break;
                        }
                        if (escapeNum % 2 == 1) continue;
                    }
                    end = i;
                    bool needEncode = false;
                    if (start != end){
                        std::string word = line.substr(start,(end - start));
                        std::vector <std::string> words;
                        zhy::regexp_lexer::splitString(word,lastRegexpNames,words);
                        for(const std::string & element : words){
                            token.tokenWord = element;
                            token.code = tempCode;
                            needEncode = token.setTokenId();
                            if (token.tokenId == zhy::regexp_token_id::OP_ASSIGNMENT){
                                if (alreadyAssign) token.tokenId = zhy::regexp_token_id::ID;
                            }
                            if (needEncode) tempCode = token.code;
                            tempCode += 1;
                            tokens.emplace_back(token);//标识符
                            if (!needDFA) needDFA = token.tokenId == zhy::regexp_token_id::IDDFA;
                            if (tableName == "") tableName = token.tokenWord;
                            token.clear();
                        }
                    }
                    start = end;
                    end = start + 1;
                    token.tokenWord = line.substr(start,(end - start));
                    token.code = tempCode;
                    needEncode = token.setTokenId();
                    if (needEncode) tempCode = token.code;
                    tempCode += 1;
                    if (token.tokenId == zhy::regexp_token_id::OP_ASSIGNMENT){
                        if (!alreadyAssign) alreadyAssign = true;
                    }
                    tokens.emplace_back(token);//运算符
                    token.clear();
                    start = end;
                    continue;
                }
                else if (i == (line.size() - 1)){
                    end = line.size();
                    if (start != end){
                        std::string word = line.substr(start,(end - start));
                        std::vector <std::string> words;
                        zhy::regexp_lexer::splitString(word,lastRegexpNames,words);
                        for(const std::string & element : words){
                            token.tokenWord = element;
                            token.code = tempCode;
                            bool needEncode = token.setTokenId();
                            if (needEncode) tempCode = token.code;
                            tempCode += 1;
                            tokens.emplace_back(token);//标识符
                            if (!needDFA) needDFA = token.tokenId == zhy::regexp_token_id::IDDFA;
                            if (needDFA){
                                if (tableName == "") tableName = token.tokenWord;
                            }
                            token.clear();
                        }
                    }
                }
            }
            return true;
        }

        inline static bool getAllRegexps(
            std::istream & in,
            std::vector <zhy::regexp_tokens> & regexps,
            bool needDisplay = false,
            std::ostream & out = std::cout,
            std::ostream & err = std::cerr
        ){
            bool succ = true;
            zhy::regexp_tokens tempRegexp;
            std::vector <std::string> lastRegexpNames;
            while (std::getline(in,tempRegexp.regexp_str)){
                try{
                    zhy::regexp_lexer::trimLine(tempRegexp.regexp_str);
                    if (tempRegexp.regexp_str.empty()) break;
                    zhy::regexp_lexer::pretreatmentLine(tempRegexp.regexp_str,tempRegexp.isRange);
                    succ = getRegexpTokens(tempRegexp.regexp_str,tempRegexp.tokens,lastRegexpNames,tempRegexp.needDfa,tempRegexp.tableName,err);
                    lastRegexpNames.emplace_back(tempRegexp.tableName);
                    regexps.emplace_back(tempRegexp);
                    if (needDisplay) tempRegexp.display(out);
                    tempRegexp.clear();
                }
                catch (const std::exception & e){
                    err<<"在进行正则表达式的词法分析时出现了意外的错误, 我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    succ = false;
                    continue;
                }
            }
            return succ;
        }

        inline static bool getFileAlltokens(
            const std::string & filePath,
            std::vector <zhy::regexp_tokens> & regexps,
            bool needDisplay = false,
            std::ostream & out = std::cout,
            std::ostream & err = std::cerr
        ){
            std::filesystem::path path(filePath);
            std::ifstream in(path,std::ios::in);
            if (!in){
                err<<"文件打开失败: "<<filePath<<std::endl;
                return false;
            }
            bool succ = zhy::regexp_lexer::getAllRegexps(in,regexps,needDisplay,out,err);
            in.close();
            return succ;
        }

    };


    class unordered_nfa_graph{

        private:
        std::vector <std::tuple <size_t,std::string,size_t>> edges;
        size_t stateStart;
        size_t stateEnd;
        std::unordered_set <size_t> stateSet;

        public:
        unordered_nfa_graph():edges(),stateStart(0),stateEnd(0),stateSet(){}
        ~unordered_nfa_graph(){}

        unordered_nfa_graph(size_t start,const std::string & id,size_t end){
            addEdge(start,id,end);
            stateStart = start;
            stateEnd = end;
        }

        unordered_nfa_graph(const zhy::unordered_nfa_graph & other){
            edges = other.edges;
            stateStart = other.stateStart;
            stateEnd = other.stateEnd;
            stateSet = other.stateSet;
        }
        unordered_nfa_graph & operator =(const zhy::unordered_nfa_graph & other){
            if (this != &other){
                edges = other.edges;
                stateStart = other.stateStart;
                stateEnd = other.stateEnd;
                stateSet = other.stateSet;
            }
            return *this;
        }

        void clear(){
            edges.clear();
            stateStart = 0;
            stateEnd = 0;
            stateSet.clear();
            return;
        }

        bool isAtom() const{
            return (
                edges.size() == 1 &&
                std::get <0>(edges[0]) == stateStart &&
                std::get <2>(edges[0]) == stateEnd &&
                stateSet.size() == 2
            );
        }

        bool empty(){
            return (
                edges.empty() &&
                stateStart == 0 &&
                stateEnd == 0 &&
                stateSet.empty()
            );
        }

        void getEdges(std::vector <std::tuple <size_t,std::string,size_t>> & value) const{
            value = edges;
            return;
        }

        std::vector <std::tuple <size_t,std::string,size_t>> & getEdges(){
            return edges;
        }

        size_t getStart() const{
            return stateStart;
        }

        size_t getEnd() const{
            return stateEnd;
        }

        size_t getStateNum() const{
            return stateSet.size();
        }

        size_t getMaxState() const{
            auto it = std::max_element(stateSet.begin(),stateSet.end());
            if (it != stateSet.end()) return *it;
            else{
                throw std::runtime_error("无法获取最大值!");
                return std::numeric_limits <size_t>::max();
            }
        }

        size_t getMinState() const{
            auto it = std::min_element(stateSet.begin(),stateSet.end());
            if (it != stateSet.end()) return *it;
            else{
                throw std::runtime_error("无法获取最小值!");
                return 0;
            }
        }

        void display(std::ostream & out = std::cout) const{
            out<<"stateStart : "<<stateStart<<std::endl;
            out<<"stateEnd : "<<stateEnd<<std::endl;
            for(const auto & edge : edges){
                out<<std::get <0>(edge)<<" "<<std::get <1>(edge)<<" "<<std::get <2>(edge)<<std::endl;
            }
            return;
        }

        friend bool operator ==(const zhy::unordered_nfa_graph & a,const zhy::unordered_nfa_graph & b);

        void resetState(size_t start){
            size_t newStart = start + 1;
            std::unordered_map <size_t,size_t> stateMap;
            std::unordered_set <size_t> newStateSet;
            for(size_t state : stateSet){
                if (state != stateStart && state != stateEnd){
                    stateMap.emplace(state,newStart);
                    newStateSet.emplace(newStart);
                    newStart += 1;
                }
            }
            stateMap.emplace(stateStart,start);
            stateMap.emplace(stateEnd,newStart);//始终保证stateStart和stateEnd分别为最小的和最大的
            newStateSet.emplace(start);
            newStateSet.emplace(newStart);
            for(auto & edge : edges){
                std::get <0>(edge) = stateMap[std::get <0>(edge)];
                std::get <2>(edge) = stateMap[std::get <2>(edge)];
            }
            stateStart = stateMap[stateStart];
            stateEnd = stateMap[stateEnd];
            stateSet = newStateSet;
            return;
        }

        void addEdge(size_t start,const std::string & id,size_t end){
            edges.emplace_back(std::make_tuple(start,id,end));
            stateSet.emplace(start);
            stateSet.emplace(end);
            return;
        }

        void append(const zhy::unordered_nfa_graph & other){
            for(const auto & edge : other.edges){
                addEdge(std::get <0>(edge),std::get <1>(edge),std::get <2>(edge));
            }
            return;
        }

        void selfClosure(){
            size_t newStart = getMinState();
            resetState(newStart + 1);
            addEdge(stateEnd,"#",stateStart);
            size_t newEnd = getMaxState() + 1;
            addEdge(newStart,"#",stateStart);
            addEdge(stateEnd,"#",newEnd);
            stateStart = newStart;
            stateEnd = newEnd;
            addEdge(stateStart,"#",stateEnd);
            display();
            return;
        }

        void selfClosurePositive(){
            addEdge(stateEnd,"#",stateStart);
            return;
        }

        void selfOptional(){
            size_t newStart = getMinState();
            resetState(newStart + 1);
            size_t newEnd = getMaxState() + 1;
            addEdge(newStart,"#",stateStart);
            addEdge(stateEnd,"#",newEnd);
            stateStart = newStart;
            stateEnd = newEnd;
            addEdge(stateStart,"#",stateEnd);
            return;
        }

        inline static void connect(const zhy::unordered_nfa_graph & a,zhy::unordered_nfa_graph b,zhy::unordered_nfa_graph & result){
            result = a;
            b.resetState(a.getMaxState() + 1);
            result.addEdge(a.stateEnd,"#",b.stateStart);
            result.append(b);
            result.stateEnd = b.stateEnd;
            return;
        }

        inline static void select(std::vector <zhy::unordered_nfa_graph> & graphs,zhy::unordered_nfa_graph & result){
            if (graphs.size() == 1){
                result = graphs[0];
                return;
            }
            else if (graphs.empty()) return;
            result.clear();
            std::unordered_set <size_t> allStateSet;
            for(const auto & graph : graphs){
                for(const auto & state : graph.stateSet){
                    allStateSet.emplace(state);
                }
            }
            result.stateStart = *std::min_element(allStateSet.begin(),allStateSet.end());
            size_t newStart = result.stateStart + 1;
            std::unordered_set <size_t> ends;
            for(auto & graph : graphs){
                graph.resetState(newStart);
                newStart = graph.getMaxState() + 1;
                result.addEdge(result.stateStart,"#",graph.stateStart);
                result.append(graph);
                ends.emplace(graph.stateEnd);
            }
            result.stateEnd = *std::max_element(ends.begin(),ends.end()) + 1;
            for(size_t end : ends){
                result.addEdge(end,"#",result.stateEnd);
            }
            return;
        }


    };

    inline bool operator ==(const zhy::unordered_nfa_graph & a,const zhy::unordered_nfa_graph & b){
        return (
            a.edges == b.edges &&
            a.stateStart == b.stateStart &&
            a.stateEnd == b.stateEnd &&
            a.stateSet == b.stateSet
        );
    }



    class unordered_nfa_state_transition_table{
        
        public:

        std::string tableName;
        std::unordered_map <
            std::string,
            std::vector <std::unordered_set <size_t>>
        > table;
        size_t stateNum;
        size_t stateStart;
        size_t stateEnd;

        unordered_nfa_state_transition_table():tableName(""),table(),stateNum(0),stateStart(0),stateEnd(0){}
        unordered_nfa_state_transition_table(const std::string & name,const std::unordered_set <std::string> & ids,size_t sn,size_t start,size_t end):tableName(name){
            for(const std::string & id : ids){
                table.emplace(id,std::vector <std::unordered_set <size_t>>(sn,std::unordered_set <size_t>()));
            }
            stateNum = sn;
            if (start >= stateNum) stateStart = stateNum - 1;
            else stateStart = start;
            if (end >= stateNum) stateEnd = stateNum - 1;
            else stateEnd = end;
        }
        unordered_nfa_state_transition_table(const std::string & name,const std::unordered_set <std::string> & ids):tableName(name){
            for(const std::string & id : ids){
                table.emplace(id,std::vector <std::unordered_set <size_t>>());
            }
            stateNum = 0;
            stateStart = 0;
            stateEnd = 0;
        }
        ~unordered_nfa_state_transition_table(){}

        unordered_nfa_state_transition_table(const zhy::unordered_nfa_state_transition_table & other){
            tableName = other.tableName;
            table = other.table;
            stateNum = other.stateNum;
            stateStart = other.stateStart;
            stateEnd = other.stateEnd;
        }

        unordered_nfa_state_transition_table & operator =(const zhy::unordered_nfa_state_transition_table & other){
            if (this != &other){
                tableName = other.tableName;
                table = other.table;
                stateNum = other.stateNum;
                stateStart = other.stateStart;
                stateEnd = other.stateEnd;
            }
            return *this;
        }

        void clear(){
            tableName.clear();
            table.clear();
            stateStart = 0;
            stateEnd = 0;
            stateNum = 0;
            return;
        }

        void setTableName(const std::string & name){
            tableName = name;
            return;
        }

        std::string & getTableName(){
            return tableName;
        }

        size_t getHeadSize() const{
            return table.size();
        }

        size_t getStateNum() const{
            return stateNum;
        }

        void appendState(){
            for(auto & head : table){
                head.second.emplace_back(std::unordered_set <size_t>());
            }
            stateNum += 1;
            return;
        }

        bool setStateStart(size_t start,std::ostream & err = std::cerr){
            if (start >= stateNum){
                err<<"设置失败, 原因是不存在状态编号: "<<start<<std::endl;
                return false;
            }
            stateStart = start;
            return true;
        }

        bool setEndStart(size_t end,std::ostream & err = std::cerr){
            if (end >= stateNum){
                err<<"设置失败, 原因是不存在状态编号: "<<end<<std::endl;
                return false;
            }
            stateEnd = end;
            return true;
        }

        bool addEnd(size_t start,const std::string & id,size_t end,std::ostream & err = std::cerr){
            if (start >= stateNum){
                err<<"添加失败, 原因是不存在状态编号: "<<start<<std::endl;
                return false;
            }
            if (end >= stateNum){
                err<<"添加失败, 原因是不存在状态编号: "<<end<<std::endl;
                return false;
            }
            auto it = table.find(id);
            if (it == table.end()){
                err<<"添加失败, 原因是不存在表头: "<<id<<std::endl;
                return false;
            }
            std::unordered_set <size_t> & element = it->second[start];
            element.emplace(end);
            return true;
        }

        void getValue(std::vector <std::vector <std::string>> & value) const{
            value = std::vector <std::vector <std::string>>(stateNum + 1,std::vector <std::string>(table.size() + 2,""));
            value[stateStart + 1][0] = "-";
            value[stateEnd + 1][0] = "+";
            for(size_t i=1;i<=stateNum;i++){
                value[i][1] = std::to_string(i);
            }
            size_t headIndex = 2;
            for(const auto & head : table){
                value[0][headIndex] = head.first;
                for(size_t i=0;i<stateNum;i++){
                    const std::unordered_set <size_t> & element = head.second[i];
                    if (element.size() > 0){
                        std::ostringstream oss;
                        oss<<"[";
                        for(size_t state : element){
                            oss<<(state + 1)<<" ";
                        }
                        oss<<"]";
                        value[i + 1][headIndex] = oss.str();
                    }
                    else value[i + 1][headIndex] = "none";
                }
                headIndex += 1;
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            std::vector <std::vector <std::string>> value;
            getValue(value);
            for(const auto & row : value){
                for(const std::string & element : row){
                    out<<element<<" ";
                }
                out<<std::endl;
            }
        }



    };



    class nfa_graph{

        private:
        std::vector <std::tuple <size_t,std::string,size_t>> edges;
        size_t stateStart;
        size_t stateEnd;
        std::set <size_t> stateSet;

        public:
        nfa_graph():edges(),stateStart(0),stateEnd(0),stateSet(){}
        ~nfa_graph(){}

        nfa_graph(size_t start,const std::string & id,size_t end){
            addEdge(start,id,end);
            stateStart = start;
            stateEnd = end;
        }

        nfa_graph(const zhy::nfa_graph & other){
            edges = other.edges;
            stateStart = other.stateStart;
            stateEnd = other.stateEnd;
            stateSet = other.stateSet;
        }
        nfa_graph & operator =(const zhy::nfa_graph & other){
            if (this != &other){
                edges = other.edges;
                stateStart = other.stateStart;
                stateEnd = other.stateEnd;
                stateSet = other.stateSet;
            }
            return *this;
        }

        void clear(){
            edges.clear();
            stateStart = 0;
            stateEnd = 0;
            stateSet.clear();
            return;
        }

        bool isAtom() const{
            return (
                edges.size() == 1 &&
                std::get <0>(edges[0]) == stateStart &&
                std::get <2>(edges[0]) == stateEnd &&
                stateSet.size() == 2
            );
        }

        bool empty(){
            return (
                edges.empty() &&
                stateStart == 0 &&
                stateEnd == 0 &&
                stateSet.empty()
            );
        }

        void getEdges(std::vector <std::tuple <size_t,std::string,size_t>> & value) const{
            value = edges;
            return;
        }

        const std::vector <std::tuple <size_t,std::string,size_t>> & getEdges() const{
            return edges;
        }

        size_t getStart() const{
            return stateStart;
        }

        size_t getEnd() const{
            return stateEnd;
        }

        size_t getStateNum() const{
            return stateSet.size();
        }

        size_t getMaxState() const{
            auto it = std::max_element(stateSet.begin(),stateSet.end());
            if (it != stateSet.end()) return *it;
            else{
                throw std::runtime_error("无法获取最大值!");
                return std::numeric_limits <size_t>::max();
            }
        }

        size_t getMinState() const{
            auto it = std::min_element(stateSet.begin(),stateSet.end());
            if (it != stateSet.end()) return *it;
            else{
                throw std::runtime_error("无法获取最小值!");
                return 0;
            }
        }

        void display(std::ostream & out = std::cout) const{
            out<<"stateStart : "<<stateStart<<std::endl;
            out<<"stateEnd : "<<stateEnd<<std::endl;
            for(const auto & edge : edges){
                out<<std::get <0>(edge)<<" "<<std::get <1>(edge)<<" "<<std::get <2>(edge)<<std::endl;
            }
            return;
        }

        friend bool operator ==(const zhy::nfa_graph & a,const zhy::nfa_graph & b);

        void resetState(size_t start){
            size_t newStart = start + 1;
            std::unordered_map <size_t,size_t> stateMap;
            std::set <size_t> newStateSet;
            for(size_t state : stateSet){
                if (state != stateStart && state != stateEnd){
                    stateMap.emplace(state,newStart);
                    newStateSet.emplace(newStart);
                    newStart += 1;
                }
            }
            stateMap.emplace(stateStart,start);
            stateMap.emplace(stateEnd,newStart);//始终保证stateStart和stateEnd分别为最小的和最大的
            newStateSet.emplace(start);
            newStateSet.emplace(newStart);
            for(auto & edge : edges){
                std::get <0>(edge) = stateMap[std::get <0>(edge)];
                std::get <2>(edge) = stateMap[std::get <2>(edge)];
            }
            stateStart = stateMap[stateStart];
            stateEnd = stateMap[stateEnd];
            stateSet = newStateSet;
            return;
        }

        void addEdge(size_t start,const std::string & id,size_t end){
            edges.emplace_back(std::make_tuple(start,id,end));
            stateSet.emplace(start);
            stateSet.emplace(end);
            return;
        }

        void append(const zhy::nfa_graph & other){
            for(const auto & edge : other.edges){
                addEdge(std::get <0>(edge),std::get <1>(edge),std::get <2>(edge));
            }
            return;
        }

        void selfClosure(){
            size_t newStart = getMinState();
            resetState(newStart + 1);
            addEdge(stateEnd,"#",stateStart);
            size_t newEnd = getMaxState() + 1;
            addEdge(newStart,"#",stateStart);
            addEdge(stateEnd,"#",newEnd);
            stateStart = newStart;
            stateEnd = newEnd;
            addEdge(stateStart,"#",stateEnd);
            //display();
            return;
        }

        void selfClosurePositive(){
            addEdge(stateEnd,"#",stateStart);
            return;
        }

        void selfOptional(){
            size_t newStart = getMinState();
            resetState(newStart + 1);
            size_t newEnd = getMaxState() + 1;
            addEdge(newStart,"#",stateStart);
            addEdge(stateEnd,"#",newEnd);
            stateStart = newStart;
            stateEnd = newEnd;
            addEdge(stateStart,"#",stateEnd);
            return;
        }

        inline static void connect(const zhy::nfa_graph & a,zhy::nfa_graph b,zhy::nfa_graph & result){
            result = a;
            b.resetState(a.getMaxState() + 1);
            result.addEdge(a.stateEnd,"#",b.stateStart);
            result.append(b);
            result.stateEnd = b.stateEnd;
            return;
        }

        inline static void select(std::vector <zhy::nfa_graph> & graphs,zhy::nfa_graph & result){
            if (graphs.size() == 1){
                result = graphs[0];
                return;
            }
            else if (graphs.empty()) return;
            result.clear();
            std::unordered_set <size_t> allStateSet;
            for(const auto & graph : graphs){
                for(const auto & state : graph.stateSet){
                    allStateSet.emplace(state);
                }
            }
            result.stateStart = *std::min_element(allStateSet.begin(),allStateSet.end());
            size_t newStart = result.stateStart + 1;
            std::unordered_set <size_t> ends;
            for(auto & graph : graphs){
                graph.resetState(newStart);
                newStart = graph.getMaxState() + 1;
                result.addEdge(result.stateStart,"#",graph.stateStart);
                result.append(graph);
                ends.emplace(graph.stateEnd);
            }
            result.stateEnd = *std::max_element(ends.begin(),ends.end()) + 1;
            for(size_t end : ends){
                result.addEdge(end,"#",result.stateEnd);
            }
            return;
        }


    };

    inline bool operator ==(const zhy::nfa_graph & a,const zhy::nfa_graph & b){
        return (
            a.edges == b.edges &&
            a.stateStart == b.stateStart &&
            a.stateEnd == b.stateEnd &&
            a.stateSet == b.stateSet
        );
    }



    class nfa_state_transition_table{
        
        public:

        std::string tableName;
        std::map <
            std::string,
            std::vector <std::set <size_t>>
        > table;
        size_t stateNum;
        size_t stateStart;
        size_t stateEnd;

        nfa_state_transition_table():tableName(""),table(),stateNum(0),stateStart(0),stateEnd(0){}
        nfa_state_transition_table(const std::string & name,const std::unordered_set <std::string> & ids,size_t sn,size_t start,size_t end):tableName(name){
            for(const std::string & id : ids){
                table.emplace(id,std::vector <std::set <size_t>>(sn,std::set <size_t>()));
            }
            stateNum = sn;
            if (start >= stateNum) stateStart = stateNum - 1;
            else stateStart = start;
            if (end >= stateNum) stateEnd = stateNum - 1;
            else stateEnd = end;
        }
        nfa_state_transition_table(const std::string & name,const std::unordered_set <std::string> & ids):tableName(name){
            for(const std::string & id : ids){
                table.emplace(id,std::vector <std::set <size_t>>());
            }
            stateNum = 0;
            stateStart = 0;
            stateEnd = 0;
        }
        ~nfa_state_transition_table(){}

        nfa_state_transition_table(const zhy::nfa_state_transition_table & other){
            tableName = other.tableName;
            table = other.table;
            stateNum = other.stateNum;
            stateStart = other.stateStart;
            stateEnd = other.stateEnd;
        }

        nfa_state_transition_table & operator =(const zhy::nfa_state_transition_table & other){
            if (this != &other){
                tableName = other.tableName;
                table = other.table;
                stateNum = other.stateNum;
                stateStart = other.stateStart;
                stateEnd = other.stateEnd;
            }
            return *this;
        }

        void clear(){
            tableName.clear();
            table.clear();
            stateStart = 0;
            stateEnd = 0;
            stateNum = 0;
            return;
        }

        void setTableName(const std::string & name){
            tableName = name;
            return;
        }

        std::string & getTableName(){
            return tableName;
        }

        size_t getHeadSize() const{
            return table.size();
        }

        size_t getStateNum() const{
            return stateNum;
        }

        void appendState(){
            for(auto & head : table){
                head.second.emplace_back(std::set <size_t>());
            }
            stateNum += 1;
            return;
        }

        bool setStateStart(size_t start,std::ostream & err = std::cerr){
            if (start >= stateNum){
                err<<"设置失败, 原因是不存在状态编号: "<<start<<std::endl;
                return false;
            }
            stateStart = start;
            return true;
        }

        bool setEndStart(size_t end,std::ostream & err = std::cerr){
            if (end >= stateNum){
                err<<"设置失败, 原因是不存在状态编号: "<<end<<std::endl;
                return false;
            }
            stateEnd = end;
            return true;
        }

        bool addEnd(size_t start,const std::string & id,size_t end,std::ostream & err = std::cerr){
            if (start >= stateNum){
                err<<"添加失败, 原因是不存在状态编号: "<<start<<std::endl;
                return false;
            }
            if (end >= stateNum){
                err<<"添加失败, 原因是不存在状态编号: "<<end<<std::endl;
                return false;
            }
            auto it = table.find(id);
            if (it == table.end()){
                err<<"添加失败, 原因是不存在表头: "<<id<<std::endl;
                return false;
            }
            std::set <size_t> & element = it->second[start];
            element.emplace(end);
            return true;
        }

        void getValue(std::vector <std::vector <std::string>> & value) const{
            value = std::vector <std::vector <std::string>>(stateNum + 1,std::vector <std::string>(table.size() + 2,""));
            value[stateStart + 1][0] = "-";
            value[stateEnd + 1][0] = "+";
            for(size_t i=1;i<=stateNum;i++){
                value[i][1] = std::to_string(i);
            }
            size_t headIndex = 2;
            for(const auto & head : table){
                value[0][headIndex] = head.first;
                for(size_t i=0;i<stateNum;i++){
                    const std::set <size_t> & element = head.second[i];
                    if (element.size() > 0){
                        std::ostringstream oss;
                        oss<<"[";
                        for(size_t state : element){
                            oss<<(state + 1)<<" ";
                        }
                        oss<<"]";
                        value[i + 1][headIndex] = oss.str();
                    }
                    else value[i + 1][headIndex] = "none";
                }
                headIndex += 1;
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            out<<"nfa_graph_name : "<<tableName<<std::endl;
            std::vector <std::vector <std::string>> value;
            getValue(value);
            for(const auto & row : value){
                for(const std::string & element : row){
                    out<<element<<" ";
                }
                out<<std::endl;
            }
        }



    };



    class regexp_nfa_generator{

        private:
        regexp_nfa_generator() = delete;
        ~regexp_nfa_generator() = delete;
        regexp_nfa_generator(const regexp_nfa_generator & other) = delete;
        regexp_nfa_generator & operator =(const regexp_nfa_generator & other) = delete;

        template <class T>
        inline static bool parse(
            std::vector <zhy::regexp_token> & tokens,//token数组
            const size_t start,
            size_t end,
            T & graph//正则表达式转换为的nfa图
        ){
            bool succ = true;
            graph.clear();
            if (start >= tokens.size() || end >= tokens.size() || start > end) return true;
            else if (tokens.size() == 1){
                graph = T(0,tokens[0].tokenWord,1);
                return true;
            }
            std::vector <T> tokenGraphs(tokens.size());
            for(size_t i=start;i<=end;i++){//处理括号中的token
                if (tokens[i].tokenId == zhy::regexp_token_id::OP_BRACKETLEFT){
                    long long bracketNum = 1;
                    size_t left = i;
                    size_t right = end;
                    for(size_t j=(i + 1);j<=end;j++){
                        if (tokens[j].tokenId == zhy::regexp_token_id::OP_BRACKETRIGHT) bracketNum -= 1;
                        else if (tokens[j].tokenId == zhy::regexp_token_id::OP_BRACKETLEFT) bracketNum += 1;
                        if (bracketNum <= 0){
                            right = j;
                            break;
                        }
                    }
                    T bracketGraph;
                    succ = succ && zhy::regexp_nfa_generator::parse <T>(
                        tokens,
                        left + 1,
                        right - 1,
                        bracketGraph
                    );
                    tokens.erase(tokens.begin() + left,tokens.begin() + left + 2);
                    tokenGraphs.erase(tokenGraphs.begin() + left,tokenGraphs.begin() + right);
                    tokens[i].tokenId = zhy::regexp_token_id::ID;
                    tokenGraphs[i] = bracketGraph;
                    size_t eraseNum = right - left;
                    end -= eraseNum;
                }
            }
            for(size_t i=start;i<=end;i++){//处理atom token
                if (tokens[i].isId()){
                    if (tokenGraphs[i].empty()) tokenGraphs[i] = T(i * 2,tokens[i].tokenWord,i * 2 + 1);
                }
            }
            for(size_t i=(start+1);i<=end;i++){//处理单目运算符: 闭包、正闭包、可选
                //tokens[i].display();
                switch (tokens[i].tokenId){
                    case zhy::regexp_token_id::OP_CLOSURE:{
                        tokenGraphs[i - 1].selfClosure();
                        tokens.erase(tokens.begin() + i);
                        tokenGraphs.erase(tokenGraphs.begin() + i);
                        i -= 1;
                        end -= 1;
                        break;
                    }
                    case zhy::regexp_token_id::OP_CLOSUREPOSITIVE:{
                        tokenGraphs[i - 1].selfClosurePositive();
                        tokens.erase(tokens.begin() + i);
                        tokenGraphs.erase(tokenGraphs.begin() + i);
                        i -= 1;
                        end -= 1;
                        break;
                    }
                    case zhy::regexp_token_id::OP_OPTIONAL:{
                        tokenGraphs[i - 1].selfOptional();
                        tokens.erase(tokens.begin() + i);
                        tokenGraphs.erase(tokenGraphs.begin() + i);
                        i -= 1;
                        end -= 1;
                        break;
                    }
                    default: continue;
                }
            }
            for(size_t i=(start + 1);i<=end;i++){//处理连接(并置)运算
                if (tokens[i].isId() && tokens[i - 1].isId()){
                    T result;
                    T::connect(tokenGraphs[i - 1],tokenGraphs[i],result);
                    tokenGraphs[i - 1] = result;
                    tokens.erase(tokens.begin() + i);
                    tokenGraphs.erase(tokenGraphs.begin() + i);
                    i -= 1;
                    end -= 1;
                }
            }
            for(size_t i=(start + 1);i<end;i++){//处理选择运算
                if (
                    tokens[i].tokenId == zhy::regexp_token_id::OP_SELECT &&
                    tokens[i - 1].isId() &&
                    tokens[i + 1].isId()
                ){
                    std::vector <T> selectGraphs;
                    selectGraphs.emplace_back(tokenGraphs[i - 1]);
                    size_t selectEnd = end - 1;
                    for(size_t j=(i + 1);j<end;j++){
                        if (
                            tokens[j].tokenId == zhy::regexp_token_id::OP_SELECT &&
                            tokens[j - 1].isId() &&
                            tokens[j + 1].isId()
                        ){
                            if (
                                tokens[j - 1].isId() &&
                                !tokenGraphs[j - 1].empty()
                            ) selectGraphs.emplace_back(tokenGraphs[j - 1]);
                        }
                        else if (
                            tokens[j].isId() &&
                            tokens[j - 1].tokenId == zhy::regexp_token_id::OP_SELECT &&
                            tokens[j + 1].tokenId == zhy::regexp_token_id::OP_SELECT
                        ){}
                        else{
                            selectEnd = j - 1;
                            break;
                        }
                    }
                    if (
                        tokens[selectEnd + 1].isId() &&
                        !tokenGraphs[selectEnd + 1].empty()
                    ) selectGraphs.emplace_back(tokenGraphs[selectEnd + 1]);
                    T result;
                    T::select(selectGraphs,result);
                    tokenGraphs[i - 1] = result;
                    size_t eraseNum = selectEnd + 2 - i;
                    tokens.erase(tokens.begin() + i,tokens.begin() + selectEnd + 2);
                    tokenGraphs.erase(tokenGraphs.begin() + i,tokenGraphs.begin() + selectEnd + 2);
                    i -= 1;
                    end -= eraseNum;
                }
            }
            graph = tokenGraphs[start];
            graph.resetState(0);
            //graph.display();
            return succ;
        }

/*
regexp -> connect { | connect }
connect -> unary { unary }
unary -> factor [ unary-op ]
unary-op -> * | + | ?
factor -> ( regexp ) | atom
*/

        template <class graphT>
        inline static bool regexp_factor(
            const zhy::regexp_tokens & tokens,
            size_t & current,
            graphT & graph
        ){
            if (current >= tokens.size()) return false;
            if (tokens[current].tokenId == zhy::regexp_token_id::OP_BRACKETLEFT){
                current += 1;
                if (current >= tokens.size()) return false;
                if (!zhy::regexp_nfa_generator::regexp_select(tokens,current,graph)) return false;
                if (current >= tokens.size()) return false;
                if (tokens[current].tokenId != zhy::regexp_token_id::OP_BRACKETRIGHT) return false;
                current += 1;
                return true;
            }
            else if (tokens[current].isId()){
                graph = graphT(0,tokens[current].tokenWord,1);
                current += 1;
                return true;
            }
            else return false;
        }

        template <class graphT>
        inline static bool regexp_unary(
            const zhy::regexp_tokens & tokens,
            size_t & current,
            graphT & graph
        ){
            if (current >= tokens.size()) return false;
            if (!zhy::regexp_nfa_generator::regexp_factor(tokens,current,graph)) return false;
            if (current < tokens.size() && tokens[current].tokenId == zhy::regexp_token_id::OP_CLOSURE){
                current += 1;
                graph.selfClosure();
            }
            else if (current < tokens.size() && tokens[current].tokenId == zhy::regexp_token_id::OP_CLOSUREPOSITIVE){
                current += 1;
                graph.selfClosurePositive();
            }
            else if (current < tokens.size() && tokens[current].tokenId == zhy::regexp_token_id::OP_OPTIONAL){
                current += 1;
                graph.selfOptional();
            }
            //std::cout<<graph.getStateNum()<<std::endl;
            return true;
        }

        template <class graphT>
        inline static bool regexp_connect(
            const zhy::regexp_tokens & tokens,
            size_t & current,
            graphT & graph
        ){
            if (current >= tokens.size()) return false;
            graph.clear();
            graphT tempGraph;
            if (!zhy::regexp_nfa_generator::regexp_unary(tokens,current,tempGraph)) return false;
            //std::cout<<tempGraph.getStateNum()<<std::endl;
            while (current < tokens.size() && tokens[current].isFactorFirst()){
                graphT newTempGraph;
                if (!zhy::regexp_nfa_generator::regexp_unary(tokens,current,newTempGraph)) return false;
                graphT::connect(tempGraph,newTempGraph,graph);
                tempGraph = graph;
                //std::cout<<tempGraph.getStateNum()<<std::endl;
            }
            graph = tempGraph;
            return true;
        }

        template <class graphT>
        inline static bool regexp_select(
            const zhy::regexp_tokens & tokens,
            size_t & current,
            graphT & graph
        ){
            if (current >= tokens.size()) return false;
            std::vector <graphT> graphs;
            graphT tempGraph;
            if (!zhy::regexp_nfa_generator::regexp_connect(tokens,current,tempGraph)) return false;
            //std::cout<<tempGraph.getStateNum()<<std::endl;
            graphs.emplace_back(tempGraph);
            while (current < tokens.size() && tokens[current].tokenId == zhy::regexp_token_id::OP_SELECT){
                current += 1;
                if (current >= tokens.size()) return false;
                if (!zhy::regexp_nfa_generator::regexp_connect(tokens,current,tempGraph)) return false;
                //std::cout<<tempGraph.getStateNum()<<std::endl;
                graphs.emplace_back(tempGraph);
            }
            //std::cout<<graphs.size()<<std::endl;
            /*for(const auto & element : graphs){
                std::cout<<element.getStateNum()<<std::endl;
            }*/
            graphT::select(graphs,graph);
            return true;
        }

        template <class graphT>
        inline static bool parse_recursive_descent(
            const zhy::regexp_tokens & tokens,//token数组
            const size_t start,
            graphT & graph//正则表达式转换为的nfa图
        ){
            graph.clear();
            size_t current = start;
            return zhy::regexp_nfa_generator::regexp_select(tokens,current,graph);
        }

        public:

        template <class T>
        inline static bool regexp_to_nfa_graph(
            std::vector <zhy::regexp_token> tokens,
            T & graph,
            std::ostream & err = std::cerr
        ){
            graph.clear();
            try{
                bool succ = zhy::regexp_nfa_generator::parse <T>(
                    tokens,
                    2,
                    tokens.size() - 1,
                    graph
                );
                return succ;
            }
            catch (const std::exception & e){
                err<<"regexp_to_nfa_graph error: "<<e.what()<<std::endl;
                return false;
            }
        }

        template <class graphT,class tableT>
        inline static bool regexp_to_nfa(
            const zhy::regexp_tokens & tokens,
            graphT & graph,
            tableT & nfa,
            std::ostream & err = std::cerr
        ){
            if (!zhy::regexp_nfa_generator::regexp_to_nfa_graph <graphT>(tokens.tokens,graph,err)) return false;
            std::unordered_set <std::string> heads;
            tokens.getHeads(heads);
            heads.emplace("#");
            //std::cout<<edges.size()<<" "<<heads.size()<<std::endl;
            nfa = tableT(tokens.tableName,heads,graph.getStateNum(),graph.getStart(),graph.getEnd());
            for (const auto & edge : graph.getEdges()){
                nfa.addEnd(std::get <0>(edge),std::get <1>(edge),std::get <2>(edge));
            }
            return true;
        }

        template <class graphT>
        inline static bool regexp_to_nfa_graph_recursive_descent(
            const zhy::regexp_tokens & tokens,
            graphT & graph,
            std::ostream & err = std::cerr
        ){
            graph.clear();
            try{
                return zhy::regexp_nfa_generator::parse_recursive_descent <graphT>(tokens,2,graph);
            }
            catch (const std::exception & e){
                err<<"regexp_to_nfa_graph error: "<<e.what()<<std::endl;
                return false;
            }
        }

        template <class graphT,class tableT>
        inline static bool regexp_to_nfa_recursive_descent(
            const zhy::regexp_tokens & tokens,
            graphT & graph,
            tableT & nfa,
            std::ostream & err = std::cerr
        ){
            if (!zhy::regexp_nfa_generator::regexp_to_nfa_graph_recursive_descent <graphT>(tokens,graph,err)) return false;
            std::unordered_set <std::string> heads;
            tokens.getHeads(heads);
            heads.emplace("#");
            nfa = tableT(tokens.tableName,heads,graph.getStateNum(),graph.getStart(),graph.getEnd());
            for (const auto & edge : graph.getEdges()){
                nfa.addEnd(std::get <0>(edge),std::get <1>(edge),std::get <2>(edge));
            }
            return true;
        }


    };


    template <class graphT,class tableT>
    class regexp_nfa_dfas;

    class min_dfa_state_transition_table;


    class dfa_state_transition_table{
        friend class regexp_nfa_dfas <zhy::unordered_nfa_graph,zhy::unordered_nfa_state_transition_table>;
        friend class regexp_nfa_dfas <zhy::nfa_graph,zhy::nfa_state_transition_table>;
        protected:
        std::string tableName;
        std::map <
            std::string,
            std::map <size_t,size_t>
        > table;
        std::vector <std::set <size_t>> statePosSetMap;
        size_t stateEnd;

        void nfa_to_dfa(
            const std::map <
                std::string,
                std::vector <std::set <size_t>>
            > & nfaTable,
            size_t stateStart
        ){
            table.clear();
            statePosSetMap.clear();
            size_t pos = 1;
            std::map <std::set <size_t>,size_t> stateSetPosMap;
            size_t stateNum = nfaTable.find("#")->second.size();
            std::vector <std::set <size_t>> stateSetMap(stateNum);
            std::unordered_map <size_t,std::tuple <std::string,size_t>> stateStartEndMap;
            for(const auto & column : nfaTable){
                const std::string & id = column.first;
                if (id == "#") continue;
                const std::vector <std::set <size_t>> & endSets = column.second;
                for(size_t i=0;i<endSets.size();i++){
                    const std::set <size_t> & endSet = endSets[i];
                    if (!endSet.empty()){
                        for(size_t element : endSet){
                            stateStartEndMap.emplace(i,std::make_tuple(id,element));
                            std::set <size_t> stateSet;
                            std::queue <size_t> stateQueue;
                            stateQueue.push(element);
                            while (!stateQueue.empty()){//层序遍历找到相同的状态
                                size_t stateStart = stateQueue.front();
                                stateQueue.pop();
                                stateSet.emplace(stateStart);
                                const std::set <size_t> & whiteStateSet = nfaTable.find("#")->second[stateStart];
                                for(size_t end : whiteStateSet){
                                    stateQueue.push(end);
                                }
                            }
                            stateSetPosMap.emplace(stateSet,pos);
                            pos += 1;
                            for(size_t end : stateSet){
                                stateSetMap[end] = stateSet;
                            }
                        }
                    }
                }
            }
            {
                std::set <size_t> stateSet;
                std::queue <size_t> stateQueue;
                stateQueue.push(stateStart);
                while (!stateQueue.empty()){//层序遍历找到相同的状态
                    size_t stateStart = stateQueue.front();
                    stateQueue.pop();
                    stateSet.emplace(stateStart);
                    const std::set <size_t> & whiteStateSet = nfaTable.find("#")->second[stateStart];
                    for(size_t end : whiteStateSet){
                        stateQueue.push(end);
                    }
                }
                stateSetPosMap.emplace(stateSet,0);
                for(size_t end : stateSet){
                    stateSetMap[end] = stateSet;
                }
            }
            /*for(const auto & it : stateStartEndMap){
                size_t start = it.first;
                const std::string & id = std::get <0>(it.second);
                size_t end = std::get <1>(it.second);
                std::cout<<start<<" "<<id<<" "<<end<<std::endl;
            }*/
            for(const auto & column : nfaTable){
                const std::string & id = column.first;
                if (id != "#") table.emplace(
                    id,
                    std::map <size_t,size_t>()
                );
            }
            for(const auto & stateSetIt : stateSetPosMap){
                const std::set <size_t> & stateSet = stateSetIt.first;
                for(size_t state : stateSet){
                    const auto & stateEndIt = stateStartEndMap.find(state);
                    if (stateEndIt != stateStartEndMap.end()){
                        const std::tuple <std::string,size_t> stateEndMap = stateEndIt->second;
                        const std::string endId = std::get <0>(stateEndMap);
                        size_t stateEnd = std::get <1>(stateEndMap);
                        table[endId].emplace(stateSetPosMap[stateSet],stateSetPosMap[stateSetMap[stateEnd]]);
                        //std::cout<<stateSetPosMap[stateSet]<<" "<<stateSetPosMap[stateSetMap[stateEnd]]<<std::endl;
                    }
                }
            }
            statePosSetMap.resize(pos);
            for(const auto & setPos : stateSetPosMap){
                statePosSetMap[setPos.second] = setPos.first;
            }
            return;
        }

        public:

        dfa_state_transition_table():tableName(),table(),statePosSetMap(),stateEnd(0){}
        dfa_state_transition_table(const zhy::nfa_state_transition_table & nfa):tableName(nfa.tableName),stateEnd(nfa.stateEnd){
            this->nfa_to_dfa(
                nfa.table,
                nfa.stateStart
            );
        }
        ~dfa_state_transition_table(){}

        dfa_state_transition_table(const zhy::dfa_state_transition_table & other){
            tableName = other.tableName;
            table = other.table;
            statePosSetMap = other.statePosSetMap;
            stateEnd = other.stateEnd;
        }

        dfa_state_transition_table & operator =(const zhy::dfa_state_transition_table & other){
            if (this != &other){
                tableName = other.tableName;
                table = other.table;
                statePosSetMap = other.statePosSetMap;
                stateEnd = other.stateEnd;
            }
            return *this;
        }

        size_t getStateSetNum() const{
            return statePosSetMap.size();
        }

        size_t getHeadSize() const{
            return table.size();
        }

        void clear(){
            tableName.clear();
            table.clear();
            statePosSetMap.clear();
            stateEnd = 0;
            return;
        }

        virtual void getValue(std::vector <std::vector <std::string>> & value) const{
            value = std::vector <std::vector <std::string>>(getStateSetNum() + 1,std::vector <std::string>(getHeadSize() + 2," "));
            std::vector <std::string> stateSetStr(getStateSetNum());
            std::vector <bool> isEnd(getStateSetNum(),false);
            for(size_t i=1;i<=getStateSetNum();i++){
                std::ostringstream oss;
                const std::set <size_t> & stateSet = statePosSetMap[i - 1];
                oss<<"{";
                for(size_t state : stateSet){
                    oss<<(state + 1)<<" ";
                    if (state == stateEnd) isEnd[i - 1] = true;
                }
                oss<<"}";
                stateSetStr[i - 1] = oss.str();
                value[i][1] = stateSetStr[i - 1];
                for(size_t j=0;j<getHeadSize();j++){
                    value[i][j + 2] = "none";
                }
                if (i == 1) value[i][0] = "-";
                else if (isEnd[i - 1]) value[i][0] = "+";
            }
            size_t idPos = 2;
            for(const auto & columnIt : table){
                const std::string & id = columnIt.first;
                value[0][idPos] = id;
                const std::map <size_t,size_t> & stateMap = columnIt.second;
                for(const auto & stateMapIt : stateMap){
                    size_t stateStart = stateMapIt.first;
                    size_t stateEnd = stateMapIt.second;
                    //std::cout<<stateStart<<" "<<id<<" "<<stateEnd<<std::endl;
                    value[stateStart + 1][idPos] = stateSetStr[stateEnd];
                }
                idPos += 1;
            }
            return;
        }

        virtual void display(std::ostream & out = std::cout) const{
            out<<"dfa_graph_name : "<<tableName<<std::endl;
            std::vector <std::vector <std::string>> value;
            getValue(value);
            for(const auto & row : value){
                for(const std::string & element : row){
                    out<<element<<" ";
                }
                out<<std::endl;
            }
            return;
        }

        const std::map <
            std::string,
            std::map <size_t,size_t>
        > & getTable() const{
            return table;
        }

        void getMinimized(zhy::min_dfa_state_transition_table & minTable) const;


    };


    class min_dfa_state_transition_table : public zhy::dfa_state_transition_table{
        friend zhy::dfa_state_transition_table;
        private:
        std::unordered_set <size_t> finalStates;

        public:

        min_dfa_state_transition_table():zhy::dfa_state_transition_table(),finalStates(){}
        ~min_dfa_state_transition_table(){}

        min_dfa_state_transition_table(const zhy::min_dfa_state_transition_table & other):zhy::dfa_state_transition_table(other){
            finalStates = other.finalStates;
        }

        zhy::min_dfa_state_transition_table & operator =(const zhy::min_dfa_state_transition_table & other){
            if (this != &other){
                zhy::dfa_state_transition_table::operator =(other);
                finalStates = other.finalStates;
            }
            return *this;
        }

        void getValue(std::vector <std::vector <std::string>> & value) const override{
            value = std::vector <std::vector <std::string>>(getStateSetNum() + 1,std::vector <std::string>(getHeadSize() + 2," "));
            std::vector <std::string> stateSetStr(getStateSetNum());
            std::vector <bool> isEnd(getStateSetNum(),false);
            for(size_t i=1;i<=getStateSetNum();i++){
                std::ostringstream oss;
                const std::set <size_t> & stateSet = statePosSetMap[i - 1];
                oss<<"{";
                for(size_t state : stateSet){
                    oss<<(state + 1)<<" ";
                    if (finalStates.find(state) != finalStates.end()) isEnd[i - 1] = true;
                }
                oss<<"}";
                stateSetStr[i - 1] = oss.str();
                value[i][1] = stateSetStr[i - 1];
                for(size_t j=0;j<getHeadSize();j++){
                    value[i][j + 2] = "none";
                }
                if (i == 1) value[i][0] = "-";
                else if (isEnd[i - 1]) value[i][0] = "+";
            }
            size_t idPos = 2;
            for(const auto & columnIt : table){
                const std::string & id = columnIt.first;
                value[0][idPos] = id;
                const std::map <size_t,size_t> & stateMap = columnIt.second;
                for(const auto & stateMapIt : stateMap){
                    size_t stateStart = stateMapIt.first;
                    size_t stateEnd = stateMapIt.second;
                    //std::cout<<stateStart<<" "<<id<<" "<<stateEnd<<std::endl;
                    value[stateStart + 1][idPos] = stateSetStr[stateEnd];
                }
                idPos += 1;
            }
            return;
        }

        void display(std::ostream & out = std::cout) const override{
            out<<"min_dfa_graph_name : "<<tableName<<std::endl;
            std::vector <std::vector <std::string>> value;
            getValue(value);
            for(const auto & row : value){
                for(const std::string & element : row){
                    out<<element<<" ";
                }
                out<<std::endl;
            }
            return;
        }

    };


    inline void zhy::dfa_state_transition_table::getMinimized(zhy::min_dfa_state_transition_table & minTable) const{
        std::set <size_t> finalStateSet;
        std::set <size_t> unfinalStateSet;
        for(size_t i=0;i<statePosSetMap.size();i++){
            if (statePosSetMap[i].find(stateEnd) != statePosSetMap[i].end()) finalStateSet.emplace(i);
            else unfinalStateSet.emplace(i);
        }
        std::deque <std::set <size_t>> stateSetDeque;
        std::unordered_set <size_t> finalStateSets;
        if (unfinalStateSet.empty()){
            stateSetDeque.emplace_back(finalStateSet);
            finalStateSets.emplace(0);
        }
        else{
            stateSetDeque.emplace_back(unfinalStateSet);
            stateSetDeque.emplace_back(finalStateSet);
            finalStateSets.emplace(1);
        }
        std::unordered_map <size_t,size_t> stateSetMap;
        for(size_t i=0;i<stateSetDeque.size();i++){
            for(size_t state : stateSetDeque[i]){
                stateSetMap.emplace(state,i);
            }
        }
        while (true){
            bool changed = false;
            for(size_t k=0;k<stateSetDeque.size();k++){
                std::set <size_t> & stateSet = stateSetDeque[k];
                if (stateSet.size() <= 1){
                    changed = changed || false;
                    continue;
                }
                bool isFinal = finalStateSets.find(k) != finalStateSets.end();
                zhy::disjoint_set <size_t> stateDs;
                for(size_t state : stateSet){
                    stateDs.push(state);
                }
                for(auto i=stateSet.begin();i!=stateSet.end();i++){
                    for(auto j=std::next(i);j!=stateSet.end();j++){
                        size_t ii = *i;
                        size_t jj = *j;
                        std::set <std::tuple <std::string,size_t>> signI;
                        std::set <std::tuple <std::string,size_t>> signJ;
                        for(const auto & column : table){
                            const std::string & id = column.first;
                            const std::map <size_t,size_t> & stateStartEndMap = column.second;
                            auto endItI = stateStartEndMap.find(ii);
                            auto endItJ = stateStartEndMap.find(jj);
                            if (endItI != stateStartEndMap.end()){
                                size_t endISet = stateSetMap.find(endItI->second)->second;
                                signI.emplace(std::make_tuple(id,endISet));
                            }
                            if (endItJ != stateStartEndMap.end()){
                                size_t endJSet = stateSetMap.find(endItJ->second)->second;
                                signJ.emplace(std::make_tuple(id,endJSet));
                            }
                        }
                        /*for(const std::tuple <std::string,size_t> & sign : signI){
                            std::cout<<std::get <0>(sign)<<" "<<std::get <1>(sign)<<" ";
                        }
                        std::cout<<std::endl;
                        for(const std::tuple <std::string,size_t> & sign : signJ){
                            std::cout<<std::get <0>(sign)<<" "<<std::get <1>(sign)<<" ";
                        }
                        std::cout<<std::endl<<(signI == signJ)<<std::endl;*/
                        if (signI == signJ) stateDs.merge(ii,jj);
                    }
                }
                std::vector <std::set <size_t>> sets;
                stateDs.splitToSets(sets);
                if (sets.size() > 1){
                    //std::cout<<"sets.size() > 1"<<std::endl;
                    stateSet = sets[0];
                    for(size_t state : sets[0]){
                        stateSetMap[state] = k;
                        //std::cout<<state<<" ";
                    }
                    //std::cout<<std::endl;
                    for(size_t i=1;i<sets.size();i++){
                        stateSetDeque.emplace_back(sets[i]);
                        for(size_t state : sets[i]){
                            stateSetMap[state] = stateSetDeque.size() - 1;
                            //std::cout<<state<<" ";
                        }
                        //std::cout<<std::endl;
                        if (isFinal) finalStateSets.emplace(stateSetDeque.size() - 1);
                    }
                    changed = true;
                }
                else changed = changed || false;
            }
            if (!changed) break;
        }
        minTable.clear();
        minTable.tableName = tableName;
        //std::sort(stateSetDeque.begin(),stateSetDeque.end());
        minTable.statePosSetMap.resize(stateSetDeque.size());
        for(size_t i=0;i<stateSetDeque.size();i++){
            minTable.statePosSetMap[i] = stateSetDeque[i];
        }
        for(const auto & column : table){
            minTable.table.emplace(column.first,std::map <size_t,size_t>());
        }
        for(const auto & column : table){
            const std::string & id = column.first;
            const std::map <size_t,size_t> & stateStartEndMap = column.second;
            for(const auto & it : stateStartEndMap){
                size_t start = it.first;
                size_t end = it.second;
                minTable.table[id].emplace(stateSetMap[start],stateSetMap[end]);
                //std::cout<<start<<" "<<id<<" "<<end<<std::endl;
                //std::cout<<stateSetMap[start]<<" "<<id<<" "<<stateSetMap[end]<<std::endl;
            }
        }
        /*for(const auto & column : minTable.table){
            const std::string & id = column.first;
            const std::map <size_t,size_t> & stateStartEndMap = column.second;
            for(const auto it : stateStartEndMap){
                size_t start = it.first;
                size_t end = it.second;
                std::cout<<start<<" "<<id<<" "<<end<<std::endl;
            }
        }*/
        minTable.finalStates = finalStateSets;
        return;
    }


    template <class graphT,class tableT>
    class regexp_nfa_dfa{

        public:
        zhy::regexp_tokens regexp;
        std::string regexpTokensStr;
        graphT nfaGraph;
        tableT nfa;
        zhy::dfa_state_transition_table dfa;
        zhy::min_dfa_state_transition_table minDfa;

        regexp_nfa_dfa():regexp(),regexpTokensStr(),nfaGraph(),nfa(),dfa(),minDfa(){}
        ~regexp_nfa_dfa(){}

        regexp_nfa_dfa(const zhy::regexp_nfa_dfa <graphT,tableT> & other){
            regexp = other.regexp;
            regexpTokensStr = other.regexpTokensStr;
            nfaGraph = other.nfaGraph;
            nfa = other.nfa;
            dfa = other.dfa;
            minDfa = other.minDfa;
        }

        regexp_nfa_dfa <graphT,tableT> & operator =(const zhy::regexp_nfa_dfa <graphT,tableT> & other){
            if (this != &other){
                regexp = other.regexp;
                regexpTokensStr = other.regexpTokensStr;
                nfaGraph = other.nfaGraph;
                nfa = other.nfa;
                dfa = other.dfa;
                minDfa = other.minDfa;
            }
            return *this;
        }

        void displayTokens(std::ostream & out = std::cout) const{
            regexp.display(out);
            return;
        }

        void setRegexpTokensStr(){
            std::ostringstream oss;
            regexp.display(oss);
            regexpTokensStr = oss.str();
            return;
        }

        void setMinDfa(){
            dfa.getMinimized(minDfa);
            return;
        }

        bool needDfa() const{
            return regexp.needDfa;
        }

        bool isRange() const{
            return regexp.isRange;
        }

        const std::string & getRegexpName() const{
            return regexp.tableName;
        }

        size_t getCode() const{
            return regexp.getCode();
        }

        void clear(){
            regexp.clear();
            regexpTokensStr.clear();
            nfaGraph.clear();
            nfa.clear();
            dfa.clear();
            minDfa.clear();
            return;
        }

        void selfNfaToMinDfa(){
            dfa = zhy::dfa_state_transition_table(nfa);
            setMinDfa();
            return;
        }


    };


    template <class graphT,class tableT>
    class regexp_nfa_dfas{

        private:
        std::string lexerProgram;

        public:
        std::vector <zhy::regexp_nfa_dfa <graphT,tableT>> tables;
        std::unordered_map <std::string,size_t> nameMap;
        zhy::regexp_nfa_dfa <graphT,tableT> total;

        regexp_nfa_dfas():tables(),nameMap(),lexerProgram(),total(){}
        ~regexp_nfa_dfas(){}

        regexp_nfa_dfas(const regexp_nfa_dfas & other){
            tables = other.tables;
            nameMap = other.nameMap;
            lexerProgram = other.lexerProgram;
            total = other.total;
        }

        regexp_nfa_dfas & operator =(const regexp_nfa_dfas & other){
            if (this != &other){
                tables = other.tables;
                nameMap = other.nameMap;
                lexerProgram = other.lexerProgram;
                total = other.total;
            }
            return *this;
        }

        void clear(){
            tables.clear();
            nameMap.clear();
            lexerProgram.clear();
            total.clear();
            return;
        }

        std::string getLexerName() const{
            return tables.back().getRegexpName() + "_total";
        }

        const std::string & getLexerProgram() const{
            return lexerProgram;
        }

        void setLexerProgram(){
            std::ostringstream oss;
            outputLexerProgram(oss);
            lexerProgram = oss.str();
            return;
        }

        void mergeGraphs(){
            size_t lastRegexp = 0;
            std::vector <graphT> graphCopys;
            std::unordered_set <std::string> heads;
            for(size_t i=0;i<tables.size();i++){
                if (tables[i].needDfa()){
                    graphCopys.emplace_back(tables[i].nfaGraph);
                    tables[i].regexp.getHeads(heads,false);
                    lastRegexp = i;
                }
            }
            //std::cout<<graphCopys.size()<<std::endl;
            total.regexp = tables[lastRegexp].regexp;
            heads.emplace("#");
            graphT::select(graphCopys,total.nfaGraph);
            total.nfa = tableT(getLexerName(),heads,total.nfaGraph.getStateNum(),total.nfaGraph.getStart(),total.nfaGraph.getEnd());
            for (const auto & edge : total.nfaGraph.getEdges()){
                total.nfa.addEnd(std::get <0>(edge),std::get <1>(edge),std::get <2>(edge));
            }
            total.selfNfaToMinDfa();
            return;
        }

        inline static bool isXId(const std::string & id){
            if (id.size() < 1) return false;
            if (zhy::regexp_token::isIdStart(id[0])){
                for(size_t i=1;i<id.size();i++){
                    if (!zhy::regexp_token::isIdContinue(id[i])) return false;
                }
                return true;
            }
            else return false;
        }

        inline static const std::unordered_map <char,std::string> char_to_id = {
            {'+', "_plus"},
            {'-', "_minus"},
            {'*', "_star"},
            {'/', "_slash"},
            {'%', "_percent"},
            {'=', "_equal"},
            {'>', "_greater"},
            {'<', "_less"},
            {'!', "_excl"},
            {'&', "_and"},
            {'|', "_or"},
            {'^', "_xor"},
            {'~', "_tilde"},

            {'(', "_lparen"},
            {')', "_rparen"},
            {'[', "_lbracket"},
            {']', "_rbracket"},
            {'{', "_lbrace"},
            {'}', "_rbrace"},

            {';', "_semi"},
            {':', "_colon"},
            {'"', "_quote"},
            {'\'', "_apost"},
            {',', "_comma"},
            {'.', "_dot"},
            {'?', "_question"},
            {'`', "_backtick"},
            {'#', "_hash"},
            {'@', "_at"},
            {'$', "_dollar"},

            {' ', "_space"},
            {'\t', "_tab"},
            {'\n', "_newline"}
        };

        inline static std::string charToIdentifier(char ch){
            auto it = char_to_id.find(ch);
            if (it != char_to_id.end()){
                return it->second;
            }
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_'){
                return std::string(1,ch);
            }
            return "_" + std::to_string(static_cast <unsigned char>(ch));
        }

        inline static void idLegalize(std::string & id){
            std::string res = "";
            for(const char ch : id){
                res += charToIdentifier(ch);
            }
            if (!res.empty() && isdigit(static_cast <unsigned char>(res[0]))) res = "_" + res;
            id = res;
            return;
        }

        void outputLexerProgram(std::ostream & out = std::cout) const{
            out<<"#include <bits/stdc++.h>"<<std::endl;
            out<<"using namespace std;"<<std::endl<<std::endl;
            out<<"int pos;"<<std::endl;
            out<<"char ch;"<<std::endl;
            out<<"char buffer[255];"<<std::endl;
            out<<"char getNext(){return buffer[pos++];}"<<std::endl<<std::endl;
            size_t getTokenNum = 0;
            for(const auto & rnd : tables){
                if (rnd.isRange()){
                    std::string fName = "judge" + rnd.getRegexpName();
                    if (!zhy::regexp_nfa_dfas <graphT,tableT>::isXId(fName)) zhy::regexp_nfa_dfas <graphT,tableT>::idLegalize(fName);
                    out<<"bool "<<fName<<"(char ch){"<<std::endl;
                    out<<"    switch(ch){"<<std::endl;
                    for(const auto & column : rnd.minDfa.table){
                        const std::string & id = column.first;
                        out<<"        case \'"<<id<<"\':"<<" return true;"<<std::endl;
                    }
                    out<<"        default: return false;"<<std::endl;
                    out<<"    }"<<std::endl;
                    out<<"}"<<std::endl;
                }
                else{
                    if (!rnd.needDfa()){
                        out<<"void getToken"<<(getTokenNum + 1)<<"(){"<<std::endl;
                        out<<"    int state = 1;"<<std::endl;
                        out<<"    bool done = false;"<<std::endl;
                        out<<"    while (!done){"<<std::endl;
                        out<<"        switch (state){"<<std::endl;
                        for(size_t i=0;i<rnd.minDfa.getStateSetNum();i++){
                            out<<"            case "<<(i + 1)<<":{"<<std::endl;
                            bool alreadyIf = false;
                            for(const auto & column : rnd.minDfa.table){
                                const auto & endIt = column.second.find(i);
                                if (endIt != column.second.end()){
                                    std::string f = "judge" + column.first;
                                    if (!zhy::regexp_nfa_dfas <graphT,tableT>::isXId(f)) zhy::regexp_nfa_dfas <graphT,tableT>::idLegalize(f);
                                    size_t end = endIt->second;
                                    std::string ifStr;
                                    if (alreadyIf) ifStr = "else if";
                                    else{
                                        ifStr = "if";
                                        alreadyIf = true;
                                    }
                                    if (end != i){
                                        out<<"                "<<ifStr<<" ("<<f<<"(ch) == 1){"<<std::endl;
                                        out<<"                    state = "<<(end + 1)<<";"<<std::endl;
                                        out<<"                    ch = getNext();"<<std::endl;
                                        out<<"                }"<<std::endl;
                                    }
                                    else{
                                        out<<"                "<<ifStr<<" ("<<f<<"(ch) == 1) ch = getNext();"<<std::endl;
                                    }
                                }
                            }
                            if (i == (rnd.minDfa.getStateSetNum() - 1)){
                                out<<"                else done = true;"<<std::endl;
                            }
                            else{
                                out<<"                else{"<<std::endl;
                                out<<"                    cout<<\"Error!\"<<endl;"<<std::endl;
                                out<<"                    return;"<<std::endl;
                                out<<"                }"<<std::endl;
                            }
                            out<<"            }"<<std::endl;
                        }
                        out<<"        }"<<std::endl;
                        out<<"    }"<<std::endl;
                        out<<"    cout<<\""<<rnd.getCode()<<"\"<<endl;"<<std::endl;
                        out<<"    return;"<<std::endl;
                        out<<"}"<<std::endl;
                        getTokenNum += 1;
                    }
                }
            }
            {
                out<<"void getToken(){"<<std::endl;
                out<<"    int state = 1;"<<std::endl;
                out<<"    bool done = false;"<<std::endl;
                out<<"    while (!done){"<<std::endl;
                out<<"        switch (state){"<<std::endl;
                for(size_t i=0;i<total.minDfa.getStateSetNum();i++){
                    out<<"            case "<<(i + 1)<<":{"<<std::endl;
                    bool alreadyIf = false;
                    for(const auto & column : total.minDfa.table){
                        const auto & endIt = column.second.find(i);
                        if (endIt != column.second.end()){
                            std::string f = "judge" + column.first;
                            if (!zhy::regexp_nfa_dfas <graphT,tableT>::isXId(f)) zhy::regexp_nfa_dfas <graphT,tableT>::idLegalize(f);
                            size_t end = endIt->second;
                            std::string ifStr;
                            if (alreadyIf) ifStr = "else if";
                            else{
                                ifStr = "if";
                                alreadyIf = true;
                            }
                            if (end != i){
                                out<<"                "<<ifStr<<" ("<<f<<"(ch) == 1){"<<std::endl;
                                out<<"                    state = "<<(end + 1)<<";"<<std::endl;
                                out<<"                    ch = getNext();"<<std::endl;
                                out<<"                }"<<std::endl;
                            }
                            else{
                                out<<"                "<<ifStr<<" ("<<f<<"(ch) == 1) ch = getNext();"<<std::endl;
                            }
                        }
                    }
                    if (i == (total.minDfa.getStateSetNum() - 1)){
                        out<<"                else done = true;"<<std::endl;
                    }
                    else{
                        out<<"                else{"<<std::endl;
                        out<<"                    cout<<\"Error!\"<<endl;"<<std::endl;
                        out<<"                    return;"<<std::endl;
                        out<<"                }"<<std::endl;
                    }
                    out<<"            }"<<std::endl;
                }
                out<<"        }"<<std::endl;
                out<<"    }"<<std::endl;
                out<<"    cout<<\""<<total.getCode()<<"\"<<endl;"<<std::endl;
                out<<"    return;"<<std::endl;
                out<<"}"<<std::endl;
            }
            out<<std::endl<<"int main()"<<std::endl;
            out<<"{"<<std::endl;
            out<<"    cin.getline(buffer,255);"<<std::endl;
            out<<"    pos = 0;"<<std::endl;
            out<<"    ch = getNext();"<<std::endl;
            for(size_t i=0;i<getTokenNum;i++){
                out<<"    getToken"<<(i + 1)<<"();"<<std::endl;
            }
            out<<"    getToken();"<<std::endl;
            out<<"    return 0;"<<std::endl;
            out<<"}"<<std::endl;
            return;
        }

        bool outputFileLexerProgram(
            const std::string & filePath = ".",
            const std::string & fileName = "untitled",
            std::ostream & err = std::cerr
        ){
            std::string name = fileName + ".cpp";
            std::filesystem::path outputPath = std::filesystem::path(filePath) / std::filesystem::path(name);
            std::ofstream out(outputPath,std::ios::out);
            if (!out){
                err<<"文件打开失败: "<<outputPath<<std::endl;
                return false;
            }
            outputLexerProgram(out);
            out.close();
            return true;
        }



    };

    class regexp_nfa_dfa_lexer_generator{

        private:
        regexp_nfa_dfa_lexer_generator() = delete;
        ~regexp_nfa_dfa_lexer_generator() = delete;
        regexp_nfa_dfa_lexer_generator(const regexp_nfa_dfa_lexer_generator & other) = delete;
        regexp_nfa_dfa_lexer_generator & operator =(const regexp_nfa_dfa_lexer_generator & other) = delete;

        public:

        template <class graphT,class tableT>
        inline static bool getRegexpsNfasDfasLexers(
            std::istream & in,
            zhy::regexp_nfa_dfas <graphT,tableT> & rndl,
            std::ostream & out = std::cout,
            std::ostream & err = std::cerr
        ){
            rndl.clear();
            std::vector <zhy::regexp_tokens> regexps;
            bool succ = zhy::regexp_lexer::getAllRegexps(in,regexps,false,out,err);//词法分析
            rndl.tables.resize(regexps.size());
            for(size_t i=0;i<regexps.size();i++){
                try{
                    rndl.nameMap.emplace(regexps[i].tableName,i);
                    rndl.tables[i].regexp = regexps[i];
                    rndl.tables[i].setRegexpTokensStr();
                    succ = succ && zhy::regexp_nfa_generator::regexp_to_nfa <graphT,tableT>(rndl.tables[i].regexp,rndl.tables[i].nfaGraph,rndl.tables[i].nfa,err);
                    rndl.tables[i].dfa = zhy::dfa_state_transition_table(rndl.tables[i].nfa);
                    rndl.tables[i].setMinDfa();
                }
                catch (const std::exception & e){
                    err<<"在进行正则表达式向词法分析程序的转换时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    succ = false;
                }
            }
            rndl.mergeGraphs();
            rndl.setLexerProgram();
            return succ;
        }

        template <class graphT,class tableT>
        inline static bool getRegexpsNfasDfasLexers(
            const std::string & filePath,
            zhy::regexp_nfa_dfas <graphT,tableT> & rndl,
            std::ostream & out = std::cout,
            std::ostream & err = std::cerr
        ){
            rndl.clear();
            std::vector <zhy::regexp_tokens> regexps;
            bool succ = zhy::regexp_lexer::getFileAlltokens(filePath,regexps,false,out,err);//词法分析
            rndl.tables.resize(regexps.size());
            for(size_t i=0;i<regexps.size();i++){
                try{
                    rndl.nameMap.emplace(regexps[i].tableName,i);
                    rndl.tables[i].regexp = regexps[i];
                    rndl.tables[i].setRegexpTokensStr();
                    succ = succ && zhy::regexp_nfa_generator::regexp_to_nfa <graphT,tableT>(rndl.tables[i].regexp,rndl.tables[i].nfaGraph,rndl.tables[i].nfa,err);
                    rndl.tables[i].dfa = zhy::dfa_state_transition_table(rndl.tables[i].nfa);
                    rndl.tables[i].setMinDfa();
                }
                catch (const std::exception & e){
                    err<<"在进行正则表达式向词法分析程序的转换时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    succ = false;
                }
            }
            rndl.mergeGraphs();
            rndl.setLexerProgram();
            return succ;
        }

        template <class graphT,class tableT>
        inline static bool getRegexpsNfasDfasLexers_recursive_descent(
            std::istream & in,
            zhy::regexp_nfa_dfas <graphT,tableT> & rndl,
            std::ostream & out = std::cout,
            std::ostream & err = std::cerr
        ){
            rndl.clear();
            std::vector <zhy::regexp_tokens> regexps;
            bool succ = zhy::regexp_lexer::getAllRegexps(in,regexps,false,out,err);//词法分析
            rndl.tables.resize(regexps.size());
            for(size_t i=0;i<regexps.size();i++){
                try{
                    rndl.nameMap.emplace(regexps[i].tableName,i);
                    rndl.tables[i].regexp = regexps[i];
                    rndl.tables[i].setRegexpTokensStr();
                    succ = succ && zhy::regexp_nfa_generator::regexp_to_nfa_recursive_descent <graphT,tableT>(rndl.tables[i].regexp,rndl.tables[i].nfaGraph,rndl.tables[i].nfa,err);
                    rndl.tables[i].dfa = zhy::dfa_state_transition_table(rndl.tables[i].nfa);
                    rndl.tables[i].setMinDfa();
                }
                catch (const std::exception & e){
                    err<<"在进行正则表达式向词法分析程序的转换时遇到了未知的问题, 我们正在努力解决。"<<std::endl;
                    err<<e.what()<<std::endl;
                    succ = false;
                }
            }
            rndl.mergeGraphs();
            rndl.setLexerProgram();
            return succ;
        }



    };


    class regexp_tester{

        private:

        regexp_tester() = delete;
        ~regexp_tester() = delete;
        regexp_tester(const regexp_tester & other) = delete;
        regexp_tester & operator =(const regexp_tester & other) = delete;

        public:

        inline static void original_test(){
            zhy::regexp_nfa_dfas <zhy::nfa_graph,zhy::nfa_state_transition_table> rndl;
            zhy::regexp_nfa_dfa_lexer_generator::getRegexpsNfasDfasLexers <zhy::nfa_graph,zhy::nfa_state_transition_table>(std::cin,rndl,std::cout,std::cerr);
            for(const auto & rnd : rndl.tables){
                rnd.displayTokens();
                rnd.nfa.display();
                rnd.dfa.display();
                rnd.minDfa.display();
            }
            rndl.total.displayTokens();
            rndl.total.nfa.display();
            rndl.total.dfa.display();
            rndl.total.minDfa.display();
            rndl.outputLexerProgram();
            return;
        }

        inline static void recursive_descent_test(){
            zhy::regexp_nfa_dfas <zhy::nfa_graph,zhy::nfa_state_transition_table> rndl;
            zhy::regexp_nfa_dfa_lexer_generator::getRegexpsNfasDfasLexers_recursive_descent <zhy::nfa_graph,zhy::nfa_state_transition_table>(std::cin,rndl,std::cout,std::cerr);
            for(const auto & rnd : rndl.tables){
                rnd.displayTokens();
                rnd.nfa.display();
                rnd.dfa.display();
                rnd.minDfa.display();
            }
            rndl.total.displayTokens();
            rndl.total.nfa.display();
            rndl.total.dfa.display();
            rndl.total.minDfa.display();
            rndl.outputLexerProgram();
            return;
        }


    };

}

namespace std{
    template <>
    struct hash <zhy::regexp_token>{
        inline size_t operator()(const zhy::regexp_token & token) const{
            return {
                (std::hash <std::string>()(token.tokenWord) + token.code) ^
                static_cast <size_t>(token.tokenId)
            };
        }
    };
}

#endif