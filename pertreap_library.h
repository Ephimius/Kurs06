#ifndef PERTREAP_LIBRARY_H
#define PERTREAP_LIBRARY_H

#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <utility>
#include <vector>

template <class T, class F>
class PerTreap {
private:
    class Node {
    private:
        int priority;
        size_t siz;
        std::shared_ptr<T> val;
        std::shared_ptr<T> val_mod;
        std::shared_ptr<Node> ll;
        std::shared_ptr<Node> rr;
        bool rev;
        F func;
        bool modif;

        void ModifyUpdater(std::shared_ptr<T> modificator);

        void Update();

        void UpdateSiz();
        
        static std::shared_ptr<Node> Construct(std::vector<std::shared_ptr<Node>>& nodes, int from, int top_priority);

    public:
        Node(T value, int prior, F funct);
        
        Node(std::shared_ptr<T> value, int prior, F funct);
        
        Node(const Node& node); 
        
        Node(Node&& node); 
        
        Node& operator=(const Node& node); 
        
        Node& operator=(Node&& node); 
        
        Node(const Node& node, std::map<std::shared_ptr<Node>, std::shared_ptr<Node>>& nodes);
        
        void Clone(const Node& node);

        static std::shared_ptr<Node> Reverse(std::shared_ptr<Node> t_, size_t l, size_t r);
    
        static std::shared_ptr<Node> Modify(
                std::shared_ptr<Node> t_, size_t l, size_t r, 
                std::shared_ptr<T> modif);
            
        static std::shared_ptr<Node> MoveToEnd(std::shared_ptr<Node> t_, size_t ls);
        
        std::shared_ptr<T> Get(size_t ind);

        static void Merge(std::shared_ptr<Node>& rez, std::shared_ptr<Node> ll, std::shared_ptr<Node> rr);

        static void Split(std::shared_ptr<Node> what, std::shared_ptr<Node>& ll, std::shared_ptr<Node>& rr, int ls);
        
        static std::shared_ptr<Node> Construct(std::vector<std::shared_ptr<Node>>& nodes); 
        
        void ToVector(std::vector<T>& v, size_t l, size_t r); 
        
        std::shared_ptr<Node> GetLeft(); 
        
        std::shared_ptr<Node> GetRight(); 
        
        T& GetVal(); 
        
        /*int Visit() { //DEBUG
            Update();
            int ans = 0;
            std::cerr << "(";
            if (ll.get() != nullptr) {
                ans = ll->Visit();
            }
            std::cerr << "," << *val << ":" << siz;
            if (rr.get() != nullptr) {
                ans = std::max(ans, rr->Visit());
            }
            std::cerr << ")";
            return ans + 1;
        }*/

        ~Node() {
        }
    };

    std::vector<std::shared_ptr<Node>> roots;
    F func;
    size_t size;

public:
    PerTreap(const std::vector<T>& data, F funct, bool random = true, int seed = 0); 

    PerTreap(const PerTreap& treap); 

    PerTreap(PerTreap&& treap);

    PerTreap& operator=(const PerTreap& treap); 

    PerTreap& operator=(PerTreap&& treap);
    
    size_t GetNumberOfStates();

    void Reverse(size_t l, size_t r, size_t state);

    void Reverse(size_t l, size_t r);

    void Modify(T modif, size_t l, size_t r, size_t state);

    void Modify(T modif, size_t l, size_t r);
    
    void MoveToEnd(size_t ls, size_t state);
    
    void MoveToEnd(size_t ls); 

    T GetAt(size_t ind, size_t state);

    T GetAt(size_t ind);
    
    std::vector<T> ToVector(size_t l, size_t r, size_t state);
    
    std::vector<T> ToVector(size_t l, size_t r); 
    
    static void Merge(PerTreap* rez, PerTreap* ll, size_t state_l, 
                      PerTreap* rr, size_t state_r);

    static void Merge(PerTreap* rez, PerTreap* ll, PerTreap* rr); 
    
    static void Split(PerTreap* what, size_t state_w, PerTreap* ll, 
                      PerTreap* rr, int ls);

    static void Split(PerTreap* what, PerTreap* ll, PerTreap* rr, int ls); 
    
    /*int Visit(int i) { //DEBUG
        return roots[i]->Visit();
    }*/

    ~PerTreap() {
    }
};

#endif
