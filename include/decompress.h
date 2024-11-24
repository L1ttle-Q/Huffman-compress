#ifndef __DECOMPRESS_H__
#define __DECOMPRESS_H__

#include <cstdio>
#include <string>
#include <chrono>
#include "Util.h"

struct Huffman_node
{
    u_char c;
    bool term;
    Huffman_node *lch, *rch;
    Huffman_node() {lch = rch = nullptr; c = '\0'; term = false;}
    Huffman_node(Huffman_node *l, Huffman_node *r) {lch = l; rch = r; c = '\0'; term = false;}
    Huffman_node(Huffman_node *l, Huffman_node *r, u_char p) {lch = l; rch = r; c = p; term = true;}
};

class Huffman_Automata
{
private:
    Huffman_node *root;
    Huffman_node *now;

    bool Add_elem(Huffman_node* rt, const std::string& s, int idx, const u_char& c)
    {
        if (idx == s.size())
        {
            if (!rt->term)
            {
                rt->c = c;
                rt->term = true;
                return true;
            }
            else return false;
        }
        if (s[idx] == '0')
        {
            if (!rt->lch) rt->lch = new Huffman_node();
            return Add_elem(rt->lch, s, idx + 1, c);
        }

        if (!rt->rch) rt->rch = new Huffman_node();
        return Add_elem(rt->rch, s, idx + 1, c);
    }

    void Print_Code(Huffman_node *rt, std::string s)
    {
        if (rt->term)
        {
            std::cout << (int)rt->c << ": " << s << std::endl;
            return ;
        }
        if (rt->lch) Print_Code(rt->lch, s + '0');
        if (rt->rch) Print_Code(rt->rch, s + '1');
    }

    void Delele_Tree(Huffman_node *rt)
    {
        if (!rt) return ;
        if (rt->lch) Delele_Tree(rt->lch);
        if (rt->rch) Delele_Tree(rt->rch);
        delete rt; rt = nullptr;
    }

public:
    Huffman_Automata() {now = root = new Huffman_node();}
    ~Huffman_Automata() {Delele_Tree(root);}

    void init() {now = root;}

    void Print()
    {
        std::cout << "Dict:" << std::endl;
        Print_Code(root, std::string(""));
    }

    void Add_Code(const std::string& s, const u_char& c)
    {
        if (!Add_elem(root, s, 0, c))
            throw std::runtime_error("Dictionary conflict!");
    }

    bool get_Now(u_char& c)
    {
        c = now->c;
        if (now->term) {now = root; return true;}
        return false;
    }

    void go_Left()
    {
        if (!now->lch) throw std::runtime_error("Decompress error!");
        now = now->lch;
    }
    void go_Right()
    {
        if (!now->rch) throw std::runtime_error("Decompress error!");
        now = now->rch;
    }
};

class Huffman_Decompress
{
private:
    FILE *fp_in, *fp_out;
    // std::unordered_map<std::string, char> dict;
    Huffman_Automata *dict;
    uint64_t textLength;

    void ReadDict()
    {
        size_t size_M;
        fread(&size_M, sizeof(uint32_t), 1, fp_in);
        for (size_t i = 0; i < size_M; ++i)
        {
            u_char c;
            size_t length;
            fread(&c, sizeof(u_char), 1, fp_in);
            fread(&length, sizeof(size_t), 1, fp_in);
            std::string code;
            for (int l = 0; l < length; l++) code += '0' + Read_binary(fp_in);
            Read_binary(fp_in, true);
            dict->Add_Code(code, c);
        }
        fread(&textLength, sizeof(uint64_t), 1, fp_in);
    }

    void Decode()
    {
        char c = '\0'; u_char decode_c = '\0';
        uint64_t decode_length = 0;
        dict->init();
        while (~(c = Read_binary(fp_in)))
        {
            if (decode_length == textLength) break;
            if (!c) dict->go_Left();
            else dict->go_Right();
            if (dict->get_Now(decode_c))
            {
                fwrite(&decode_c, sizeof(u_char), 1, fp_out);
                decode_length++;
            }
        }
    }

public:
    Huffman_Decompress(FILE *in, FILE *out) : fp_in(in), fp_out(out), dict(new Huffman_Automata()) {}
    ~Huffman_Decompress() {fclose(fp_in); fclose(fp_out); delete dict; dict = nullptr;}

    void Print_Dict()
    {
        dict->Print();
    }

    void Solve()
    {
        auto event_begin = std::chrono::high_resolution_clock::now();

        ReadDict();
        Decode();

        auto event_end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (event_end - event_begin);
        std::cout << "\nSucceed to decompress and output!\n"
                  << "Time taken: " << elapsed.count() << " ms\n" << std::endl;
    }
};

#endif