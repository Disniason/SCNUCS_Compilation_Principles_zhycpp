#ifndef Z_SET_H
#define Z_SET_H
#pragma once

#include <vector>
#include <set>
#include <unordered_map>
#include <cstddef>

namespace zhy{
    template <class T>
    class disjoint_set{
        protected:
        std::unordered_map <T,size_t> positions;
        std::vector <long long> parents;
        size_t setNum;

        public:
        disjoint_set(){
            positions.clear();
            parents.clear();
        }

        ~disjoint_set(){}

        void push(const T & newElement){
            if (positions.find(newElement) != positions.end()) return;
            size_t position = parents.size();
            parents.push_back(-1);
            positions.emplace(newElement,position);
            setNum += 1;
            return;
        }

        long long findRoot(size_t position){
            long long root = position;
            while (parents[root] >= 0){
                root = parents[root];
            }
            while (position != root) {
                long long temp = parents[position];
                parents[position] = root;
                position = temp;
            }
            return root;
        }

        void mergePos(size_t posA,size_t posB){
            long long rootA = findRoot(posA);
            long long rootB = findRoot(posB);
            if (rootA != rootB){
                long long temp = parents[rootA] + parents[rootB];
                if (parents[rootA] < parents[rootB]){
                    parents[rootA] = temp;
                    parents[rootB] = rootA;
                }
                else{
                    parents[rootB] = temp;
                    parents[rootA] = rootB;
                }
                setNum -= 1;
            }
            return;
        }

        void merge(const T & a,const T & b){
            auto itA = positions.find(a);
            auto itB = positions.find(b);
            if (itA == positions.end() || itB == positions.end()) return;
            mergePos(itA->second,itB->second);
            return;
        }

        bool isConnected(const T & a,const T & b){
            auto itA = positions.find(a);
            auto itB = positions.find(b);
            if (itA == positions.end() || itB == positions.end()) return false;
            long long rootA = findRoot(itA->second);
            long long rootB = findRoot(itB->second);
            return rootA == rootB;
        }

        size_t getSetNum(){
            return setNum;
        }

        void splitToSets(std::vector <std::set <T>> & sets){
            std::unordered_map <size_t,std::set<T>> rootMap;
            for (const auto & pair : positions){
                const T & element = pair.first;
                size_t pos = pair.second;
                size_t root = findRoot(pos);
                rootMap[root].insert(element);
            }
            sets.reserve(rootMap.size());
            for (const auto & pair : rootMap) {
                sets.emplace_back(pair.second);
            }
            return;
        }


    };

}

#endif