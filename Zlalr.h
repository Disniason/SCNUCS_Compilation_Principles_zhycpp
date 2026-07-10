#ifndef Z_LALR_H
#define Z_LALR_H
#pragma once

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <cstddef>

namespace zhy{

    class grammar_token_getter{

        private:

        grammar_token_getter(const grammar_token_getter & other) = delete;
        grammar_token_getter & operator =(const grammar_token_getter & other) = delete;

        public:

        enum class grammar_token_id{
            NONE,

            ENTER,

            OP_ASSIGNMENT,
            OP_SELECT,

            TM_IDENTIFIER,
            TM_NUMBER,
            TM_NULLSTRING,
            TM_BRACKETLEFT,
            TM_BRACKETRIGHT,
            TM_OP,
            TM_KW,

            NTM

        };

        inline static const std::unordered_map <std::string,grammar_token_id> operator_id_map = {
            {"->",grammar_token_id::OP_ASSIGNMENT},
            {"|",grammar_token_id::OP_SELECT},

        };

        inline static const std::unordered_map <std::string,grammar_token_id> terminal_id_map = {
            {"identifier",grammar_token_id::TM_IDENTIFIER},
            {"number",grammar_token_id::TM_NUMBER},
            {"n",grammar_token_id::TM_NUMBER},
            {"#",grammar_token_id::TM_NULLSTRING},
            {"(",grammar_token_id::TM_BRACKETLEFT},
            {")",grammar_token_id::TM_BRACKETRIGHT},
        };

        inline static const std::unordered_map <grammar_token_id,std::string> id_display_map = {
            {grammar_token_id::NONE,"未知的token"},
            {grammar_token_id::OP_ASSIGNMENT,"运算符(赋值)"},
            {grammar_token_id::OP_SELECT,"运算符(选择)"},
            {grammar_token_id::TM_IDENTIFIER,"终结符(标识符)"},
            {grammar_token_id::TM_NUMBER,"终结符(数字)"},
            {grammar_token_id::TM_NULLSTRING,"终结符(空串)"},
            {grammar_token_id::TM_BRACKETLEFT,"终结符(左括号'(')"},
            {grammar_token_id::TM_BRACKETRIGHT,"终结符(右括号')')"},
            {grammar_token_id::TM_OP,"终结符(来自用户输入的运算符)"},
            {grammar_token_id::TM_KW,"终结符(来自用户输入的关键字)"},
            {grammar_token_id::NTM,"非终结符"},
        };

        class grammar_token{

            public:

            grammar_token_id tokenId;
            std::string tokenWord;

            grammar_token():tokenId(grammar_token_id::NONE),tokenWord(){}
            grammar_token(grammar_token_id i,const std::string & w):tokenId(i),tokenWord(w){}
            ~grammar_token(){}

            grammar_token(const grammar_token & other){
                tokenId = other.tokenId;
                tokenWord = other.tokenWord;
            }

            grammar_token & operator =(const grammar_token & other){
                if (this != &other){
                    tokenId = other.tokenId;
                    tokenWord = other.tokenWord;
                }
                return *this;
            }

            bool operator ==(const grammar_token & other) const{
                return (
                    this->tokenId == other.tokenId &&
                    this->tokenWord == other.tokenWord
                );
            }

            void clear(){
                tokenId = grammar_token_id::NONE;
                tokenWord.clear();
                return;
            }

            bool isEmpty() const{
                return (
                    tokenId == grammar_token_id::NONE &&
                    tokenWord.empty()
                );
            }

            const std::string & getWord() const{
                return tokenWord;
            }

            bool isEnter() const{
                return tokenId == grammar_token_id::ENTER;
            }

            bool isSelect() const{
                return tokenId == grammar_token_id::OP_SELECT;
            }

            bool isOperator() const{
                return (tokenId >= grammar_token_id::OP_ASSIGNMENT && tokenId < grammar_token_id::TM_IDENTIFIER);
            }

            bool isTerminal() const{
                return (tokenId >= grammar_token_id::TM_IDENTIFIER && tokenId < grammar_token_id::NTM);
            }

            bool isNonTerminal() const{
                return tokenId == grammar_token_id::NTM;
            }

            bool isNullString() const{
                return (
                    tokenId == grammar_token_id::TM_NULLSTRING &&
                    tokenWord == "#"
                );
            }

            void NTM_to_TM_KW(){
                if (tokenId == grammar_token_id::NTM) tokenId = grammar_token_id::TM_KW;
                return;
            }

            void display(std::ostream & out = std::cout) const{
                if (isEnter()) return;
                out<<tokenWord<<" : ";
                out<<id_display_map.find(tokenId)->second<<std::endl;
                return;
            }

            inline static bool isNTMStart(char input = ' '){
                return (
                    std::isalpha(static_cast <unsigned char>(input)) ||
                    input == '_'
                );
            }

            inline static bool isNTMContinue(char input = ' '){
                return (
                    std::isalnum(static_cast <unsigned char>(input)) ||
                    input == '_' ||
                    input == '-'
                );
            }

            inline static bool isNTMEnd(char input = ' '){
                return (
                    std::isalnum(static_cast <unsigned char>(input)) ||
                    input == '_'
                );
            }

            inline static bool isSymbol(char input = ' '){
                unsigned char sym = static_cast <unsigned char>(input);
                return (
                    std::isprint(sym) &&
                    !grammar_token::isWhiteSpaceChar(input) &&
                    !std::isalnum(sym) &&
                    input != '|'
                );
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

            inline static bool isOperator(const std::string & input = ""){
                return operator_id_map.find(input) != operator_id_map.end();
            }

            inline static bool isTerminalKeyword(const std::string & input = ""){
                return terminal_id_map.find(input) != terminal_id_map.end();
            }

        };

        std::string filePath;
        std::ifstream in;
        std::deque <grammar_token> tokens;

        grammar_token_getter(const std::string & fp,std::ostream & err = std::cerr):filePath(fp),tokens(){
            in.open(std::filesystem::path(filePath),std::ios::in);
            if (!in) err<<"打开文件失败: "<<fp<<std::endl;
        }

        ~grammar_token_getter(){
            if (in.is_open()) in.close();
        }

        bool getLineTokens(const std::string & line){
            grammar_token tempToken;
            for(size_t i=0;i<line.size();i++){
                if (grammar_token::isWhiteSpaceChar(line[i])) continue;
                else if (line[i] == '|'){//注意这里不存在转义，因为转义的情况已经被处理过
                    tempToken.tokenWord = line.substr(i,1);
                    tempToken.tokenId = grammar_token_id::OP_SELECT;
                    tokens.emplace_back(tempToken);
                    tempToken.clear();
                }
                else if (grammar_token::isNTMStart(line[i])){
                    size_t start = i;
                    size_t end = line.size();
                    for(size_t j=(start + 1);j<line.size();j++){
                        if (line[j] == '-'){
                            if (!grammar_token::isNTMContinue(line[j + 1])){
                                end = j;
                                break;
                            }
                        }
                        else if (!grammar_token::isNTMContinue(line[j])){
                            end = j;
                            break;
                        }
                    }
                    tempToken.tokenWord = line.substr(start,(end - start));
                    if (grammar_token::isTerminalKeyword(tempToken.tokenWord)) tempToken.tokenId = terminal_id_map.find(tempToken.tokenWord)->second;
                    else tempToken.tokenId = grammar_token_id::NTM;
                    tokens.emplace_back(tempToken);
                    tempToken.clear();
                    i = end - 1;//保证不重复遍历
                }
                else if (grammar_token::isSymbol(line[i])){
                    size_t start = i;
                    size_t end = line.size();
                    for(size_t j=(start + 1);j<line.size();j++){
                        if (line[j] == '|'){
                            size_t escapeNum = 0;
                            for(size_t k=(j - 1);k>=0;k--){//计算转义符号的数量判断'|'是否被转义
                                if (line[k] == '\\') escapeNum += 1;
                                else break;
                            }
                            if (escapeNum % 2 == 0){//'|'没有被转移，直接终止
                                end = j;
                                break;
                            }
                        }
                        else if (!grammar_token::isSymbol(line[j])){
                            end = j;
                            break;
                        }
                    }
                    tempToken.tokenWord = line.substr(start,(end - start));
                    if (grammar_token::isOperator(tempToken.tokenWord)) tempToken.tokenId = operator_id_map.find(tempToken.tokenWord)->second;
                    else if (grammar_token::isTerminalKeyword(tempToken.tokenWord)) tempToken.tokenId = terminal_id_map.find(tempToken.tokenWord)->second;
                    else tempToken.tokenId = grammar_token_id::TM_OP;
                    tokens.emplace_back(tempToken);
                    tempToken.clear();
                    i = end - 1;//保证不重复遍历
                }
                else{
                    tempToken.tokenWord = line.substr(i,1);
                    tempToken.tokenId = grammar_token_id::NONE;
                    tokens.emplace_back(tempToken);
                    tempToken.clear();
                }
            }
            tokens.emplace_back(grammar_token(grammar_token_id::ENTER,""));//一行结束标志
            return true;
        }

        bool getAllTokens(std::ostream & err = std::cerr){
            std::string line;
            bool succ = true;
            while (std::getline(in,line)){
                try{
                    trimLine(line);
                    if (!line.empty()) succ = succ && getLineTokens(line);
                }
                catch (const std::exception & e){
                    err<<e.what()<<std::endl;
                    succ = succ && false;
                    continue;
                }
            }
            return succ;
        }

        void restart(){
            if (in.is_open()){
                in.clear();
                in.seekg(0);
            }
            tokens.clear();
            return;
        }

        void displayAllTokens(std::ostream & out = std::cout) const{
            out<<"文件: "<<filePath<<std::endl;
            for(const auto & token : tokens){
                token.display(out);
            }
            return;
        }

        inline static void trimLine(std::string & line){//去除行首尾空字符
            line.erase(0,line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            return;
        }





    };

    class grammar_first_follow_set{

        public:

        std::string grammarStart;

        std::unordered_map <
            std::string,
            std::vector <
                std::vector <zhy::grammar_token_getter::grammar_token>
            >
        > non_terminal_token_map;
        bool expanded;

        void clearTokenMap(){
            non_terminal_token_map.clear();
            return;
        }

        void expandGrammar(){
            if (expanded) return;
            std::string newGrammarStart = grammarStart + "\'";
            non_terminal_token_map.emplace(
                newGrammarStart,
                std::vector <std::vector <zhy::grammar_token_getter::grammar_token>>(
                    1,
                    std::vector <zhy::grammar_token_getter::grammar_token>(
                        1,
                        zhy::grammar_token_getter::grammar_token(zhy::grammar_token_getter::grammar_token_id::NTM,grammarStart)
                    )
                )
            );
            grammarStart = newGrammarStart;
            expanded = true;
            return;
        }

        void getNonTerminalTokenMap(const zhy::grammar_token_getter & gtg){
            for(size_t i=0;i<gtg.tokens.size();i++){
                if (i == 0 || gtg.tokens[i].isEnter()){
                    size_t start = i == 0 ? i : i + 1;
                    if (start >= gtg.tokens.size()) break;
                    size_t end = gtg.tokens.size();
                    const std::string & nonTerminal = gtg.tokens.at(start).getWord();
                    std::vector <zhy::grammar_token_getter::grammar_token> newGrammar;
                    for(size_t j=(start + 2);j<gtg.tokens.size();j++){//跳过"->"
                        if (gtg.tokens[j].isSelect()){
                            non_terminal_token_map[nonTerminal].emplace_back(newGrammar);
                            newGrammar.clear();
                        }
                        else if (gtg.tokens[j].isEnter()){
                            non_terminal_token_map[nonTerminal].emplace_back(newGrammar);
                            newGrammar.clear();
                            end = j;
                            break;
                        }
                        else{
                            newGrammar.emplace_back(gtg.tokens[j]);
                        }
                    }
                    i = end - 1;
                }
            }
            for(auto & element : non_terminal_token_map){
                for(auto & row : element.second){
                    for(auto & column : row){
                        if (column.isNonTerminal()){
                            if (non_terminal_token_map.find(column.getWord()) == non_terminal_token_map.end()){
                                column.NTM_to_TM_KW();//把没有出现在非终结符集合中的非终结符改为用户输入的关键字
                            }
                        }
                    }
                }
            }
            return;
        }

        size_t getTokenStringWithPoint(std::string & tokenStr,const std::string & nonTerminal,size_t grammarPosition,size_t pointPosition) const{
            auto it = non_terminal_token_map.find(nonTerminal);
            if (it == non_terminal_token_map.end()) return std::numeric_limits <size_t>:: max();
            if (grammarPosition >= it->second.size()) return std::numeric_limits <size_t>:: max();
            tokenStr.clear();
            const auto & tokens = it->second.at(grammarPosition);
            tokenStr += nonTerminal + " -> ";
            for(size_t i=0;i<tokens.size();i++){
                //if (i == (pointPosition - 2)) tokenStr += ".";
                if (i == pointPosition) tokenStr += ".";
                tokenStr += tokens[i].getWord() + " ";
            }
            if (pointPosition >= tokens.size()) tokenStr += ".";
            return pointPosition;
        }

        std::unordered_map <
            std::string,
            std::unordered_set <std::string>
        > non_terminal_first_set;

        void clearFirstSet(){
            non_terminal_first_set.clear();
            return;
        }

        void getFirstSet(const std::string & nonTerminal,std::unordered_set <std::string> & firstSet){//递归查找first集元素
            for(const auto & row : non_terminal_token_map[nonTerminal]){
                if (!row.empty()){
                    const std::string & firstWord = row[0].getWord();
                    if (row[0].isTerminal()) firstSet.emplace(firstWord);
                    else if (row[0].isNonTerminal() && firstWord != nonTerminal) this->getFirstSet(firstWord,firstSet);//防止死循环
                }
            }
            return;
        }

        void getNonTerminalFirstSet(){
            clearFirstSet();
            for(const auto & element : non_terminal_token_map){
                std::string nonTerminal = element.first;
                this->getFirstSet(nonTerminal,non_terminal_first_set[nonTerminal]);
            }
            return;
        }

        bool canEmpty(const std::string & nonTerminal) const{
            auto it = non_terminal_token_map.find(nonTerminal);
            if (it == non_terminal_token_map.end()) return false;
            const auto & nonTerminalExps = it->second;
            zhy::grammar_token_getter::grammar_token nullString(zhy::grammar_token_getter::grammar_token_id::TM_NULLSTRING,"#");
            auto found = std::find(nonTerminalExps.begin(),nonTerminalExps.end(),std::vector <zhy::grammar_token_getter::grammar_token>{nullString});
            return found != nonTerminalExps.end();
        }
        
        std::unordered_map <
            std::string,
            std::unordered_set <std::string>
        > non_terminal_follow_set;

        void clearFollowSet(){
            non_terminal_follow_set.clear();
            return;
        }

        /*
        void getFollowSet(const std::string & nonTerminal,std::unordered_set <std::string> & followSet){

        }*/

        void getNonTerminalFollowSet(){
            clearFollowSet();
            non_terminal_follow_set[grammarStart].emplace("$");
            while (true){
                bool changed = false;
                for(const auto & element : non_terminal_token_map){
                    for(const auto & row : element.second){
                        for(size_t i=0;i<row.size();i++){
                            if (i < (row.size() - 1)){
                                if (row[i].isNonTerminal()){
                                    const std::string & nonTerminal = row[i].getWord();
                                    auto & followSet = non_terminal_follow_set[nonTerminal];
                                    followSet.erase("#");
                                    size_t oldLength = followSet.size();
                                    size_t nextIndex = i + 1;
                                    while (nextIndex < row.size()){
                                        if (row[nextIndex].isNonTerminal()){
                                            const auto & firstSet = non_terminal_first_set[row[nextIndex].getWord()];
                                            followSet.insert(firstSet.begin(),firstSet.end());
                                            if (this->canEmpty(row[nextIndex].getWord())){
                                                nextIndex += 1;
                                                continue;
                                            }
                                            else break;
                                        }
                                        else if (row[nextIndex].isTerminal()){
                                            if (row[nextIndex].isNullString()){
                                                nextIndex += 1;
                                                continue;
                                            }
                                            followSet.emplace(row[nextIndex].getWord());
                                            break;
                                        }
                                    }
                                    followSet.erase("#");
                                    size_t newLength = followSet.size();
                                    changed = changed || (oldLength != newLength);
                                }
                            }
                            else{
                                if (row[i].isNonTerminal()){
                                    const auto & nonTerminalFollowSet = non_terminal_follow_set[element.first];
                                    std::string nonTerminal = row[i].getWord();
                                    auto & followSet = non_terminal_follow_set[nonTerminal];
                                    followSet.erase("#");
                                    size_t oldLength = followSet.size();
                                    followSet.insert(nonTerminalFollowSet.begin(),nonTerminalFollowSet.end());
                                    followSet.erase("#");
                                    size_t newLength = followSet.size();
                                    changed = changed || (oldLength != newLength);
                                }
                            }
                        }
                    }
                }
                if (!changed) break;
            }
            return;
        }

        grammar_first_follow_set():
            grammarStart(),non_terminal_token_map(),expanded(0),
            non_terminal_first_set(),non_terminal_follow_set(){}
        grammar_first_follow_set(const zhy::grammar_token_getter & gtg):grammarStart(){
            if (!gtg.tokens.empty()) grammarStart = gtg.tokens[0].getWord();
            expanded = false;
            non_terminal_token_map.clear();
            getNonTerminalTokenMap(gtg);
            non_terminal_first_set.clear();
            non_terminal_follow_set.clear();
            getNonTerminalFirstSet();
            getNonTerminalFollowSet();
        }
        ~grammar_first_follow_set(){}

        grammar_first_follow_set(const grammar_first_follow_set & other){
            grammarStart = other.grammarStart;
            non_terminal_token_map = other.non_terminal_token_map;
            non_terminal_first_set = other.non_terminal_first_set;
            non_terminal_follow_set = other.non_terminal_follow_set;
            expanded = other.expanded;
        }

        grammar_first_follow_set & operator =(const grammar_first_follow_set & other){
            if (this != & other){
                grammarStart = other.grammarStart;
                non_terminal_token_map = other.non_terminal_token_map;
                non_terminal_first_set = other.non_terminal_first_set;
                non_terminal_follow_set = other.non_terminal_follow_set;
                expanded = other.expanded;
            }
            return *this;
        }

        void clear(){
            grammarStart.clear();
            clearTokenMap();
            clearFirstSet();
            clearFollowSet();
            expanded = false;
            return;
        }

        size_t getNonTerminalNum(){
            return non_terminal_first_set.size();
        }

        bool inFisrtSet(const std::string & nonTerminal,const std::string & terminal) const{
            auto it = non_terminal_first_set.find(nonTerminal);
            if (it == non_terminal_first_set.end()) return false;
            const auto & firstSet = it->second;
            return firstSet.find(terminal) != firstSet.end();
        }

        bool inFollowSet(const std::string & nonTerminal,const std::string & terminal) const{
            auto it = non_terminal_follow_set.find(nonTerminal);
            if (it == non_terminal_follow_set.end()) return false;
            const auto & followSet = it->second;
            return followSet.find(terminal) != followSet.end();
        }

        bool firstSetSameElementExists(const std::string & nt1,const std::string & nt2) const{
            auto it1 = non_terminal_first_set.find(nt1);
            if (it1 == non_terminal_first_set.end()) return false;
            auto it2 = non_terminal_first_set.find(nt2);
            if (it2 == non_terminal_first_set.end()) return false;
            const auto & set1 = it1->second;
            const auto & set2 = it2->second;
            for(const auto & element : set1){
                if (set2.find(element) != set2.end()) return true;
            }
            return false;
        }

        bool followSetSameElementExists(const std::string & nt1,const std::string & nt2) const{
            auto it1 = non_terminal_follow_set.find(nt1);
            if (it1 == non_terminal_follow_set.end()) return false;
            auto it2 = non_terminal_follow_set.find(nt2);
            if (it2 == non_terminal_follow_set.end()) return false;
            const auto & set1 = it1->second;
            const auto & set2 = it2->second;
            for(const auto & element : set1){
                if (set2.find(element) != set2.end()) return true;
            }
            return false;
        }

        void displayNonTerminalTokenMap(std::ostream & out = std::cout) const{
            for(const auto & element : non_terminal_token_map){
                for(auto & row : element.second){
                    out<<element.first<<" -> ";
                    for(auto & column : row){
                        out<<column.getWord()<<" ";
                    }
                    out<<std::endl;
                }
            }
            return;
        }

        void displayNonTerminalFirstSet(std::ostream & out = std::cout) const{
            for(const auto & element : non_terminal_first_set){
                out<<element.first<<" : ";
                if (!element.second.empty()){
                    out<<"{";
                    for(const auto & atom : element.second){
                        out<<atom<<" ";
                    }
                    out<<"}"<<std::endl;
                }
                else out<<"{空}"<<std::endl;
            }
            return;
        }

        void displayNonTerminalFollowSet(std::ostream & out = std::cout) const{
            for(const auto & element : non_terminal_follow_set){
                out<<element.first<<" : ";
                if (!element.second.empty()){
                    out<<"{";
                    for(const auto & atom : element.second){
                        out<<atom<<" ";
                    }
                    out<<"}"<<std::endl;
                }
                else out<<"{空}"<<std::endl;
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            out<<"文法: "<<grammarStart<<std::endl;
            displayNonTerminalTokenMap(out);
            out<<"各非终结符的first集元素: "<<std::endl;
            displayNonTerminalFirstSet(out);
            out<<"各非终结符的follow集元素: "<<std::endl;
            displayNonTerminalFollowSet(out);
            out<<std::endl;
            return;
        }

        bool isNonTerminal(const std::string & tokenWord) const{
            auto it = non_terminal_token_map.find(tokenWord);
            return it != non_terminal_token_map.end();
        }

        void getFirstSetTable(std::vector <std::vector <std::string>> & table) const{
            size_t rowNum = non_terminal_first_set.size();
            size_t columnNum = 2;
            table = std::vector <std::vector <std::string>>(rowNum,std::vector <std::string>(columnNum));
            size_t index = 0;
            for(const auto & firstSetIt : non_terminal_first_set){
                const std::string & nonTerminal = firstSetIt.first;
                const auto & firstSet = firstSetIt.second;
                table[index][0] = nonTerminal;
                table[index][1] = "{";
                for(const auto & element : firstSet){
                    table[index][1] += element + " ";
                }
                table[index][1] += "}";
                index += 1;
            }
            return;
        }

        void getFollowSetTable(std::vector <std::vector <std::string>> & table) const{
            size_t rowNum = non_terminal_follow_set.size();
            size_t columnNum = 2;
            table = std::vector <std::vector <std::string>>(rowNum,std::vector <std::string>(columnNum));
            size_t index = 0;
            for(const auto & followSetIt : non_terminal_follow_set){
                const std::string & nonTerminal = followSetIt.first;
                const auto & followSet = followSetIt.second;
                table[index][0] = nonTerminal;
                table[index][1] = "{";
                for(const auto & element : followSet){
                    table[index][1] += element + " ";
                }
                table[index][1] += "}";
                index += 1;
            }
            return;
        }

    };

    class grammar_dfa_graph{

        public:

        class grammar_dfa_edge;

        class tokenString{

            public:

            std::string tokenStr;
            size_t pointPosition;
            bool action;//0: 移进 1: 规约

            tokenString():tokenStr(){
                pointPosition = std::numeric_limits <size_t>::max();
                action = 0;
            }
            ~tokenString(){}

            tokenString(const tokenString & other){
                tokenStr = other.tokenStr;
                pointPosition = other.pointPosition;
                action = other.action;
            }

            tokenString & operator =(const tokenString & other){
                if (this != &other){
                    tokenStr = other.tokenStr;
                    pointPosition = other.pointPosition;
                    action = other.action;
                }
                return *this;
            }

            bool operator ==(const tokenString & other) const{
                return (
                    this->tokenStr == other.tokenStr &&
                    this->pointPosition == other.pointPosition &&
                    this->action == other.action
                );
            }

            bool isNewState() const{
                return (!action && pointPosition == 0);
            }

            void clear(){
                pointPosition = std::numeric_limits <size_t>::max();
                tokenStr.clear();
                action = 0;
                return;
            }

            void display(std::ostream & out = std::cout) const{
                out<<tokenStr<<std::endl;
                out<<pointPosition<<" (";
                out<<(action ? "归约" : "移进")<<")"<<std::endl;
                return;
            }

            std::string getStart() const{
                size_t end = tokenStr.find("->") - 1;
                return tokenStr.substr(0,end);
            }

            std::string getPointToken() const{
                size_t start = tokenStr.find(".") + 1;
                size_t end = tokenStr.find(" ",start);
                return tokenStr.substr(start,(end - start));
            }

            bool isNext(const tokenString & other) const{
                const std::string & newTokenStr = remove_all_chars(tokenStr," .");
                const std::string & newOtherStr = remove_all_chars(other.tokenStr," .");
                if (
                    newTokenStr == newOtherStr &&
                    pointPosition == (other.pointPosition - 1)
                ) return true;
                else return false;
            }

            inline static size_t hash(const tokenString & self){
                return std::hash <std::string>{}(self.tokenStr);
            }

            inline static bool equal(const tokenString & a,const tokenString & b){
                return a == b;
            }

            inline static std::string remove_all_chars(const std::string & s,const std::string & chars){
                std::string res;
                res.reserve(s.size());
                for(char c : s){
                    bool isRemove = false;
                    for(char cc : chars){
                        if (c == cc){
                            isRemove = true;
                            break;
                        }
                    }
                    if (!isRemove) res += c;
                }
                return res;
            }

        };

        class grammar_dfa_node{

            public:

            size_t nodeId;
            std::unordered_set <
                tokenString,
                std::function <size_t (const tokenString &)>,
                std::function <bool (const tokenString &,const tokenString &)>
            > grammars;
            bool action;//0: 移进 1: 规约

            grammar_dfa_node():grammars(8,tokenString::hash,tokenString::equal),action(1){
                nodeId = std::numeric_limits <size_t>::max();
            }
            ~grammar_dfa_node(){}

            grammar_dfa_node(const grammar_dfa_node & other){
                nodeId = other.nodeId;
                grammars = other.grammars;
                action = other.action;
            }

            grammar_dfa_node & operator =(const grammar_dfa_node & other){
                if (this != &other){
                    nodeId = other.nodeId;
                    grammars = other.grammars;
                    action = other.action;
                }
                return *this;
            }

            bool operator <(const grammar_dfa_node & other) const{
                if (*this == other) return false;
                return this->nodeId < other.nodeId;
            }

            bool operator ==(const grammar_dfa_node & other) const{
                return (
                    this->grammars == other.grammars &&
                    this->action == other.action
                );
            }

            void clear(){
                nodeId = std::numeric_limits <size_t>::max();
                grammars.clear();
                action = 1;
                return;
            }

            std::string getString() const{
                std::ostringstream oss;
                for(const auto & grammar : grammars){
                    oss<<grammar.tokenStr<<std::endl;
                }
                return oss.str();
            }

            std::string getStringWithoutPoint() const{
                std::ostringstream oss;
                for(const auto & grammar : grammars){
                    std::string ts = grammar.tokenStr;
                    ts.erase(std::remove(ts.begin(),ts.end(), '.'),ts.end());
                    oss<<ts<<std::endl;
                }
                return oss.str();
            }

            bool actionConflicting() const{
                if (grammars.empty()) return false;
                bool firstAct = grammars.begin()->action;
                for(const auto & grammar : grammars){
                    if (grammar.action != firstAct) return true;//只要存在不同动作就直接判定移进-归约冲突
                }
                return false;
            }

            bool slr1_actionConflicting(const zhy::grammar_first_follow_set & gffs) const{
                if (grammars.empty()) return false;
                for(const auto & element : grammars){
                    bool firstAct = element.action;
                    std::string firstStart = element.getStart();//针对首项目归约
                    std::string firstPoint = element.getPointToken();//针对首项目移进
                    for(const auto & grammar : grammars){
                        if (grammar.action != firstAct){
                            if (firstAct){//本项目要移进
                                std::string point = grammar.getPointToken();
                                if (gffs.inFollowSet(firstStart,point)) return true;
                                else continue;
                            }
                            else{//本项目要归约
                                std::string start = grammar.getStart();
                                if (gffs.inFollowSet(start,firstPoint)) return true;
                                else continue;
                            }
                        }
                    }
                }
                return false;
            }

            bool reductionConflicting() const{
                if (grammars.empty()) return false;
                size_t reductionNum = 0;
                for(const auto & grammar : grammars){
                    if (grammar.action){
                        reductionNum += 1;
                        if (reductionNum > 1) return true;//只要归约项目数>1就直接判定归约-归约冲突
                    }
                }
                return false;
            }

            bool slr1_reductionConflicting(const zhy::grammar_first_follow_set & gffs) const{
                if (grammars.empty()) return false;
                for(const auto & element : grammars){
                    bool firstAct = element.action;
                    if (!firstAct) continue;//如果首项目是移进则直接跳过
                    std::string firstStart = element.getStart();//针对首项目归约
                    for(const auto & grammar : grammars){
                        if (grammar.action == firstAct){
                            std::string start = grammar.getStart();
                            if (start == firstStart) continue;
                            if (gffs.followSetSameElementExists(firstStart,start)) return true;
                            else continue;
                        }
                    }
                }
                return false;
            }

            bool LR0Conflicting(bool & flag) const{
                if (actionConflicting()){
                    flag = 0;
                    return true;
                }
                else if (reductionConflicting()){
                    flag = 1;
                    return true;
                }
                else return false;
            }

            bool SLR1Conflicting(const zhy::grammar_first_follow_set & gffs,bool & flag) const{
                if (slr1_actionConflicting(gffs)){
                    flag = 0;
                    return true;
                }
                else if (slr1_reductionConflicting(gffs)){
                    flag = 1;
                    return true;
                }
                else return false;
            }

            std::string getAction() const{
                return action ? "归约" : "移进";
            }

            std::string getRule() const{
                if (action) return getStringWithoutPoint();
                else return "";
            }

            void addGrammar(const tokenString & g){
                grammars.emplace(g);
                this->action = this->action && g.action;
                return;
            }

            void getClosure(const std::unordered_map <std::string,std::vector <tokenString>> & allGrammarsWithPoint){
                while (true){
                    size_t oldLength = grammars.size();
                    for(const auto & grammar : grammars){
                        if (!grammar.action){
                            std::string pointToken = grammar.getPointToken();
                            auto it = allGrammarsWithPoint.find(pointToken);
                            if (it != allGrammarsWithPoint.end()){
                                for(const auto & ts : it->second){
                                    if (ts.isNewState()) addGrammar(ts);
                                }
                            }
                        }
                    }
                    size_t newLength = grammars.size();
                    if (newLength == oldLength) break;
                }
                return;
            }

            void getGoto(
                grammar_dfa_graph & graph,
                const std::unordered_map <std::string,std::vector <tokenString>> & allGrammarsWithPoint
            ) const{
                std::unordered_map <std::string,std::vector <tokenString>> edgeGrammarMap;
                for(const auto & grammar : grammars){
                    if (!grammar.action){
                        const std::string & grammarPointToken = grammar.getPointToken();
                        if (grammarPointToken != "#") edgeGrammarMap[grammarPointToken].emplace_back(grammar);
                    }
                }
                for(const auto & element : edgeGrammarMap){
                    const std::string & grammarPointToken = element.first;
                    const auto & edgeGrammars = element.second;
                    bool iog = allGrammarsWithPoint.find(grammarPointToken) != allGrammarsWithPoint.end();
                    grammar_dfa_node tempNode;
                    tempNode.nodeId = graph.getNodeNum();
                    for(const auto & edgeGrammar : edgeGrammars){
                        const std::string & grammarStart = edgeGrammar.getStart();
                        const auto & tokenStrings = allGrammarsWithPoint.at(grammarStart);
                        for(const auto & ts : tokenStrings){
                            if (edgeGrammar.isNext(ts)){
                                tempNode.addGrammar(ts);
                                break;
                            }
                        }
                    }
                    tempNode.getClosure(allGrammarsWithPoint);
                    size_t edgeEnd = graph.addNode(tempNode);
                    graph.edges.emplace(grammar_dfa_edge(this->nodeId,grammarPointToken,edgeEnd,iog));
                }
                /*
                for(const auto & grammar : grammars){
                    if (!grammar.action){
                        const std::string & grammarStart = grammar.getStart();
                        const std::string & grammarPointToken = grammar.getPointToken();
                        bool iog = allGrammarsWithPoint.find(grammarPointToken) != allGrammarsWithPoint.end();
                        const auto & tokenStrings = allGrammarsWithPoint.at(grammarStart);
                        for(const auto & ts : tokenStrings){
                            if (grammar.isNext(ts)){
                                grammar_dfa_node tempNode;
                                tempNode.nodeId = graph.getNodeNum();
                                tempNode.addGrammar(ts);
                                tempNode.getClosure(allGrammarsWithPoint);
                                size_t edgeEnd = graph.addNode(tempNode);
                                graph.edges.emplace(grammar_dfa_edge(this->nodeId,grammarPointToken,edgeEnd,iog));
                                break;
                            }
                        }
                    }
                }
                */
                return;
            }


        };

        class grammar_dfa_edge{
    
            public:

            size_t start;
            std::string edgeName;
            size_t end;
            bool inputOrGoto;//0: input, 1: goto

            grammar_dfa_edge(size_t s,const std::string & en,size_t e,bool iog):
                start(s),
                edgeName(en),
                end(e),
                inputOrGoto(iog)
            {}
            ~grammar_dfa_edge(){}

            grammar_dfa_edge(const grammar_dfa_edge & other){
                start = other.start;
                edgeName = other.edgeName;
                end = other.end;
                inputOrGoto = other.inputOrGoto;
            }

            grammar_dfa_edge & operator =(const grammar_dfa_edge & other){
                if (this != &other){
                    start = other.start;
                    edgeName = other.edgeName;
                    end = other.end;
                    inputOrGoto = other.inputOrGoto;
                }
                return *this;
            }

            bool operator ==(const grammar_dfa_edge & other) const{
                return (
                    start == other.start &&
                    edgeName == other.edgeName &&
                    end == other.end &&
                    inputOrGoto == other.inputOrGoto
                );
            }

            inline static size_t hash(const grammar_dfa_edge & self){
                size_t h1 = std::hash <std::string>{}(self.edgeName);
                size_t h2 = std::hash <size_t>{}(self.start);
                size_t h3 = std::hash <size_t>{}(self.end);
                size_t h4 = std::hash <bool>{}(self.inputOrGoto);
                return h1 ^ (h2 << 1) ^ (h3 >> 1) ^ h4;
            }

            inline static bool equal(const grammar_dfa_edge & a,const grammar_dfa_edge & b){
                return a == b;
            }


        };

        std::unordered_map <size_t,grammar_dfa_node> id_node_map;
        std::unordered_map <std::string,size_t> node_id_map;
        std::unordered_set <
            grammar_dfa_edge,
            std::function <size_t (const grammar_dfa_edge &)>,
            std::function <bool (const grammar_dfa_edge &,const grammar_dfa_edge &)>
        > edges;

        size_t getNodeNum() const{
            return node_id_map.size();
        }

        size_t getEdgeNum() const{
            return edges.size();
        }

        size_t addNode(const grammar_dfa_node & node){
            const std::string & nodeStr = node.getString();
            auto it = node_id_map.find(nodeStr);
            if (it != node_id_map.end()) return it->second;
            if (node.nodeId != getNodeNum()) return getNodeNum() - 1;
            node_id_map.emplace(nodeStr,node.nodeId);
            id_node_map.emplace(node.nodeId,node);
            return node.nodeId;
        }

        /*
        using graphMode = int;
        inline static constexpr graphMode lr0 = 0x01;
        inline static constexpr graphMode lr1 = 0x02;
        inline static constexpr graphMode lalr1 = 0x04;
        */

        void getLR0Graph(const zhy::grammar_first_follow_set & gffs){
            if (!gffs.expanded) return;
            const auto & nttm = gffs.non_terminal_token_map;
            std::unordered_map <std::string,std::vector <tokenString>> allGrammarsWithPoint;
            for(const auto & element : nttm){
                const std::string & nonTerminal = element.first;
                const auto & grammars = element.second;
                tokenString tempTokenString;
                for(size_t i=0;i<grammars.size();i++){
                    for(size_t j=0;j<grammars[i].size();j++){
                        tempTokenString.pointPosition = gffs.getTokenStringWithPoint(tempTokenString.tokenStr,nonTerminal,i,j);
                        tempTokenString.action = 0;
                        allGrammarsWithPoint[nonTerminal].emplace_back(tempTokenString);
                        //tempTokenString.display(std::cout);
                        tempTokenString.clear();
                    }
                    tempTokenString.pointPosition = gffs.getTokenStringWithPoint(tempTokenString.tokenStr,nonTerminal,i,grammars[i].size());
                    tempTokenString.action = 1;
                    allGrammarsWithPoint[nonTerminal].emplace_back(tempTokenString);
                    //tempTokenString.display(std::cout);
                    tempTokenString.clear();
                }
            }
            /*
            for(const auto & grammarWithPoint : allGrammarsWithPoint){
                for(const auto & tempTokenString : grammarWithPoint.second){
                    tempTokenString.display(std::cout);
                }
            }
            */
            grammar_dfa_node tempNode;
            tempNode.nodeId = getNodeNum();
            const auto & startGrammarsWithPoint = allGrammarsWithPoint[gffs.grammarStart];
            for(const auto & grammar : startGrammarsWithPoint){
                if (grammar.isNewState()) tempNode.addGrammar(grammar);
            }
            tempNode.getClosure(allGrammarsWithPoint);
            addNode(tempNode);
            tempNode.clear();
            while (true){
                bool changed = false;
                size_t oldNodeNum = getNodeNum();
                size_t oldEdgeNum = getEdgeNum();
                for(const auto & node : id_node_map){
                    node.second.getGoto(*this,allGrammarsWithPoint);
                }
                size_t newNodeNum = getNodeNum();
                size_t newEdgeNum = getEdgeNum();
                //std::cout<<oldNodeNum<<" "<<oldEdgeNum<<" "<<newNodeNum<<" "<<newEdgeNum<<std::endl;
                //if (newNodeNum > 10) break;
                changed = (oldNodeNum != newNodeNum || oldEdgeNum != newEdgeNum);
                if (!changed) break;
            }
            return;
        }

        grammar_dfa_graph():
            id_node_map(),node_id_map(),
            edges(8,grammar_dfa_edge::hash,grammar_dfa_edge::equal){}
        grammar_dfa_graph(zhy::grammar_first_follow_set & gffs/*,graphMode mode = lr0*/)
            :id_node_map(),node_id_map(),
            edges(8,grammar_dfa_edge::hash,grammar_dfa_edge::equal)
        {
            gffs.expandGrammar();
            gffs.getNonTerminalFirstSet();
            gffs.getNonTerminalFollowSet();
            getLR0Graph(gffs);
        }

        ~grammar_dfa_graph(){}

        grammar_dfa_graph(const grammar_dfa_graph & other){
            id_node_map = other.id_node_map;
            node_id_map = other.node_id_map;
            edges = other.edges;
        }

        grammar_dfa_graph & operator =(const grammar_dfa_graph & other){
            if (this != &other){
                id_node_map = other.id_node_map;
                node_id_map = other.node_id_map;
                edges = other.edges;
            }
            return *this;
        }

        bool isLR0(std::ostream & description = std::cout) const{
            for(const auto & node : id_node_map){
                bool flag;
                if (node.second.LR0Conflicting(flag)){
                    if (!flag) description<<"结点 "<<node.first<<" 存在移进-归约冲突"<<std::endl;
                    else description<<"结点 "<<node.first<<" 存在归约-归约冲突"<<std::endl;
                    return false;
                }
            }
            return true;
        }

        bool isSLR1(const zhy::grammar_first_follow_set & gffs,std::ostream & description = std::cout) const{
            for(const auto & node : id_node_map){
                bool flag;
                if (node.second.SLR1Conflicting(gffs,flag)){
                    if (!flag) description<<"结点 "<<node.first<<" 存在移进-归约冲突"<<std::endl;
                    else description<<"结点 "<<node.first<<" 存在归约-归约冲突"<<std::endl;
                    return false;
                }
            }
            return true;
        }

        void getTable(std::vector <std::vector <std::string>> & table) const{
            std::unordered_set <std::string> inputEdgesSet;
            for(const auto & edge : edges){
                if (!edge.inputOrGoto) inputEdgesSet.emplace(edge.edgeName);
            }
            std::unordered_set <std::string> gotoEdgesSet;
            for(const auto & edge : edges){
                if (edge.inputOrGoto) gotoEdgesSet.emplace(edge.edgeName);
            }
            size_t rowNum = getNodeNum() + 2;
            size_t columnNum = inputEdgesSet.size() + gotoEdgesSet.size() + 4;
            table = std::vector <std::vector <std::string>>(rowNum,std::vector <std::string>(columnNum));
            table[0][0] = "state";
            table[0][1] = "nodeName";
            table[0][2] = "action";
            table[0][3] = "rule";
            size_t inputIndex = 4;
            for(const auto & input : inputEdgesSet){
                table[0][inputIndex] = "input";
                table[1][inputIndex] = input;
                inputIndex += 1;
            }
            for(const auto & _goto : gotoEdgesSet){
                table[0][inputIndex] = "goto";
                table[1][inputIndex] = _goto;
                inputIndex += 1;
            }
            for(const auto & node : id_node_map){
                size_t state = node.first;
                size_t row = state + 2;
                table[row][0] = std::to_string(state);
                table[row][1] = node.second.getString();
                table[row][2] = node.second.getAction();
                table[row][3] = node.second.getRule();
            }
            for(const auto & edge : edges){
                for(size_t i=4;i<columnNum;i++){
                    if (table[1][i] == edge.edgeName){
                        size_t state = edge.start + 2;
                        if (table[state][i].empty()) table[state][i] = std::to_string(edge.end);
                        else{
                            if (table[state][i][0] != '{') table[state][i] = "{" + table[state][i] + "}";
                            table[state][i].insert(table[state][i].size() - 1,", " + std::to_string(edge.end));
                        }
                    }
                }
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            std::vector <std::vector <std::string>> table;
            getTable(table);
            for(const auto & row : table){
                for(const auto & column : row){
                    if (column.empty()) out<<"空 ";
                    else out<<column<<" ";
                }
                out<<std::endl;
            }
            return;
        }


    };


    class grammar_lr1_dfa_graph{

        public:

        class LR1tokenString{

            public:

            std::string tokenStr;
            size_t pointPosition;
            bool action;//1: 移进, 0:归约
            std::unordered_set <std::string> lookAheads;

            LR1tokenString():tokenStr(),action(0),lookAheads(){
                pointPosition = std::numeric_limits <size_t>::max();
            }
            ~LR1tokenString(){}

            LR1tokenString(const LR1tokenString & other){
                tokenStr = other.tokenStr;
                pointPosition = other.pointPosition;
                action = other.action;
                lookAheads = other.lookAheads;
            }

            LR1tokenString & operator =(const LR1tokenString & other){
                if (this != &other){
                    tokenStr = other.tokenStr;
                    pointPosition = other.pointPosition;
                    action = other.action;
                    lookAheads = other.lookAheads;
                }
                return *this;
            }

            bool operator ==(const LR1tokenString & other) const{
                return (
                    this->tokenStr == other.tokenStr &&
                    this->pointPosition == other.pointPosition &&
                    this->action == other.action &&
                    this->lookAheads == other.lookAheads
                );
            }

            void clear(){
                tokenStr.clear();
                pointPosition = std::numeric_limits <size_t>::max();
                action = 0;
                lookAheads.clear();
                return;
            }

            void addLookAhead(const std::string & la){
                lookAheads.emplace(la);
                return;
            }

            bool isNewState() const{
                return (!action && pointPosition == 0);
            }

            bool isNext(const LR1tokenString & other) const{
                const std::string & newTokenStr = remove_all_chars(tokenStr," .");
                const std::string & newOtherStr = remove_all_chars(other.tokenStr," .");
                if (
                    newTokenStr == newOtherStr &&
                    pointPosition == (other.pointPosition - 1)
                ) return true;
                else return false;
            }

            std::string getString() const{
                std::string str = "[ " + tokenStr + ", ";
                str += *lookAheads.begin();
                for(auto it = std::next(lookAheads.begin());it != lookAheads.end();it++){
                    str += " / " + *it;
                }
                str += "]";
                return str;
            }

            std::string getStringWithoutPoint() const{
                return remove_all_chars(getString(),".");
            }

            void display(std::ostream & out = std::cout) const{
                out<<getString()<<std::endl;
                out<<pointPosition<<" ("<<(action ? "归约" : "移进")<<")"<<std::endl;
                return;
            }

            friend std::ostream & operator <<(std::ostream & out,const LR1tokenString & self){
                out<<self.getString();
                return out;
            }

            std::string getStart() const{
                size_t end = tokenStr.find("->") - 1;
                return tokenStr.substr(0,end);
            }

            std::string getPointToken() const{
                size_t start = tokenStr.find(".") + 1;
                size_t end = tokenStr.find(" ",start);
                return tokenStr.substr(start,(end - start));
            }

            std::string pointNextToken() const{
                size_t start = tokenStr.find(".") + 1;
                start = tokenStr.find(" ",start);
                if (start == std::string::npos) start = tokenStr.size();
                start += 1;
                if (start >= tokenStr.size()) return "";
                size_t end = tokenStr.find(" ",start);
                return tokenStr.substr(start,(end - start));
            }

            void getLookAheads(const zhy::grammar_first_follow_set & gffs,const std::string & pointNext){
                if (pointNext == "") return;
                if (gffs.isNonTerminal(pointNext)){
                    auto it = gffs.non_terminal_first_set.find(pointNext);
                    if (it != gffs.non_terminal_first_set.end()){
                        const auto & firstSet = it->second;
                        lookAheads.insert(firstSet.begin(),firstSet.end());
                    }
                }
                else lookAheads.emplace(pointNext);
                return;
            }

            void mergeLookAheads(const LR1tokenString & other){
                lookAheads.insert(other.lookAheads.begin(),other.lookAheads.end());
                //if (lookAheads.empty()) lookAheads.emplace("$");
                return;
            }

            bool isSameCore(const LR1tokenString & other) const{
                return this->tokenStr == other.tokenStr;
            }

            const std::string & getCore() const{
                return tokenStr;
            }

            inline static size_t hash(const LR1tokenString & self){
                return std::hash <std::string>{}(self.getString());
            }

            inline static bool equal(const LR1tokenString & a,const LR1tokenString & b){
                return a == b;
            }

            inline static std::string remove_all_chars(const std::string & s,const std::string & chars){
                std::string res;
                res.reserve(s.size());
                for(char c : s){
                    if (chars.find(c) == std::string::npos) res += c;
                }
                return res;
            }


        };

        class lr1_grammar_dfa_node{

            public:

            size_t nodeId;
            std::unordered_set <
                LR1tokenString,
                std::function <size_t (const LR1tokenString &)>,
                std::function <bool (const LR1tokenString &,const LR1tokenString &)>
            > grammars;
            bool action;

            lr1_grammar_dfa_node():grammars(8,LR1tokenString::hash,LR1tokenString::equal),action(1){
                nodeId = std::numeric_limits <size_t>::max();
            }
            ~lr1_grammar_dfa_node(){}

            lr1_grammar_dfa_node(const lr1_grammar_dfa_node & other){
                nodeId = other.nodeId;
                grammars = other.grammars;
                action = other.action;
            }

            lr1_grammar_dfa_node & operator =(const lr1_grammar_dfa_node & other){
                if (this != &other){
                    nodeId = other.nodeId;
                    grammars = other.grammars;
                    action = other.action;
                }
                return *this;
            }

            bool operator ==(const lr1_grammar_dfa_node & other) const{
                return (
                    this->grammars == other.grammars &&
                    this->action == other.action
                );
            }

            void clear(){
                nodeId = std::numeric_limits <size_t>::max();
                grammars.clear();
                action = 1;
                return;
            }

            void addGrammar(const LR1tokenString & g){
                grammars.emplace(g);
                this->action = this->action && g.action;
                return;
            }

            std::string getString() const{
                std::ostringstream oss;
                for(const auto & grammar : grammars){
                    oss<<grammar<<std::endl;
                }
                return oss.str();
            }

            std::string getStringWithoutPoint() const{
                std::ostringstream oss;
                for(const auto & grammar : grammars){
                    oss<<grammar.getStringWithoutPoint()<<std::endl;
                }
                return oss.str();
            }

            std::string getAction() const{
                return action ? "归约" : "移进";
            }

            std::string getRule() const{
                if (action) return getStringWithoutPoint();
                else return "";
            }

            std::string getCore() const{
                std::ostringstream oss;
                for(const auto & grammar : grammars){
                    oss<<grammar.getCore()<<std::endl;
                }
                return oss.str();
            }

            void display(std::ostream & out = std::cout) const{
                out<<nodeId<<std::endl;
                out<<getString()<<std::endl;
                out<<getAction()<<std::endl;
                return;
            }

            void getClosure(
                const std::unordered_map <std::string,std::vector <LR1tokenString>> & allGrammarsWithPoint,
                const zhy::grammar_first_follow_set & gffs
            ){
                while (true){
                    size_t oldLength = grammars.size();
                    for(const auto & grammar : grammars){
                        if (!grammar.action){
                            std::string pointToken = grammar.getPointToken();
                            std::string pointNextToken = grammar.pointNextToken();
                            auto it = allGrammarsWithPoint.find(pointToken);
                            if (it != allGrammarsWithPoint.end()){
                                for(const auto & ts : it->second){
                                    if (ts.isNewState()){
                                        auto newTs = ts;
                                        newTs.getLookAheads(gffs,pointNextToken);
                                        if (gffs.canEmpty(pointNextToken)) newTs.mergeLookAheads(grammar);
                                        else if (pointNextToken == "") newTs.mergeLookAheads(grammar);
                                        addGrammar(newTs);
                                    }
                                }
                            }
                        }
                    }
                    size_t newLength = grammars.size();
                    if (newLength == oldLength) break;
                }
                return;
            }

            void getGoto(
                grammar_lr1_dfa_graph & graph,
                const std::unordered_map <std::string,std::vector <LR1tokenString>> & allGrammarsWithPoint,
                const zhy::grammar_first_follow_set & gffs
            ) const{
                std::unordered_map <std::string,std::vector <LR1tokenString>> edgeGrammarMap;
                for(const auto & grammar : grammars){
                    if (!grammar.action){
                        const std::string & grammarPointToken = grammar.getPointToken();
                        if (grammarPointToken != "#") edgeGrammarMap[grammarPointToken].emplace_back(grammar);
                    }
                }
                for(const auto & element : edgeGrammarMap){
                    const std::string & grammarPointToken = element.first;
                    const auto & edgeGrammars = element.second;
                    bool iog = allGrammarsWithPoint.find(grammarPointToken) != allGrammarsWithPoint.end();
                    lr1_grammar_dfa_node tempNode;
                    tempNode.nodeId = graph.getNodeNum();
                    for(const auto & edgeGrammar : edgeGrammars){
                        const std::string & grammarStart = edgeGrammar.getStart();
                        const auto & tokenStrings = allGrammarsWithPoint.at(grammarStart);
                        for(const auto & ts : tokenStrings){
                            if (edgeGrammar.isNext(ts)){
                                auto newTs = ts;
                                newTs.mergeLookAheads(edgeGrammar);
                                tempNode.addGrammar(newTs);
                                break;
                            }
                        }
                    }
                    tempNode.getClosure(allGrammarsWithPoint,gffs);
                    size_t edgeEnd = graph.addNode(tempNode);
                    graph.edges.emplace(lr1_grammar_dfa_edge(this->nodeId,grammarPointToken,edgeEnd,iog));
                }
                /*std::unordered_map <
                    std::string,
                    std::vector <std::pair<std::string,LR1tokenString>>
                > edgeGrammarMap;
                for(const auto & grammar : grammars){
                    if (!grammar.action){
                        const std::string & grammarPointToken = grammar.getPointToken();
                        const std::string & pointNext = grammar.pointNextToken();
                        if (grammarPointToken != "#") edgeGrammarMap[grammarPointToken].emplace_back(std::make_pair(pointNext,grammar));
                    }
                }
                for(const auto & element : edgeGrammarMap){
                    const std::string & grammarPointToken = element.first;
                    const auto & edgeGrammars = element.second;
                    bool iog = allGrammarsWithPoint.find(grammarPointToken) != allGrammarsWithPoint.end();
                    lr1_grammar_dfa_node tempNode;
                    tempNode.nodeId = graph.getNodeNum();
                    for(const auto & edgeGrammarIt : edgeGrammars){
                        const auto & pointNext = edgeGrammarIt.first;
                        const auto & edgeGrammar = edgeGrammarIt.second;
                        const std::string & grammarStart = edgeGrammar.getStart();
                        const auto & tokenStrings = allGrammarsWithPoint.at(grammarStart);
                        for(const auto & ts : tokenStrings){
                            if (edgeGrammar.isNext(ts)){
                                auto newTs = ts;
                                newTs.mergeLookAheads(edgeGrammar);
                                tempNode.addGrammar(ts);
                                break;
                            }
                        }
                    }
                    tempNode.getClosure(allGrammarsWithPoint,gffs);
                    size_t edgeEnd = graph.addNode(tempNode);
                    graph.edges.emplace(lr1_grammar_dfa_edge(this->nodeId,grammarPointToken,edgeEnd,iog));
                }*/
                return;
            }

            void selfMergeLookAheads(){
                std::unordered_map <std::string,std::vector <LR1tokenString>> coreGrammarMap;
                for(const auto & grammar : grammars){
                    std::string grammarCore = grammar.getCore();
                    coreGrammarMap[grammarCore].emplace_back(grammar);
                }
                lr1_grammar_dfa_node newNode;
                for(const auto & coreGrammars : coreGrammarMap){
                    LR1tokenString tempGrammar = coreGrammars.second[0];
                    for(const auto & coreGrammar : coreGrammars.second){
                        tempGrammar.mergeLookAheads(coreGrammar);
                    }
                    newNode.addGrammar(tempGrammar);
                }
                *this = newNode;
                return;
            }

            void mergeLookAheads(const lr1_grammar_dfa_node & other){
                std::unordered_map <std::string,std::vector <LR1tokenString>> coreGrammarMap;
                /*for(const auto & grammar : grammars){
                    const std::string & grammarCore = grammar.getCore();
                    for(const auto & tempGrammar : other.grammars){
                        const std::string & tempCore = tempGrammar.getCore();
                        if (grammarCore == tempCore){
                            LR1tokenString newGrammar = grammar;
                            newGrammar.mergeLookAheads(tempGrammar);
                            coreGrammarMap[grammarCore].emplace_back(newGrammar);
                        }
                    }
                }*/
                for(const auto & grammar : grammars){
                    std::string grammarCore = grammar.getCore();
                    coreGrammarMap[grammarCore].emplace_back(grammar);
                }
                for(const auto & grammar : other.grammars){
                    std::string grammarCore = grammar.getCore();
                    coreGrammarMap[grammarCore].emplace_back(grammar);
                }
                lr1_grammar_dfa_node newNode;
                for(const auto & coreGrammars : coreGrammarMap){
                    LR1tokenString tempGrammar = coreGrammars.second[0];
                    for(const auto & coreGrammar : coreGrammars.second){
                        tempGrammar.mergeLookAheads(coreGrammar);
                    }
                    newNode.addGrammar(tempGrammar);
                }
                *this = newNode;
                return;
            }

        };

        class lr1_grammar_dfa_edge{

            public:

            size_t start;
            std::string edgeName;
            size_t end;
            bool inputOrGoto;//0: input, 1: goto

            lr1_grammar_dfa_edge(size_t s,const std::string & en,size_t e,bool iog):
                start(s),
                edgeName(en),
                end(e),
                inputOrGoto(iog)
            {}
            ~lr1_grammar_dfa_edge(){}

            lr1_grammar_dfa_edge(const lr1_grammar_dfa_edge & other){
                start = other.start;
                edgeName = other.edgeName;
                end = other.end;
                inputOrGoto = other.inputOrGoto;
            }

            lr1_grammar_dfa_edge & operator =(const lr1_grammar_dfa_edge & other){
                if (this != &other){
                    start = other.start;
                    edgeName = other.edgeName;
                    end = other.end;
                    inputOrGoto = other.inputOrGoto;
                }
                return *this;
            }

            bool operator ==(const lr1_grammar_dfa_edge & other) const{
                return (
                    start == other.start &&
                    edgeName == other.edgeName &&
                    end == other.end &&
                    inputOrGoto == other.inputOrGoto
                );
            }

            inline static size_t hash(const lr1_grammar_dfa_edge & self){
                size_t h1 = std::hash <std::string>{}(self.edgeName);
                size_t h2 = std::hash <size_t>{}(self.start);
                size_t h3 = std::hash <size_t>{}(self.end);
                size_t h4 = std::hash <bool>{}(self.inputOrGoto);
                return h1 ^ (h2 << 1) ^ (h3 >> 1) ^ h4;
            }

            inline static bool equal(const lr1_grammar_dfa_edge & a,const lr1_grammar_dfa_edge & b){
                return a == b;
            }

        };

        std::unordered_map <size_t,lr1_grammar_dfa_node> id_node_map;
        std::unordered_map <std::string,size_t> node_id_map;
        std::unordered_set <
            lr1_grammar_dfa_edge,
            std::function <size_t (const lr1_grammar_dfa_edge &)>,
            std::function <bool (const lr1_grammar_dfa_edge &,const lr1_grammar_dfa_edge &)>
        > edges;

        size_t getNodeNum() const{
            return node_id_map.size();
        }

        size_t getEdgeNum() const{
            return edges.size();
        }

        size_t addNode(const lr1_grammar_dfa_node & node){
            const std::string & nodeStr = node.getString();
            auto it = node_id_map.find(nodeStr);
            if (it != node_id_map.end()) return it->second;
            if (node.nodeId != getNodeNum()) return getNodeNum() - 1;
            node_id_map.emplace(nodeStr,node.nodeId);
            id_node_map.emplace(node.nodeId,node);
            return node.nodeId;
        }

        void getLR1Graph(const zhy::grammar_first_follow_set & gffs){
            if (!gffs.expanded) return;
            const auto & nttm = gffs.non_terminal_token_map;
            std::unordered_map <std::string,std::vector <LR1tokenString>> allGrammarsWithPoint;
            for(const auto & element : nttm){
                const std::string & nonTerminal = element.first;
                const auto & grammars = element.second;
                LR1tokenString tempTokenString;
                for(size_t i=0;i<grammars.size();i++){
                    for(size_t j=0;j<grammars[i].size();j++){
                        tempTokenString.pointPosition = gffs.getTokenStringWithPoint(tempTokenString.tokenStr,nonTerminal,i,j);
                        tempTokenString.action = 0;
                        allGrammarsWithPoint[nonTerminal].emplace_back(tempTokenString);
                        //tempTokenString.display(std::cout);
                        tempTokenString.clear();
                    }
                    tempTokenString.pointPosition = gffs.getTokenStringWithPoint(tempTokenString.tokenStr,nonTerminal,i,grammars[i].size());
                    tempTokenString.action = 1;
                    allGrammarsWithPoint[nonTerminal].emplace_back(tempTokenString);
                    //tempTokenString.display(std::cout);
                    tempTokenString.clear();
                }
            }
            /*
            for(const auto & grammarWithPoint : allGrammarsWithPoint){
                for(const auto & tempTokenString : grammarWithPoint.second){
                    tempTokenString.display(std::cout);
                }
            }
            */
            lr1_grammar_dfa_node tempNode;
            tempNode.nodeId = getNodeNum();
            const auto & startGrammarsWithPoint = allGrammarsWithPoint[gffs.grammarStart];
            for(const auto & grammar : startGrammarsWithPoint){
                if (grammar.isNewState()){
                    auto newGrammar = grammar;
                    newGrammar.addLookAhead("$");
                    tempNode.addGrammar(newGrammar);
                }
            }
            tempNode.getClosure(allGrammarsWithPoint,gffs);
            addNode(tempNode);
            tempNode.clear();
            while (true){
                bool changed = false;
                size_t oldNodeNum = getNodeNum();
                size_t oldEdgeNum = getEdgeNum();
                for(const auto & node : id_node_map){
                    node.second.getGoto(*this,allGrammarsWithPoint,gffs);
                }
                size_t newNodeNum = getNodeNum();
                size_t newEdgeNum = getEdgeNum();
                //std::cout<<oldNodeNum<<" "<<oldEdgeNum<<" "<<newNodeNum<<" "<<newEdgeNum<<std::endl;
                //if (newNodeNum > 10) break;
                changed = (oldNodeNum != newNodeNum || oldEdgeNum != newEdgeNum);
                if (!changed) break;
            }
            return;
        }

        void nodeMergeLookAheads(){
            for(auto & node : id_node_map){
                node.second.selfMergeLookAheads();
            }
            return;
        }

        grammar_lr1_dfa_graph():
            id_node_map(),node_id_map(),
            edges(8,lr1_grammar_dfa_edge::hash,lr1_grammar_dfa_edge::equal){}
        grammar_lr1_dfa_graph(zhy::grammar_first_follow_set & gffs):
            id_node_map(),node_id_map(),
            edges(8,lr1_grammar_dfa_edge::hash,lr1_grammar_dfa_edge::equal)
        {
            gffs.expandGrammar();
            gffs.getNonTerminalFirstSet();
            gffs.getNonTerminalFollowSet();
            getLR1Graph(gffs);
            nodeMergeLookAheads();
        }

        ~grammar_lr1_dfa_graph(){}

        grammar_lr1_dfa_graph(const grammar_lr1_dfa_graph & other){
            id_node_map = other.id_node_map;
            node_id_map = other.node_id_map;
            edges = other.edges;
        }

        grammar_lr1_dfa_graph & operator =(const grammar_lr1_dfa_graph & other){
            if (this != &other){
                id_node_map = other.id_node_map;
                node_id_map = other.node_id_map;
                edges = other.edges;
            }
            return *this;
        }

        void getTable(std::vector <std::vector <std::string>> & table) const{
            std::unordered_set <std::string> inputEdgesSet;
            for(const auto & edge : edges){
                if (!edge.inputOrGoto) inputEdgesSet.emplace(edge.edgeName);
            }
            std::unordered_set <std::string> gotoEdgesSet;
            for(const auto & edge : edges){
                if (edge.inputOrGoto) gotoEdgesSet.emplace(edge.edgeName);
            }
            size_t rowNum = getNodeNum() + 2;
            size_t columnNum = inputEdgesSet.size() + gotoEdgesSet.size() + 2;
            table = std::vector <std::vector <std::string>>(rowNum,std::vector <std::string>(columnNum));
            table[0][0] = "state";
            table[0][1] = "nodeName";
            size_t inputIndex = 2;
            for(const auto & input : inputEdgesSet){
                table[0][inputIndex] = "input";
                table[1][inputIndex] = input;
                inputIndex += 1;
            }
            for(const auto & _goto : gotoEdgesSet){
                table[0][inputIndex] = "goto";
                table[1][inputIndex] = _goto;
                inputIndex += 1;
            }
            for(const auto & node : id_node_map){
                size_t state = node.first;
                size_t row = state + 2;
                table[row][0] = std::to_string(state);
                table[row][1] = node.second.getString();
            }
            for(const auto & edge : edges){
                for(size_t i=2;i<columnNum;i++){
                    if (table[1][i] == edge.edgeName){
                        size_t state = edge.start + 2;
                        if (table[state][i].empty()) table[state][i] = std::to_string(edge.end);
                        else{
                            if (table[state][i][0] != '{') table[state][i] = "{" + table[state][i] + "}";
                            table[state][i].insert(table[state][i].size() - 1,", " + std::to_string(edge.end));
                        }
                    }
                }
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            std::vector <std::vector <std::string>> table;
            getTable(table);
            for(const auto & row : table){
                for(const auto & column : row){
                    if (column.empty()) out<<"空 ";
                    else out<<column<<" ";
                }
                out<<std::endl;
            }
            return;
        }

    };

    class grammar_lalr1_dfa_graph{

        private:

        using LALR1TokenString = zhy::grammar_lr1_dfa_graph::LR1tokenString;
        using lalr1_grammar_dfa_node = zhy::grammar_lr1_dfa_graph::lr1_grammar_dfa_node;
        using lalr1_grammar_dfa_edge = zhy::grammar_lr1_dfa_graph::lr1_grammar_dfa_edge;

        std::unordered_map <size_t,lalr1_grammar_dfa_node> id_node_map;
        std::unordered_map <std::string,size_t> node_id_map;
        std::unordered_set <
            lalr1_grammar_dfa_edge,
            std::function <size_t (const lalr1_grammar_dfa_edge &)>,
            std::function <bool (const lalr1_grammar_dfa_edge &,const lalr1_grammar_dfa_edge &)>
        > edges;

        size_t getNodeNum() const{
            return node_id_map.size();
        }

        size_t addNode(const lalr1_grammar_dfa_node & node){
            const std::string & nodeStr = node.getString();
            auto it = node_id_map.find(nodeStr);
            if (it != node_id_map.end()) return it->second;
            if (node.nodeId != getNodeNum()) return getNodeNum() - 1;
            node_id_map.emplace(nodeStr,node.nodeId);
            id_node_map.emplace(node.nodeId,node);
            return node.nodeId;
        }

        void mergeNode(const zhy::grammar_lr1_dfa_graph & lr1){
            std::unordered_map <std::string,std::unordered_set <size_t>> sameCoreNodeMap;
            for(const auto & nodeIt : lr1.id_node_map){
                size_t nodeId = nodeIt.first;
                const std::string & nodeCore = nodeIt.second.getCore();
                sameCoreNodeMap[nodeCore].emplace(nodeId);
            }
            std::unordered_map <size_t,size_t> nodeIdMap;
            for(const auto & sameCoreNodeIt : sameCoreNodeMap){
                const std::string & nodeCore = sameCoreNodeIt.first;
                lalr1_grammar_dfa_node tempNode;
                size_t tempNodeId = getNodeNum();
                tempNode.grammars = lr1.id_node_map.at(*sameCoreNodeIt.second.begin()).grammars;
                for(size_t nodeId : sameCoreNodeIt.second){
                    const auto & oldNode = lr1.id_node_map.at(nodeId);
                    tempNode.mergeLookAheads(oldNode);
                    nodeIdMap.emplace(nodeId,tempNodeId);
                }
                tempNode.nodeId = tempNodeId;
                addNode(tempNode);
            }
            for(const auto & edge : lr1.edges){
                size_t newStart = nodeIdMap.at(edge.start);
                size_t newEnd = nodeIdMap.at(edge.end);
                edges.emplace(lalr1_grammar_dfa_edge(newStart,edge.edgeName,newEnd,edge.inputOrGoto));
            }
            /*for(const auto & nodeIt : id_node_map){
                nodeIt.second.display(std::cout);
            }
            for(const auto & edge : edges){
                std::cout<<edge.start<<" "<<edge.edgeName<<" "<<edge.end<<" "<<edge.inputOrGoto<<std::endl;
            }*/
            return;
        }

        public:

        grammar_lalr1_dfa_graph():
            id_node_map(),node_id_map(),
            edges(8,lalr1_grammar_dfa_edge::hash,lalr1_grammar_dfa_edge::equal){}
        grammar_lalr1_dfa_graph(const zhy::grammar_lr1_dfa_graph & lr1):
            id_node_map(),node_id_map(),
            edges(8,lalr1_grammar_dfa_edge::hash,lalr1_grammar_dfa_edge::equal)
        {
            mergeNode(lr1);
        }
        ~grammar_lalr1_dfa_graph(){}

        grammar_lalr1_dfa_graph(const grammar_lalr1_dfa_graph & other){
            id_node_map = other.id_node_map;
            node_id_map = other.node_id_map;
            edges = other.edges;
        }

        grammar_lalr1_dfa_graph & operator =(const grammar_lalr1_dfa_graph & other){
            if (this != &other){
                id_node_map = other.id_node_map;
                node_id_map = other.node_id_map;
                edges = other.edges;
            }
            return *this;
        }

        void getTable(std::vector <std::vector <std::string>> & table) const{
            std::unordered_set <std::string> inputEdgesSet;
            for(const auto & edge : edges){
                if (!edge.inputOrGoto) inputEdgesSet.emplace(edge.edgeName);
            }
            std::unordered_set <std::string> gotoEdgesSet;
            for(const auto & edge : edges){
                if (edge.inputOrGoto) gotoEdgesSet.emplace(edge.edgeName);
            }
            size_t rowNum = getNodeNum() + 2;
            size_t columnNum = inputEdgesSet.size() + gotoEdgesSet.size() + 2;
            table = std::vector <std::vector <std::string>>(rowNum,std::vector <std::string>(columnNum));
            table[0][0] = "state";
            table[0][1] = "nodeName";
            size_t inputIndex = 2;
            for(const auto & input : inputEdgesSet){
                table[0][inputIndex] = "input";
                table[1][inputIndex] = input;
                inputIndex += 1;
            }
            for(const auto & _goto : gotoEdgesSet){
                table[0][inputIndex] = "goto";
                table[1][inputIndex] = _goto;
                inputIndex += 1;
            }
            for(const auto & node : id_node_map){
                size_t state = node.first;
                size_t row = state + 2;
                table[row][0] = std::to_string(state);
                table[row][1] = node.second.getString();
            }
            for(const auto & edge : edges){
                for(size_t i=2;i<columnNum;i++){
                    if (table[1][i] == edge.edgeName){
                        size_t state = edge.start + 2;
                        if (table[state][i].empty()) table[state][i] = std::to_string(edge.end);
                        else{
                            if (table[state][i][0] != '{') table[state][i] = "{" + table[state][i] + "}";
                            table[state][i].insert(table[state][i].size() - 1,", " + std::to_string(edge.end));
                        }
                    }
                }
            }
            return;
        }

        void display(std::ostream & out = std::cout) const{
            std::vector <std::vector <std::string>> table;
            getTable(table);
            for(const auto & row : table){
                for(const auto & column : row){
                    if (column.empty()) out<<"空 ";
                    else out<<column<<" ";
                }
                out<<std::endl;
            }
            return;
        }



    };

    class grammar_all_work{

        private:

        zhy::grammar_token_getter tokenGetter;
        zhy::grammar_first_follow_set firstFollowSet;
        zhy::grammar_dfa_graph lr0_dfa;
        zhy::grammar_lr1_dfa_graph lr1_dfa;
        zhy::grammar_lalr1_dfa_graph lalr1_dfa;
        std::ostream & err;

        public:

        grammar_all_work(const std::string & fp,std::ostream & e = std::cerr):tokenGetter(fp,e),err(e){
            tokenGetter.getAllTokens(err);
            firstFollowSet = zhy::grammar_first_follow_set(tokenGetter);
            lr0_dfa = zhy::grammar_dfa_graph(firstFollowSet);
            lr1_dfa = zhy::grammar_lr1_dfa_graph(firstFollowSet);
            lalr1_dfa = zhy::grammar_lalr1_dfa_graph(lr1_dfa);
        }
        ~grammar_all_work(){}

        grammar_all_work(const grammar_all_work & other):tokenGetter(other.tokenGetter.filePath,other.err),err(other.err){
            //tokenGetter = other.tokenGetter;
            firstFollowSet = other.firstFollowSet;
            lr0_dfa = other.lr0_dfa;
            lr1_dfa = other.lr1_dfa;
            lalr1_dfa = other.lalr1_dfa;
        }
        grammar_all_work & operator =(const grammar_all_work & other) = delete;

        void getAllTokens(std::ostream & out = std::cout,std::ostream & err = std::cerr) const{
            try{
                tokenGetter.displayAllTokens(out);
            }
            catch (const std::exception & e){
                err<<"在获取全部token时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void getFirstSetTable(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr) const{
            try{
                firstFollowSet.getFirstSetTable(table);
            }
            catch (const std::exception & e){
                err<<"在获取非终结符first集表时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void getFollowSetTable(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr) const{
            try{
                firstFollowSet.getFollowSetTable(table);
            }
            catch (const std::exception & e){
                err<<"在获取非终结符follow集表时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void getLR0Table(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr) const{
            try{
                lr0_dfa.getTable(table);
            }
            catch (const std::exception & e){
                err<<"在获取LR(0)dfa图表时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        bool isLR0(std::ostream & description = std::cout,std::ostream & err = std::cerr) const{
            try{
                bool is = lr0_dfa.isLR0(description);
                return is;
            }
            catch (const std::exception & e){
                err<<"在获取LR0文法判断结果时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
                return false;
            }
        }

        bool isSLR1(std::ostream & description = std::cout,std::ostream & err = std::cerr) const{
            try{
                bool is = lr0_dfa.isSLR1(firstFollowSet,description);
                return is;
            }
            catch (const std::exception & e){
                err<<"在获取SLR1文法判断结果时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
                return false;
            }
        }

        void getLR1Table(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr) const{
            try{
                lr1_dfa.getTable(table);
            }
            catch (const std::exception & e){
                err<<"在获取LR(1)dfa图表时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void getLALR0Table(std::vector <std::vector <std::string>> & table,std::ostream & err = std::cerr) const{
            try{
                lalr1_dfa.getTable(table);
            }
            catch (const std::exception & e){
                err<<"在获取LALR(1)dfa图表时遇到问题:"<<std::endl;
                err<<e.what()<<std::endl;
            }
            return;
        }

        void display(std::ostream & out = std::cout,std::ostream & err = std::cerr) const{
            tokenGetter.displayAllTokens(out);
            firstFollowSet.display(out);
            lr0_dfa.display(out);
            out<<"是否为SLR(1)文法: "<<lr0_dfa.isSLR1(firstFollowSet,out)<<std::endl;
            lr1_dfa.display(out);
            lalr1_dfa.display(out);
            return;
        }


        
    };

    class lalr_tester{

        public:

        inline static void lexer_tester(const std::string & filePath){
            grammar_token_getter test(filePath,std::cerr);
            test.getAllTokens(std::cerr);
            test.displayAllTokens(std::cout);
            return;
        }

        inline static void lexer_test(){
            std::vector <std::string> filePaths = {
                ".\\std_test1.txt",
                ".\\std_test2.txt",
                ".\\std_test3.txt",
                ".\\std_test4.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::lexer_tester(filePath);
            }
            return;
        }

        inline static void first_follow_set_tester(const std::string & filePath){
            grammar_token_getter test_lexer(filePath,std::cerr);
            test_lexer.getAllTokens(std::cerr);
            grammar_first_follow_set test_first_follow_set(test_lexer);
            test_first_follow_set.display(std::cout);
            return;
        }

        inline static void first_follow_set_test(){
            std::vector <std::string> filePaths = {
                ".\\std_test1.txt",
                ".\\std_test2.txt",
                ".\\std_test3.txt",
                ".\\std_test4.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::first_follow_set_tester(filePath);
            }
            return;
        }

        inline static void LR0_tester(const std::string & filePath){
            grammar_token_getter test_lexer(filePath,std::cerr);
            test_lexer.getAllTokens(std::cerr);
            grammar_first_follow_set test_first_follow_set(test_lexer);
            grammar_dfa_graph test_LR0_graph(test_first_follow_set);
            //test_first_follow_set.display(std::cout);
            test_LR0_graph.display(std::cout);
            std::cout<<"是否为LR(0)文法: "<<test_LR0_graph.isLR0(std::cout)<<std::endl;
            return;
        }

        inline static void LR0_test(){
            std::vector <std::string> filePaths = {
                ".\\std_test1.txt",
                //".\\std_test2.txt",
                //".\\std_test3.txt",
                ".\\std_test4.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::LR0_tester(filePath);
            }
            return;
        }

        inline static void SLR1_tester(const std::string & filePath){
            grammar_token_getter test_lexer(filePath,std::cerr);
            test_lexer.getAllTokens(std::cerr);
            grammar_first_follow_set test_first_follow_set(test_lexer);
            grammar_dfa_graph test_LR0_graph(test_first_follow_set);
            test_first_follow_set.display(std::cout);
            test_LR0_graph.display(std::cout);
            std::cout<<"是否为LR(0)文法: "<<test_LR0_graph.isLR0(std::cout)<<std::endl;
            std::cout<<"是否为SLR(1)文法: "<<test_LR0_graph.isSLR1(test_first_follow_set,std::cout)<<std::endl;
            return;
        }

        inline static void SLR1_test(){
            std::vector <std::string> filePaths = {
                ".\\slr_test1.txt",
                ".\\slr_test2.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::SLR1_tester(filePath);
            }
            return;
        }

        inline static void LR1_tester(const std::string & filePath){
            grammar_token_getter test_lexer(filePath,std::cerr);
            test_lexer.getAllTokens(std::cerr);
            grammar_first_follow_set test_first_follow_set(test_lexer);
            grammar_lr1_dfa_graph test_lr1(test_first_follow_set);
            test_lr1.display(std::cout);
            return;
        }

        inline static void LR1_test(){
            std::vector <std::string> filePaths = {
                ".\\lr1_test1.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::LR1_tester(filePath);
            }
            return;
        }

        inline static void LALR1_tester(const std::string & filePath){
            grammar_token_getter test_lexer(filePath,std::cerr);
            test_lexer.getAllTokens(std::cerr);
            grammar_first_follow_set test_first_follow_set(test_lexer);
            grammar_lr1_dfa_graph test_lr1(test_first_follow_set);
            grammar_lalr1_dfa_graph test_lalr1(test_lr1);
            test_lalr1.display(std::cout);
            return;
        }

        inline static void LALR1_test(){
            std::vector <std::string> filePaths = {
                ".\\lr1_test1.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::LALR1_tester(filePath);
            }
            return;
        }

        inline static void all_tester(const std::string & filePath){
            zhy::grammar_all_work test(filePath,std::cerr);
            test.display(std::cout);
            return;
        }

        inline static void all_test(){
            std::vector <std::string> filePaths = {
                ".\\std_test1.txt",
                ".\\std_test2.txt",
                ".\\std_test3.txt",
                ".\\slr_test1.txt",
                ".\\slr_test2.txt",
                ".\\lr1_test1.txt"
            };
            for(const auto & filePath : filePaths){
                zhy::lalr_tester::all_tester(filePath);
                std::cout<<std::endl<<std::endl;
            }
            return;
        }



    };

}

#endif