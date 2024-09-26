
template<typename K, typename V>
class rb_tree;

template<typename K, typename V>
class rb_tree_root{
public:
    rb_tree<K, V> rb_node;    
    static void set(int (*set_compare)(V this_value, V another_value)) {
        compare = set_compare;
    }
private:
    static int (*compare)(V this_value, V another_value);
};

template<typename K, typename V>
class rb_tree {
public:
    K key;
    V value;


public:

    rb_tree(K key, V value) {
        this->key = key;
        this->value = value;
    }
};