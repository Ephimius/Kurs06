#include "pertrip_library.h"

#include <chrono>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

template <class T, class F>
void PerTreap<T, F>::Node::ModifyUpdater(std::shared_ptr<T> modificator) {
    if (modif) {
        *val_mod = func(*val_mod, *modificator);
    } else {
        modif = true;
        *val_mod = *modificator;
    }
}

template <class T, class F>
void PerTreap<T, F>::Node::Update() {
    if (modif) {
        modif = false;
        if (ll.get() != nullptr) {
            ll->ModifyUpdater(val_mod);
        }
        if (rr.get() != nullptr) {
            rr->ModifyUpdater(val_mod);
        }
        *val = func(*val, *val_mod);
    }
}

template <class T, class F>
void PerTreap<T, F>::Node::UpdateSiz() {
    siz = 1;
    if (ll.get() != nullptr) {
        siz += ll->siz;
    }
    if (rr.get() != nullptr) {
        siz += rr->siz;
    }
}

template <class T, class F>
PerTreap<T, F>::Node::Node(T value, int prior, F funct) :
        priority(prior), siz(1), val(std::make_shared<T>(value)), 
        val_mod(std::make_shared<T>(T())), ll(), rr(), 
        func(funct), modif(false) {
}

template <class T, class F>
PerTreap<T, F>::Node::Node(std::shared_ptr<T> value, int prior, F funct) :
        priority(prior), siz(1), val(value),
        val_mod(std::make_shared<T>(T())), ll(), rr(),
        func(funct), modif(false) {
}

template <class T, class F>
PerTreap<T, F>::Node::Node(const Node& node) :
        priority(node.priority), siz(node.siz), val(std::make_shared<T>(*node.val)),
        val_mod(std::make_shared<T>(*node.val_mod)), ll(), rr(), 
        func(node.func), modif(node.modif) {
    if (node.ll.get() != nullptr) {
        ll = std::make_shared<Node>(*node.ll);
    }
    if (node.rr.get() != nullptr) {
        rr = std::make_shared<Node>(*node.rr);
    }
}

template <class T, class F>
PerTreap<T, F>::Node::Node(Node&& node) :
        priority(node.priority), siz(node.siz), ll(node.ll), rr(node.rr) {
    node.siz = 1;
    val = node.val;
    val_mod = node.val_mod;
    node.val.reset();
    node.val_mod.reset();
    func = std::move(node.func);
    modif = std::move(node.modif);
    node.ll.reset();
    node.rr.reset();
}

template <class T, class F>
typename PerTreap<T, F>::Node& PerTreap<T, F>::Node::operator=(const Node& node) {
    priority = node.priority;
    siz = node.siz;
    val = std::make_shared(*node.val);
    val_mod = std::make_shared(*node.val_mod);
    ll.reset();
    rr.reset();
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
typename PerTreap<T, F>::Node& PerTreap<T, F>::Node::operator=(Node&& node) {
    priority = node.priority;
    siz = node.siz;
    ll = node.ll;
    rr = node.rr;
    node.siz = 1;
    val = node.val;
    val_mod = node.val_mod;
    node.val.reset();
    node.val_mod.reset();
    func = std::move(node.func);
    modif = std::move(node.modif);
    node.ll.reset();
    node.rr.reset();
    return *this;
}

template <class T, class F>
PerTreap<T, F>::Node::Node(const PerTreap<T, F>::Node& node, 
                                  std::map<std::shared_ptr<PerTreap<T, F>::Node>, 
                                          std::shared_ptr<PerTreap<T, F>::Node>>& nodes) :
                                  priority(node.priority), siz(node.siz), 
                                  val(std::make_shared<T>(*node.val)),
                                  val_mod(std::make_shared<T>(*node.val_mod)), 
                                  ll(), rr(), 
                                  func(node.func), modif(node.modif) {
    if (node.ll.get() != nullptr) {
        if (nodes.count(node.ll) == 0) {
            ll = std::make_shared<Node>(*(node.ll), nodes);
            nodes.insert({node.ll, ll});
        } else {
            ll = nodes[node.ll];
        }
    }
    if (node.rr.get() != nullptr) {
        if (nodes.count(node.rr) == 0) {
            rr = std::make_shared<Node>(*(node.rr), nodes);
            nodes.insert({node.rr, rr});
        } else {
            rr = nodes[node.rr];
        }
    }
    UpdateSiz();
}

template <class T, class F>
void PerTreap<T, F>::Node::Clone(const Node& node) {
    priority = node.priority;
    siz = node.siz;
    val = node.val;
    ll = node.ll;
    rr = node.rr;
    val_mod = node.val_mod;
    func = node.func;
    modif = node.modif;
}

template<class T, class F>
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::Modify(
            std::shared_ptr<Node> t_, size_t l, size_t r, 
            std::shared_ptr<T> modif) {
    t_->Update();
    std::shared_ptr<Node> tl;
    std::shared_ptr<Node> tm;
    std::shared_ptr<Node> tmn;
    std::shared_ptr<Node> tr;
    std::shared_ptr<Node> tnew = std::make_shared<Node>(T(), 0, F());
    std::shared_ptr<Node> tnewn = std::make_shared<Node>(T(), 0, F());
    if (r <= l) {
        return tnewn;
    }
    Split(t_, tmn, tr, r);
    Split(tmn, tl, tm, l);
    if (tm.get() != nullptr) {
        tm->ModifyUpdater(modif);
    }
    Merge(tnew, tl, tm);
    Merge(tnewn, tnew, tr);
    return tnewn;
}

template<class T, class F>
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::MoveToEnd(
        std::shared_ptr<PerTreap<T, F>::Node> t_, size_t ls) {
    t_->Update();
    std::shared_ptr<Node> tl;
    std::shared_ptr<Node> tr;
    std::shared_ptr<Node> tnew;
    if (ls == 0 || ls == t_->siz) {
        return tnew;
    }
    Split(t_, tl, tr, ls);
    Merge(tnew, tr, tl);
    return tnew;
}


template <class T, class F>
std::shared_ptr<T> PerTreap<T, F>::Node::Get(size_t ind) {
    Update();
    if (ind == (ll.get() == nullptr ? 0 : ll->siz)) {
        return val;
    }
    if (ind < (ll.get() == nullptr ? 0 : ll->siz)) {
        if (ll.get() == nullptr) {
            return std::shared_ptr<T>();
        }
        return ll->Get(ind);
    }
    if (rr.get() == nullptr) {
        return std::shared_ptr<T>();
    }
    return rr->Get(ind - (ll.get() == nullptr ? 0 : ll->siz) - 1);
}

template <class T, class F>
void PerTreap<T, F>::Node::Merge(std::shared_ptr<PerTreap<T, F>::Node>& rez, 
                              std::shared_ptr<PerTreap<T, F>::Node> ll, 
                              std::shared_ptr<PerTreap<T, F>::Node> rr) {
    //std::cerr << "MERGE ";
    rez.reset();
    std::shared_ptr<Node> new_node;
    if (ll.get() != nullptr) {
        ll->Update();
    }
    if (rr.get() != nullptr) {
        rr->Update();
    }
    if (ll.get() == nullptr || rr.get() == nullptr) {
        rez = (rr.get() == nullptr ? ll : rr);
    } else if (ll->priority < rr->priority) {
        Merge(new_node, ll, rr->ll);
        rez = std::make_shared<Node>(T(), 0, F());
        rez->Clone(*rr);
        rez->ll = new_node;
    } else {
        Merge(new_node, ll->rr, rr);
        rez = std::make_shared<Node>(T(), 0, F());
        rez->Clone(*ll);
        rez->rr = new_node;
    }
    rez->UpdateSiz();
}

template <class T, class F>
void PerTreap<T, F>::Node::Split(std::shared_ptr<PerTreap<T, F>::Node> what, 
                              std::shared_ptr<PerTreap<T, F>::Node>& ll, 
                              std::shared_ptr<PerTreap<T, F>::Node>& rr, int ls) {
    //std::cerr << "SPLIT ";
    ll.reset();
    rr.reset();
    std::shared_ptr<Node> new_node;
    if (what.get() == nullptr) {
        return;
    }
    what->Update();
    if ((what->ll.get() != nullptr ? what->ll->siz : 0) >= ls) {
        rr = std::make_shared<Node>(T(), 0, F());
        Split(what->ll, ll, new_node, ls);
        rr->Clone(*what);
        rr->ll = new_node;
        rr->UpdateSiz();
    } else {
        ll = std::make_shared<Node>(T(), 0, F());
        Split(what->rr, new_node, rr, ls - (what->ll.get() != nullptr ? what->ll->siz : 0) - 1);
        ll->Clone(*what);
        ll->rr = new_node;
        ll->UpdateSiz();
    }
    what->UpdateSiz();
}

template <class T, class F>
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::Construct(
        std::vector<std::shared_ptr<PerTreap<T, F>::Node>>& nodes, 
        int from, int top_priority) {
    if (nodes.size() == 0 || from >= nodes.size()) {
        return std::shared_ptr<PerTreap<T, F>::Node>();
    }
    std::shared_ptr<PerTreap<T, F>::Node> cur = nodes[from];
    for (int i = from + 1; i < nodes.size(); ++i) {
        if (top_priority < nodes[i]->priority) {
            //std::cout << cur->siz << " ";
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
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::Construct(
        std::vector<std::shared_ptr<PerTreap<T, F>::Node>>& nodes) {
    if (nodes.size() == 0) {
        return nullptr;
    }
    std::shared_ptr<PerTreap<T, F>::Node> cur = nodes[0];
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
void PerTreap<T, F>::Node::ToVector(std::vector<T>& v, size_t l, size_t r) {
    Update();
    if (ll.get() != nullptr) {
        if (ll->siz > l) {
            ll->ToVector(v, l, r);
        }
        l = (l > ll->siz ? l - ll->siz : 0);
        r = (r > ll->siz ? r - ll->siz : 0);
    }
    if (l == 0 && r > 0) {
        v.push_back(*val);
    }
    l = (l > 0 ? l - 1 : 0);
    r = (r > 0 ? r - 1 : 0);
    if (rr.get() != nullptr && r > 0) {
        rr->ToVector(v, l, r);
    }
}

template <class T, class F>
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::GetLeft() {
    Update();
    return ll;
}

template <class T, class F>
std::shared_ptr<typename PerTreap<T, F>::Node> PerTreap<T, F>::Node::GetRight() {
    Update();
    return rr;
}

template <class T, class F>
T& PerTreap<T, F>::Node::GetVal() {
    Update();
    return *val;
}

template <class T, class F>
size_t PerTreap<T, F>::Node::GetSize() {
    Update();
    return siz;
}

template <class T, class F>
void PerTreap<T, F>::Node::Refresh() {
    if (this != nullptr) {
        Update();
        ll->Refresh();
        rr->Refresh();
    }
    return;
}

template <class T, class F>
PerTreap<T, F>::PerTreap(const std::vector<T>& data, F funct, bool random, int seed) :
        func(funct), size(data.size()), refreshed(true) {
    std::mt19937 rnd(random ? (unsigned)std::chrono::steady_clock::now().time_since_epoch().count() : seed);
    std::vector<std::shared_ptr<PerTreap<T, F>::Node>> treap;
    for (int i = 0; i < data.size(); ++i) {
        treap.push_back(std::make_shared<PerTreap<T, F>::Node>(data[i], rnd(), func));
    }
    roots.push_back(Node::Construct(treap));
}

template <class T, class F>
PerTreap<T, F>::PerTreap(const PerTreap& treap) : 
        func(treap.func), size(treap.size), refreshed(treap.refreshed) {
    std::map<std::shared_ptr<PerTreap<T, F>::Node>, std::shared_ptr<PerTreap<T, F>::Node>> nodes;
    for (size_t i = 0; i < treap.roots.size(); ++i) {
        if (treap.roots[i].get() != nullptr) {
            roots.push_back(std::make_shared<PerTreap<T, F>::Node>(*treap.roots[i], nodes));
        }
    }     
}

template <class T, class F>
PerTreap<T, F>::PerTreap(PerTreap&& treap) : 
        func(treap.func), size(treap.size), refreshed(treap.refreshed) {
    roots = std::move(treap.roots);
    treap.size = 0;
}

template <class T, class F>
PerTreap<T, F>& PerTreap<T, F>::operator=(const PerTreap& treap) {
    roots.clear();
    std::map<std::shared_ptr<PerTreap<T, F>::Node>, std::shared_ptr<PerTreap<T, F>::Node>> nodes;
    for (size_t i = 0; i < treap.roots.size(); ++i) {
        if (treap.roots[i].get() != nullptr) {
            roots.push_back(std::make_shared<PerTreap<T, F>::Node>(*treap.roots[i], nodes));
        }
    }
    func = treap.func;
    size = treap.size;
    refreshed = treap.refreshed;
}

template <class T, class F>
PerTreap<T, F>& PerTreap<T, F>::operator=(PerTreap&& treap) {
    roots = std::move(treap.roots);
    func = treap.func;
    size = treap.size;
    refreshed = treap.refreshed;
    treap.roots.clear();
    treap.size = 0;
    return *this;
}

template <class T, class F>
size_t PerTreap<T, F>::GetNumberOfStates() {
    return roots.size();
}

template <class T, class F>
void PerTreap<T, F>::Modify(T modif, size_t l, size_t r, size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    r = std::min(r, size);
    if (l >= r || state >= roots.size()) {
        return;
    }
    if (roots[state].get() != nullptr) {
        roots.push_back(Node::Modify(roots[state], l, r, std::make_shared<T>(modif)));
    }
    refreshed = false;
}

template <class T, class F>
void PerTreap<T, F>::Modify(T modif, size_t l, size_t r) {
    Modify(modif, l, r, roots.size() - 1);
}

template <class T, class F>
void PerTreap<T, F>::MoveToEnd(size_t ls, size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    ls = std::max(ls, size_t(0));
    ls = std::min(ls, size - 1);
    if (state < roots.size() && roots[state].get() != nullptr) {
        roots.push_back(Node::MoveToEnd(roots[state], ls));
    }
}

template <class T, class F>
void PerTreap<T, F>::MoveToEnd(size_t ls) {
    MoveToEnd(ls, roots.size() - 1);
}

template <class T, class F>
T PerTreap<T, F>::GetAt(size_t ind, size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    if (state >= roots.size()) {
        throw std::length_error("Outside of memory");
    }
    std::shared_ptr<T> rez = roots[state]->Get(ind);
    if (rez.get() == nullptr) {
        throw std::length_error("Outside of memory");
    }
    return T(*rez);
}

template <class T, class F>
T PerTreap<T, F>::GetAt(size_t ind) {
    return GetAt(ind, roots.size() - 1);
}

template <class T, class F>
std::vector<T> PerTreap<T, F>::ToVector(size_t l, size_t r, size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    if (state >= roots.size()) {
        throw std::length_error("Outside of memory");
    }
    l = std::max(l, size_t(0));
    r = std::min(r, size);
    std::vector<T> ans;
    if (r > l) {
        roots[state]->ToVector(ans, l, r);
    }
    return ans;
}

template <class T, class F>
std::vector<T> PerTreap<T, F>::ToVector(size_t l, size_t r) {
    return ToVector(l, r, roots.size() - 1);
}

template <class T, class F>
void PerTreap<T, F>::Merge(PerTreap* rez, PerTreap* ll, size_t state_l, 
                      PerTreap* rr, size_t state_r) {
    if (state_l < ll->roots.size() - 1 && !ll->refreshed) {
        ll->roots.back()->Refresh();
        ll->refreshed = true;
    }
    if (state_r < rr->roots.size() - 1 && !rr->refreshed) {
        rr->roots.back()->Refresh();
        rr->refreshed = true;
    }
    if (rez == nullptr) {
        rez = new PerTreap<T, F>({}, F());
    }
    rez->roots.clear();
    std::shared_ptr<Node> temp_root;
    Node::Merge(temp_root, ll->roots[state_l], rr->roots[state_r]);
    rez->roots.push_back(temp_root);
    rez->size = ll->size + rr->size;
    ll->roots.clear();
    rr->roots.clear();
    ll->size = 0;
    rr->size = 0;
}

template <class T, class F>
void PerTreap<T, F>::Merge(PerTreap* rez, PerTreap* ll, PerTreap* rr) {
    Merge(rez, ll, ll->roots.size() - 1, rr, rr->roots.size() - 1);
}

template <class T, class F>
void PerTreap<T, F>::Split(PerTreap* what, size_t state_w, PerTreap* ll, 
                      PerTreap* rr, int ls) {
    if (state_w < what->roots.size() - 1 && !what->refreshed) {
        what->roots.back()->Refresh();
        what->refreshed = true;
    }
    if (ll != nullptr) {
        ll->roots.clear();
        ll->func = what->func;
    } else {
        ll = new PerTreap<T, F>({}, what->func);
    }
    if (rr != nullptr) {
        rr->roots.clear();
        rr->func = what->func;
    } else {
        rr = new PerTreap<T, F>({}, what->func);
    }
    std::shared_ptr<Node> temp_rootl;
    std::shared_ptr<Node> temp_rootr;
    Node::Split(what->roots[state_w], temp_rootl, temp_rootr, ls);
    ll->roots.push_back(temp_rootl);
    rr->roots.push_back(temp_rootr);
    ll->size = ls;
    rr->size = what->size - ls;
    what->roots.clear();
    what->size = 0;
}

template <class T, class F>
void PerTreap<T, F>::Split(PerTreap* what, PerTreap* ll,  PerTreap* rr, int ls) {
    Split(what, what->roots.size() - 1, ll, rr, ls);
}

template <class T, class F>
PerTreap<T, F>::Iterator::Iterator(const std::vector<std::shared_ptr<PerTreap<T, F>::Node>>& cur_path) :
        path(cur_path) {
}

template <class T, class F>
bool PerTreap<T, F>::Iterator::operator==(const Iterator& iter) {
    return path == iter.path;
}
        
template <class T, class F>
bool PerTreap<T, F>::Iterator::operator!=(const Iterator& iter) {
    return path != iter.path;
}
        
template <class T, class F>
typename PerTreap<T, F>::Iterator& PerTreap<T, F>::Iterator::operator++() {
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
typename PerTreap<T, F>::Iterator PerTreap<T, F>::Iterator::operator++(int) {
    Iterator last(*this);
    operator++();
    return last;
}
        
template <class T, class F>
typename PerTreap<T, F>::Iterator& PerTreap<T, F>::Iterator::operator--() {
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
typename PerTreap<T, F>::Iterator PerTreap<T, F>::Iterator::operator--(int) {
    Iterator last(*this);
    operator--();
    return last;
}
        
template <class T, class F>
T& PerTreap<T, F>::Iterator::operator*() {
    return path.back()->GetVal();
}
        
template <class T, class F>
T* PerTreap<T, F>::Iterator::operator->() {
    return path.back()->val().get();
}

template <class T, class F>
typename PerTreap<T, F>::Iterator PerTreap<T, F>::begin(size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    std::vector<std::shared_ptr<Node>> cur_path;
    cur_path.push_back(roots[state]);
    while (cur_path.back().get() != nullptr && cur_path.back()->GetLeft().get() != nullptr) {
        cur_path.push_back(cur_path.back()->GetLeft());
    }
    return Iterator(cur_path);
}

template <class T, class F>
typename PerTreap<T, F>::Iterator PerTreap<T, F>::begin() {
    return begin(roots.size() - 1);
}
    
template <class T, class F>
typename PerTreap<T, F>::Iterator PerTreap<T, F>::end() {
    std::vector<std::shared_ptr<Node>> cur_path;
    return Iterator(cur_path);
}
    
template <class T, class F>
typename PerTreap<T, F>::Iterator PerTreap<T, F>::IterAt(size_t ind, size_t state) {
    if (state < roots.size() - 1 && !refreshed) {
        roots.back()->Refresh();
        refreshed = true;
    }
    std::vector<std::shared_ptr<Node>> cur_path;
    std::shared_ptr<Node> cur = roots[state];
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
    
template <class T, class F>
typename PerTreap<T, F>::Iterator PerTreap<T, F>::IterAt(size_t ind) {
    return IterAt(ind, roots.size() - 1);
}
