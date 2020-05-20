#ifndef TREAP_LIBRARY_H
#define TREAP_LIBRARY_H

#include <chrono>	
#include <memory>
#include <random>
#include <utility>
#include <vector>

template <class T, class F>
class Treap {
private:
    class Node {
    private:
        int priority;
        size_t siz;
        T val;
        std::shared_ptr<Node> ll;
        std::shared_ptr<Node> rr;
        bool rev;
        T buffv;
        F func;
        bool modif;

        void ModifyUpdater(T modificator);

        void Update();

        void UpdateSiz();
        
        static std::shared_ptr<Node> Construct(std::vector<std::shared_ptr<Node>>& nodes,
                                               int from, int top_priority);

    public:
        Node(T value, int prior, F funct);
        
        Node(const Node& node);
        
        Node(Node&& node);
        
        Node& operator=(const Node& node); 
        
        Node& operator=(Node&& node);

        static void Reverse(std::shared_ptr<Node>& t_, size_t l, size_t r);

        static void Modify(std::shared_ptr<Node>& t_, size_t l, size_t r, T modif);

        static void MoveToEnd(std::shared_ptr<Node>& t_, size_t ls); 

        T* Get(size_t ind);

        static void Merge(std::shared_ptr<Node>& rez, std::shared_ptr<Node> ll, std::shared_ptr<Node> rr);

        static void Split(std::shared_ptr<Node> what, std::shared_ptr<Node>& ll, std::shared_ptr<Node>& rr, int ls);
        
        static std::shared_ptr<Node> Construct(std::vector<std::shared_ptr<Node>>& nodes);
        
        void ToVector(std::vector<T>& v, size_t l, size_t r); 
        
        std::shared_ptr<Node> GetLeft(); 
        
        std::shared_ptr<Node> GetRight(); 
        
        T& GetVal(); 

        ~Node() {
        }
    };

    std::shared_ptr<Node> root;
    F func;
    size_t size;

public:
    Treap(const std::vector<T>& data, F funct, bool random = true, int seed = 0);

    Treap(const Treap& treap); 

    Treap(Treap&& treap);

    Treap& operator=(const Treap& treap); 

    Treap& operator=(Treap&& treap);

    void Reverse(size_t l, size_t r);

    void Modify(T modif, size_t l, size_t r);
    
    void MoveToEnd(size_t ls);

    T GetAt(size_t ind);
    
    std::vector<T> ToVector(size_t l, size_t r); 

    static void Merge(Treap* rez, Treap* ll, Treap* rr);

    static void Split(Treap* what, Treap* ll, Treap* rr, int ls); 

    ~Treap() {
    }
};

#endif
