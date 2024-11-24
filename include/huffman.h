#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

/*
size of dict: 4 bytes
{c: 1 bytes | length: 4 bytes | code: [length / 4 + 1] bytes } * size
text content...
*/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <chrono>
#include "Util.h"

struct Code
{
    std::string dat;
    // u_char dat[32];
    // consider 1 byte(8 bits) can contain
    // 256 kinds of characters.
    // The Huffman tree is up to 256 depths.
};

struct Node
{
    u_char c;
    int num;
    Node *lch, *rch;
    Node() {lch = rch = nullptr; c = num = 0;}
    Node(u_char p, int q) {lch = rch = nullptr; c = p; num = q;}
    Node(Node *l, Node *r) {lch = l; rch = r;}
    Node(Node *l, Node *r, u_char p, int q) {lch = l; rch = r; c = p; num = q;}
};

class Huffman
{
private:
    FILE *fp_in, *fp_out;
    static const int CHARMAX = 256;

    // std::unordered_map<u_char, int> M_list;
    int M_list[CHARMAX];
    uint64_t textLength;
    std::unordered_map<u_char, Code> M_to;

    Node *root;

    void PrinttoFile()
    {
        size_t size_M = M_to.size();
        fwrite(&size_M, sizeof(size_t), 1, fp_out);
        for (auto i: M_to)
        {
            size_t size_ele = i.second.dat.size();
            fwrite(&(i.first), sizeof(u_char), 1, fp_out);
            fwrite(&(size_ele), sizeof(size_t), 1, fp_out);
            Write_binary(i.second.dat, fp_out);
        }
        fwrite(&textLength, sizeof(uint64_t), 1, fp_out);
        rewind(fp_in);
        u_char c;
        while (fread(&c, sizeof(u_char), 1, fp_in))
            Write_binary_stream(M_to[c].dat, fp_out);
        Write_binary_stream(std::string(""), fp_out);
    }

    void Build_Tree()
    {
        auto cmp = [](Node* left, Node* right) -> bool { return left->num > right->num; };
        std::priority_queue<Node*, std::vector<Node*>, decltype(cmp) > Q(cmp);

        for (int i = 0; i < CHARMAX; i++)
            if (M_list[i])
                Q.push(new Node(static_cast<u_char>(i), M_list[i]));

        if (Q.empty()) return ;
        if (Q.size() == 1) {M_to[Q.top()->c].dat = std::string("0"); return ;}

        Node *q1, *q2;
        while (Q.size() > 1)
        {
            q1 = Q.top(); Q.pop();
            q2 = Q.top(); Q.pop();
            // std::cout << "merge: {\n" << q1->c << ":" << q1->num << std::endl;
            // std::cout << q2->c << ":" << q2->num << "\n}" << std::endl;
            Q.push(new Node(q1, q2, '\0', q1->num + q2->num));
        }
        root = Q.top();
        return ;
    }

    void Get_Code(Node* rt, std::string s)
    {
        if (!rt->lch && !rt->rch) {M_to[rt->c].dat = s; return ;}
        if (rt->lch) Get_Code(rt->lch, s + '0');
        if (rt->rch) Get_Code(rt->rch, s + '1');
    }

    void Delete_Tree(Node *rt)
    {
        if (!rt) return ;
        if (rt->lch) Delete_Tree(rt->lch);
        if (rt->rch) Delete_Tree(rt->rch);
        delete rt;
    }

public:
    Huffman() {fp_in = stdin; fp_out = stdout;}
    Huffman(FILE *in): Huffman() { if (in) fp_in = in; }
    Huffman(FILE *in, FILE *out): Huffman()
    {
        if (in) fp_in = in;
        if (out) fp_out = out;
    }
    ~Huffman() {fclose(fp_in); fclose(fp_out); Delete_Tree(root);}

    void Print_Dict()
    {
        std::cout << "Dict:" << std::endl;
        for (auto i: M_to)
            std::cout << (int)i.first << ": " << i.second.dat << std::endl;
    }

    void Solve()
    {
        auto event_begin = std::chrono::high_resolution_clock::now();

        u_char c;
        while (fread(&c, sizeof(u_char), 1, fp_in))
            M_list[static_cast<int>(c)]++,
            textLength++;

        Build_Tree();
        if (root) Get_Code(root, "");
        PrinttoFile();

        auto event_end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (event_end - event_begin);
        std::cout << "\nSucceed to compress and output!\n"
                  << "Time taken: " << elapsed.count() << " ms\n" << std::endl;
    }
};

#endif