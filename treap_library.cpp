#include "treap_library.h"

#include <chrono>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

template <class T, class F>
void Treap<T, F>::Node::ModifyUpdater(T modificator) {
    if (modif) {
        buffv = func(buffv, modificator);
    } else {
        modif = true;
        buffv = modificator;
    }
}

template <class T, class F>
void Treap<T, F>::Node::Update() {
    if (rev) {
        rev = false;
        std::swap(ll, rr);
        if (ll.get() != nullptr) {
            ll->rev = (1 ^ ll->rev);
        }
        if (rr.get() != nullptr) {
            rr->rev = (1 ^ rr->rev);
        }
    }
    if (modif) {
        modif = false;
        if (ll.get() != nullptr) {
            ll->ModifyUpdater(buffv);
        }
        if (rr.get() != nullptr) {
            rr->ModifyUpdater(buffv);
        }
        val = func(val, buffv);
    }
}

template <class T, class F>
void Treap<T, F>::Node::UpdateSiz() {
    siz = 1;
    if (ll.get() != nullptr) {
        siz += ll->siz;
    }
    if (rr.get() != nullptr) {
        siz += rr->siz;
    }
}

template <class T, class F>
Treap<T, F>::Node::Node(T value, int prior, F funct) :
        priority(prior), siz(1), val(value), ll(),
        rr(), rev(false), func(funct), modif(false) {
}

template <class T, class F>
Treap<T, F>::Node::Node(const Node& node) :
        priority(node.priority), siz(node.siz), val(node.val), ll(),
        rr(), rev(node.rev), func(node.func), modif(node.modif) {
    if (node.ll.get() != nullptr) {
        ll = std::make_shared<Node>(*node.ll);
    }
    if (node.rr.get() != nullptr) {
        rr = std::make_shared<Node>(*node.rr);
    }
}

template <class T, class F>
Treap<T, F>::Node::Node(Node&& node) :
        priority(node.priority), siz(node.siz), ll(node.ll), rr(node.rr) {
    node.siz = 1;
    val = std::move(node.val);
    rev = std::move(node.rev);
    func = std::move(node.func);
    modif = std::move(node.modif);
    node.ll.reset();
    node.rr.reset();
}

template <class T, class F>
typename Treap<T, F>::Node& Treap<T, F>::Node::operator=(const Node& node) {
    priority = node.priority;
    siz = node.siz;
    val = node.val;
    ll.reset();
    rr.reset();
    rev = node.rev;
    func = node.func;
    modif = node.modif;
    if (node.ll.get() != nullptr) {
        ll = std::make_shared<Node>(*node.ll);
    }
    if (node.rr.get() != nullptr) {
        rr = std::make_shared<Node>(*node.rr);
    }
    return *this;
}

template <class T, class F>
typename Treap<T, F>::Node& Treap<T, F>::Node::operator=(Node&& node) {
    priority = node.priority;
    siz = node.siz;
    ll = node.ll;
    rr = node.rr;
    node.siz = 1;
    val = std::move(node.val);
    rev = std::move(node.rev);
    func = std::move(node.func);
    modif = std::move(node.modif);
    node.ll.reset();
    node.rr.reset();
    return *this;
}

template <class T, class F>
void Treap<T, F>::Node::Reverse(std::shared_ptr<Treap<T, F>::Node>& t_, size_t l, size_t r) {
    t_->Update();
    if (r <= l) {
        return;
    }
    std::shared_ptr<Node> tl;
    std::shared_ptr<Node> tm;
    std::shared_ptr<Node> tr;
    Split(t_, t_, tr, r);
    Split(t_, tl, tm, l);
    if (tm.get() != nullptr) {
        tm->rev = (1 ^ tm->rev);
    }
    Merge(t_, tl, tm);
    Merge(t_, t_, tr);
}

template <class T, class F>
void Treap<T, F>::Node::Modify(std::shared_ptr<Treap<T, F>::Node>& t_, size_t l, size_t r, T modif) {
    t_->Update();
    if (r <= l) {
        return;
    }
    std::shared_ptr<Node> tl;
    std::shared_ptr<Node> tm;
    std::shared_ptr<Node> tr;
    Split(t_, t_, tr, r);
    Split(t_, tl, tm, l);
    if (tm.get() != nullptr) {
        tm->ModifyUpdater(modif);
    }
    Merge(t_, tl, tm);
    Merge(t_, t_, tr);
}

template <class T, class F>
void Treap<T, F>::Node::MoveToEnd(std::shared_ptr<Node>& t_, size_t ls) {
    t_->Update();
    if (ls == 0 || ls == t_->siz) {
        return;
    }
    std::shared_ptr<Node> tl;
    std::shared_ptr<Node> tr;
    Split(t_, tl, tr, ls);
    Merge(t_, tr, tl);
}


template <class T, class F>
T* Treap<T, F>::Node::Get(size_t ind) {
    Update();
    if (ind == (ll.get() == nullptr ? 0 : ll->siz)) {
        return &val;
    }
    if (ind < (ll.get() == nullptr ? 0 : ll->siz)) {
        if (ll.get() == nullptr) {
            return nullptr;
        }
        return ll->Get(ind);
    }
    if (rr.get() == nullptr) {
        return nullptr;
    }
    return rr->Get(ind - (ll.get() == nullptr ? 0 : ll->siz) - 1);
}

template <class T, class F>
void Treap<T, F>::Node::Merge(std::shared_ptr<Treap<T, F>::Node>& rez, 
                              std::shared_ptr<Treap<T, F>::Node> ll, 
                              std::shared_ptr<Treap<T, F>::Node> rr) {
    if (ll.get() != nullptr) {
        ll->Update();
    }
    if (rr.get() != nullptr) {
        rr->Update();
    }
    if (ll.get() == nullptr || rr.get() == nullptr) {
        rez = (rr.get() == nullptr ? ll : rr);
    } else if (ll->priority < rr->priority) {
        Merge(rr->ll, ll, rr->ll);
        rez = rr;
    } else {
        Merge(ll->rr, ll->rr, rr);
        rez = ll;
    }
    rez->UpdateSiz();
}

template <class T, class F>
void Treap<T, F>::Node::Split(std::shared_ptr<Treap<T, F>::Node> what, 
                              std::shared_ptr<Treap<T, F>::Node>& ll, 
                              std::shared_ptr<Treap<T, F>::Node>& rr, int ls) {
    if (what.get() == nullptr) {
        ll.reset();
        rr.reset();
        return;
    }
    what->Update();
    if ((what->ll.get() != nullptr ? what->ll->siz : 0) >= ls) {
        Split(what->ll, ll, what->ll, ls);
        rr = what;
    } else {
        Split(what->rr, what->rr, rr, ls - (what->ll.get() != nullptr ? what->ll->siz : 0) - 1);
        ll = what;
    }
    what->UpdateSiz();
}

template <class T, class F>
std::shared_ptr<typename Treap<T, F>::Node> Treap<T, F>::Node::Construct(
        std::vector<std::shared_ptr<Treap<T, F>::Node>>& nodes, 
        int from, int top_priority) {
    if (nodes.size() == 0 || from >= nodes.size()) {
        return std::shared_ptr<Treap<T, F>::Node>();
    }
    std::shared_ptr<Treap<T, F>::Node> cur = nodes[from];
    for (int i = from + 1; i < nodes.size(); ++i) {
        if (top_priority < nodes[i]->priority) {
            return cur;
        } else if (cur->priority < nodes[i]->priority) {
            nodes[i]->ll = cur;
            cur = nodes[i];
            cur->UpdateSiz();
        } else {
            cur->rr = Construct(nodes, i, cur->priority);
            cur->UpdateSiz();
            i += cur->rr->siz - 1;
        }
    }
    return cur;
}

template <class T, class F>
std::shared_ptr<typename Treap<T, F>::Node> Treap<T, F>::Node::Construct(
        std::vector<std::shared_ptr<Treap<T, F>::Node>>& nodes) {
    if (nodes.size() == 0) {
        return nullptr;
    }
    std::shared_ptr<Treap<T, F>::Node> cur = nodes[0];
    for (int i = 1; i < nodes.size(); ++i) {
        if (cur->priority < nodes[i]->priority) {
            nodes[i]->ll = cur;
            cur = nodes[i];
            cur->UpdateSiz();
        } else {
            cur->rr = Construct(nodes, i, cur->priority);
            cur->UpdateSiz();
            i += cur->rr->siz - 1;
        }
    }
    return cur;
}

template <class T, class F>
void Treap<T, F>::Node::ToVector(std::vector<T>& v, size_t l, size_t r) {
    Update();
    if (ll.get() != nullptr) {
        if (ll->siz > l) {
            ll->ToVector(v, l, r);
        }
        l = (l > ll->siz ? l - ll->siz : 0);
        r = (r > ll->siz ? r - ll->siz : 0);
    }
    if (l == 0 && r > 0) {
        v.push_back(val);
    }
    l = (l > 0 ? l - 1 : 0);
    r = (r > 0 ? r - 1 : 0);
    if (rr.get() != nullptr && r > 0) {
        rr->ToVector(v, l, r);
    }
}

template <class T, class F>
std::shared_ptr<typename Treap<T, F>::Node> Treap<T, F>::Node::GetLeft() {
    Update();
    return ll;
}

template <class T, class F>
std::shared_ptr<typename Treap<T, F>::Node> Treap<T, F>::Node::GetRight() {
    Update();
    return rr;
}

template <class T, class F>
T& Treap<T, F>::Node::GetVal() {
    Update();
    return val;
}

template <class T, class F>
size_t Treap<T, F>::Node::GetSize() {
    Update();
    return siz;
}

template <class T, class F>
Treap<T, F>::Treap(const std::vector<T>& data, F funct, bool random, int seed) :
        root(), func(funct), size(data.size()) {
    std::mt19937 rnd(random ? (unsigned)std::chrono::steady_clock::now().time_since_epoch().count() : seed);
    std::vector<std::shared_ptr<Treap<T, F>::Node>> treap;
    for (int i = 0; i < data.size(); ++i) {
        treap.push_back(std::make_shared<Treap<T, F>::Node>(data[i], rnd(), func));
    }
    root = Node::Construct(treap);
}

template <class T, class F>
Treap<T, F>::Treap(const Treap& treap) : 
        root(), func(treap.func), size(treap.size) {
    if (treap.root.get() != nullptr) {
        root = std::make_shared<Treap<T, F>::Node>(*treap.root);
    } else {
        root.reset();
    }        
}

template <class T, class F>
Treap<T, F>::Treap(Treap&& treap) : 
        root(treap.root), func(treap.func), size(treap.size) {
    treap.root.reset();
    treap.size = 0;
}

template <class T, class F>
Treap<T, F>& Treap<T, F>::operator=(const Treap& treap) {
    root.reset();
    if (treap.root.get() != nullptr) {
        root = std::make_shared<Treap<T, F>::Node>(*treap.root);
    } else {
        root.reset();
    }
    func = treap.func;
    size = treap.size;
}

template <class T, class F>
Treap<T, F>& Treap<T, F>::operator=(Treap&& treap) {
    root.reset();
    root = treap.root;
    func = treap.func;
    size = treap.size;
    treap.root.reset();
    treap.size = 0;
    return *this;
}

template <class T, class F>
void Treap<T, F>::Reverse(size_t l, size_t r) {
    l = std::max(l, size_t(0));
    r = std::min(r, size);
    if (l >= r) {
        return;
    }
    if (root.get() != nullptr) {
        root->Reverse(root, l, r);
    }
}

template <class T, class F>
void Treap<T, F>::Modify(T modif, size_t l, size_t r) {
    l = std::max(l, size_t(0));
    r = std::min(r, size);
    if (l >= r) {
        return;
    }
    if (root.get() != nullptr) {
        root->Modify(root, l, r, modif);
    }
}

template <class T, class F>
void Treap<T, F>::MoveToEnd(size_t ls) {
    ls = std::max(ls, size_t(0));
    ls = std::min(ls, size - 1);
    if (root.get() != nullptr) {
        root->MoveToEnd(root, ls);
    }
}

template <class T, class F>
T Treap<T, F>::GetAt(size_t ind) {
    T* rez = root->Get(ind);
    if (rez == nullptr) {
        throw std::length_error("Outside of memory");
    }
    return T(*rez);
}

template <class T, class F>
std::vector<T> Treap<T, F>::ToVector(size_t l, size_t r) {
    l = std::max(l, size_t(0));
    r = std::min(r, size);
    std::vector<T> ans;
    if (r > l) {
        root->ToVector(ans, l, r);
    }
    return ans;
}

template <class T, class F>
void Treap<T, F>::Merge(Treap* rez, Treap* ll, Treap* rr) {
    if (rez != nullptr) {
        rez->root.reset();
    } else {
        rez = new Treap<T, F>({}, F());
    }
    Node::Merge(rez->root, ll->root, rr->root);
    rez->size = ll->size + rr->size;
    ll->root.reset();
    rr->root.reset();
    ll->size = 0;
    rr->size = 0;
}

template <class T, class F>
void Treap<T, F>::Split(Treap* what, Treap* ll, Treap* rr, int ls) {
    if (ll != nullptr) {
        ll->root.reset();
        ll->func = what->func;
    } else {
        ll = new Treap<T, F>({}, what->func);
    }
    if (rr != nullptr) {
        rr->root.reset();
        rr->func = what->func;
    } else {
        rr = new Treap<T, F>({}, what->func);
    }
    Node::Split(what->root, ll->root, rr->root, ls);
    ll->size = ls;
    rr->size = what->size - ls;
    what->root.reset();
    what->size = 0;
}

template <class T, class F>
Treap<T, F>::Iterator::Iterator(const std::vector<std::shared_ptr<Treap<T, F>::Node>>& cur_path) :
        path(cur_path) {
}

template <class T, class F>
bool Treap<T, F>::Iterator::operator==(const Iterator& iter) {
    return path == iter.path;
}
        
template <class T, class F>
bool Treap<T, F>::Iterator::operator!=(const Iterator& iter) {
    return path != iter.path;
}
        
template <class T, class F>
typename Treap<T, F>::Iterator& Treap<T, F>::Iterator::operator++() {
    if (path.size() == 0) {
        return *this;
    }
    if (path.back()->GetRight().get() != nullptr) {
        path.push_back(path.back()->GetRight());
        while (path.back()->GetLeft().get() != nullptr) {
            path.push_back(path.back()->GetLeft());
        }
    } else {
        while (path.size() > 1 && path[path.size() - 2]->GetRight() == path.back()) {
            path.pop_back();
        }
        path.pop_back();
    }
    return *this;
}
        
template <class T, class F>
typename Treap<T, F>::Iterator Treap<T, F>::Iterator::operator++(int) {
    Iterator last(*this);
    operator++();
    return last;
}
        
template <class T, class F>
typename Treap<T, F>::Iterator& Treap<T, F>::Iterator::operator--() {
    if (path.size() == 0) {
        return *this;
    }
    if (path.back()->GetLeft().get() != nullptr) {
        path.push_back(path.back()->GetLeft());
        while (path.back()->GetRight().get() != nullptr) {
            path.push_back(path.back()->GetRight());
        }
    } else {
        while (path.size() > 1 && path[path.size() - 2]->GetLeft() == path.back()) {
            path.pop_back();
        }
        path.pop_back();
    }
    return *this;
}
        
template <class T, class F>
typename Treap<T, F>::Iterator Treap<T, F>::Iterator::operator--(int) {
    Iterator last(*this);
    operator--();
    return last;
}
        
template <class T, class F>
T& Treap<T, F>::Iterator::operator*() {
    return path.back()->GetVal();
}
        
template <class T, class F>
T* Treap<T, F>::Iterator::operator->() {
    return path.back()->val().get();
}

template <class T, class F>
typename Treap<T, F>::Iterator Treap<T, F>::begin() {
    std::vector<std::shared_ptr<Node>> cur_path;
    cur_path.push_back(root);
    while (cur_path.back().get() != nullptr && cur_path.back()->GetLeft().get() != nullptr) {
        cur_path.push_back(cur_path.back()->GetLeft());
    }
    return Iterator(cur_path);
}
    
template <class T, class F>
typename Treap<T, F>::Iterator Treap<T, F>::end() {
    std::vector<std::shared_ptr<Node>> cur_path;
    return Iterator(cur_path);
}
    
template <class T, class F>
typename Treap<T, F>::Iterator Treap<T, F>::IterAt(size_t ind) {
    std::vector<std::shared_ptr<Node>> cur_path;
    std::shared_ptr<Node> cur = root;
    while (ind < size && cur != nullptr) {
        cur_path.push_back(cur);
        size_t left_size = (cur->GetLeft().get() == nullptr ? 0 : cur->GetLeft()->GetSize());
        if (ind == left_size) {
            break;
        } else if (ind < left_size) {
            cur = cur->GetLeft();
        } else {
            ind -= left_size;
            --ind;
            cur = cur->GetRight();
        }
    }
    return Iterator(cur_path);
}
