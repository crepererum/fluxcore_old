#ifndef FLUXCORE_INDEX_HPP
#define FLUXCORE_INDEX_HPP

#include <iostream>

#include <cstring>
#include <list>
#include <stdexcept>

#include "provider/abstractprovider.hpp"

namespace fluxcore {

/* Index that maps keys to <std::size_t>
 *
 * @K type of the keys
 * @nodeSize number of stored keys per node
 *
 * The index is implemented as a B+ tree. The <nodeSize> should be chosen according to the application. The bigger the number of stored records, the bigger should <nodeSize> be.
 */
template <typename K, std::size_t nodeSize = 2>
class Index {
    public:
        /* Creates new index
         *
         * @provider_ StorageProvider used to store the index data
         */
        explicit Index(const provider_t& provider_) : provider(provider_) {
            Segment s = provider->createSegment(sizeof(Node)); // waste some space to enable usage of block providers
            rootNode = static_cast<std::size_t*>(s.ptr());
            *rootNode = 0;
            id = s.id();
        }

        /* Loads an index
         *
         * @provider_ StorageProvider used to store the index data
         * @id_ id of the root anchor, which is used to load the index
         *
         * Warning: Providing an illegal root ID leads to undefinied behavoir!
         */
        Index(const provider_t& provider_, std::size_t id_) : provider(provider_), id(id_) {
            Segment s = provider->getSegment(id);
            rootNode = static_cast<std::size_t*>(s.ptr());
        }

        /* Returns id of the root anchor
         *
         * @return id of the root anchor
         *
         * This id can be used to restore the index later by using the load constructor.
         */
        std::size_t getID() const {
            return id;
        }

        /* Dumps index tree to a given <ostream>
         *
         * @os ostream that recieves the dump
         *
         * Warning: This method requires a valid state, otherwise it can lead to undefinied behavior!
         *
         * This method should mostly used for debugging. The ostream should be able to recieve endlines (<std::endl>). The ostream won't be flushed.
         */
        void dump(std::ostream& os) {
            dumpNode(*rootNode, os);
        }

        /* Checks if the index is empty
         *
         * @return <true> if the index is empty
         */
        bool empty() const {
            return *rootNode == 0;
        }

        /* Returns the first record in the index
         *
         * Warning: This method should only be used when the index is NOT empty!
         */
        std::pair<K, std::size_t> first() const {
            if (*rootNode == 0) {
                throw std::runtime_error("Index is empty!");
            }

            std::size_t current = *rootNode;
            const K* k = nullptr;
            const std::size_t* record = nullptr;

            while (current != 0) {
                Segment s = provider->getSegment(current);
                Node* n = static_cast<Node*>(s.ptr());

                if (!n->leaf) {
                    current = n->children[0];
                } else {
                    current = 0;
                    k = &(n->keys[0]);
                    record = &(n->children[0]);
                }
            }

            return std::make_pair(*k, *record);
        }

        /* Returns the last record in the index
         *
         * Warning: This method should only be used when the index is NOT empty!
         */
        std::pair<K, std::size_t> last() const {
            if (*rootNode == 0) {
                throw std::runtime_error("Index is empty!");
            }

            std::size_t current = *rootNode;
            const K* k = nullptr;
            const std::size_t* record = nullptr;

            while (current != 0) {
                Segment s = provider->getSegment(current);
                Node* n = static_cast<Node*>(s.ptr());

                if (!n->leaf) {
                    current = n->children[n->filled];
                } else {
                    current = 0;
                    k = &(n->keys[n->filled - 1]);
                    record = &(n->children[n->filled - 1]);
                }
            }

            return std::make_pair(*k, *record);
        }

        /* Inserts new key and record to index
         *
         * @key new, unique key that sould inserted
         * @record record that gets assoziated to the key
         *
         * Warning: The key has to be new and unique, otherwise it leads to undefinied behavior!
         */
        void insert(const K& key, const std::size_t record) {
            // Step 1: walk down and find insert point
            std::list<std::size_t> history = walkDown(key);

            // Step 2: push data up to the root
            auto step = getParentNode(history);
            K stepKey = key;
            std::size_t child1 = record;
            std::size_t child2 = 0;

            // rise up and look for space
            while (std::get<1>(step)->full()) {
                // prepate 2 new nodes
                Segment s1 = provider->createSegment(sizeof(Node));
                Segment s2 = provider->createSegment(sizeof(Node));
                memset(s1.ptr(), 0, sizeof(Node));
                memset(s2.ptr(), 0, sizeof(Node));
                Node* n1 = static_cast<Node*>(s1.ptr());
                Node* n2 = static_cast<Node*>(s2.ptr());
                n1->leaf = step.second->leaf;
                n2->leaf = step.second->leaf;
                n1->left = step.second->left;
                n1->right = s2.id();
                n2->left = s1.id();
                n2->right = step.second->right;

                // update neigbors
                if (n1->left != 0) {
                    Segment tmpS = provider->getSegment(n1->left);
                    Node* tmpN = static_cast<Node*>(tmpS.ptr());
                    tmpN->right = s1.id();
                }
                if (n2->right != 0) {
                    Segment tmpS = provider->getSegment(n2->right);
                    Node* tmpN = static_cast<Node*>(tmpS.ptr());
                    tmpN->left = s2.id();
                }

                // split, free, next
                stepKey = step.second->split(stepKey, n1, n2, child1, child2);
                provider->freeSegment(step.first.id());
                step = getParentNode(history);
                child1 = s1.id();
                child2 = s2.id();
            }

            // finally add data to a non-full node
            step.second->add(stepKey, child1, child2);
        }

        /* Erases exisiting key and assoziated record from index
         *
         * @key the key that should be removed
         *
         * Warning: The key has to exist, otherwise it leads to undefinied behavior!
         */
        void erase(const K& key) {
            // Step 1: find leaf
            std::list<std::size_t> history = walkDown(key);

            // Step 2: delete until tree is happy
            auto step = getParentNode(history);
            step.second->removeByKeyLeft(key);

            while (!history.empty() && (step.second->filled < nodeSize / 2)) {
                auto parentStep = getParentNode(history);
                K* separator = nullptr;

                // find sibling to re-distribute
                if ((step.second->left != 0) && (separator = parentStep.second->getSeparator(step.second->left, step.first.id()))) {
                    Segment s = provider->getSegment(step.second->left);
                    Node* n = static_cast<Node*>(s.ptr());

                    if (step.second->filled + n->filled >= nodeSize) {
                        if (step.second->leaf) {
                            // => shift to right
                            // get relevant information
                            K k = n->keys[n->filled - 1];
                            std::size_t c1 = n->children[n->filled - 1];

                            // remove element from left node
                            memset(&n->keys[n->filled - 1], 0, sizeof(K));
                            n->children[n->filled - 1] = 0;
                            --n->filled;

                            // add element to right node
                            step.second->add(k, c1, step.second->children[0]);

                            // fix separator
                            *separator = k;
                        } else {
                            // => rotate clockwise
                            // get relevant information
                            K k = n->keys[n->filled - 1];
                            std::size_t c1 = n->children[n->filled];

                            // remove element from left node
                            memset(&n->keys[n->filled - 1], 0, sizeof(K));
                            n->children[n->filled] = 0;
                            --n->filled;

                            // add element to right node
                            step.second->add(*separator, c1, step.second->children[0]);

                            // fix separator
                            *separator = k;
                        }
                    } else {
                        // merge
                        n->mergeWith(step.second, *separator);
                        n->right = step.second->right;
                        if (step.second->right != 0) {
                            Segment tmpS = provider->getSegment(step.second->right);
                            Node* tmpN = static_cast<Node*>(tmpS.ptr());
                            tmpN->left = s.id();
                        }
                        provider->freeSegment(step.first.id());
                        parentStep.second->removeByKeyRight(*separator);
                    }
                } else {
                    separator = parentStep.second->getSeparator(step.first.id(), step.second->right);
                    Segment s = provider->getSegment(step.second->right);
                    Node* n = static_cast<Node*>(s.ptr());

                    if (step.second->filled + n->filled >= nodeSize) {
                        if (step.second->leaf) {
                            // => shift to left
                            // get relevant information
                            K k = n->keys[0];
                            std::size_t c1 = n->children[0];

                            // remove element from left node
                            n->removeByKeyLeft(k);

                            // add element to right node
                            step.second->keys[step.second->filled] = k;
                            step.second->children[step.second->filled] = c1;
                            ++step.second->filled;

                            // fix separator
                            *separator = n->keys[0];
                        } else {
                            // => rotate anti-clockwise
                            // get relevant information
                            K k = n->keys[0];
                            std::size_t c1 = n->children[0];

                            // remove element from left node
                            n->removeByKeyLeft(k);

                            // add element to right node
                            step.second->keys[step.second->filled] = *separator;
                            step.second->children[step.second->filled + 1] = c1;
                            ++step.second->filled;

                            // fix separator
                            *separator = k;
                        }
                    } else {
                        // merge
                        step.second->mergeWith(n, *separator);
                        step.second->right = n->right;
                        if (n->right != 0) {
                            Segment tmpS = provider->getSegment(n->right);
                            Node* tmpN = static_cast<Node*>(tmpS.ptr());
                            tmpN->left = step.first.id();
                        }
                        provider->freeSegment(s.id());
                        parentStep.second->removeByKeyRight(*separator);
                    }
                }

                step = parentStep;
            }

            // cleanup root
            if (step.second->filled < nodeSize / 2) {
                *rootNode = step.second->children[0];
                provider->freeSegment(step.first.id());
            }
        }

    private:
        /* Tree node, can be internal or leaf
         *
         * Parent nodes are not stored because the can be extracted from the walk down history
         */
        struct Node {
            bool leaf;
            std::size_t left;
            std::size_t right;
            std::size_t filled;
            K keys[nodeSize];
            std::size_t children[nodeSize + 1];

            /* checks if node is full
             *
             * @return <true> if node is full
             */
            bool full() {
                return filled == nodeSize;
            }

            /* Gets separtor key of to children
             *
             * This method works only on direct siblings.
             */
            K* getSeparator(std::size_t child1, std::size_t child2) {
                for (std::size_t i = 0; i < filled; ++i) {
                    if ((children[i] == child1) && (children[i + 1] == child2)) {
                        return &keys[i];
                    }
                }

                return nullptr;
            }

            /* Adds entry to non-full node
             *
             * @key the key of the entry
             * @child1 left child for internal nodes, record pointer for leaf nodees
             * @child2 right child for internal nodes, ignored for leaf nodes
             */
            void add(const K& key, std::size_t child1, std::size_t child2) {
                std::size_t i;

                for (i = filled; (i > 0) && (keys[i - 1] > key); --i) {
                    keys[i] = keys[i - 1];
                    if (leaf) {
                        children[i] = children[i - 1];
                    } else {
                        children[i + 1] = children[i];
                    }
                }

                keys[i] = key;
                children[i] = child1;
                if (!leaf) {
                    children[i + 1] = child2;
                }
                ++filled;
            }

            /* Remove key and the left child
             */
            void removeByKeyLeft(const K& key) {
                std::size_t idx = 0;

                for (std::size_t i = 0; i < filled; ++i) {
                    if (keys[i] == key) {
                        memset(&keys[i], 0, sizeof(K));
                        children[i] = 0;
                    } else {
                        keys[idx] = keys[i];
                        children[idx] = children[i];
                        ++idx;
                    }
                }

                memset(&keys[filled - 1], 0, sizeof(K));
                children[idx] = children[filled];
                --filled;
            }

            /* Remove key and the right child
             */
            void removeByKeyRight(const K& key) {
                std::size_t idx = 0;

                for (std::size_t i = 0; i < filled; ++i) {
                    if (keys[i] == key) {
                        memset(&keys[i], 0, sizeof(K));
                        children[i + 1] = 0;
                    } else {
                        keys[idx] = keys[i];
                        children[idx + 1] = children[i + 1];
                        ++idx;
                    }
                }

                memset(&keys[filled - 1], 0, sizeof(K));
                --filled;
            }

            /* Merges this node with a node on the left side
             *
             * @other node on the left side
             * @separator for internal nodes a separator key gets merged in, ignored for leaf nodes
             */
            void mergeWith(Node* other, const K& separator) {
                std::size_t idxChildren = filled;
                std::size_t idxKeys = filled;

                if (!leaf) {
                    keys[idxKeys] = separator;
                    ++idxChildren;
                    ++idxKeys;
                }

                for (std::size_t i = 0; i < other->filled; ++i) {
                    keys[idxKeys] = other->keys[i];
                    children[idxChildren] = other->children[i];
                    ++idxKeys;
                    ++idxChildren;
                }

                if (!leaf) {
                    children[idxChildren] = other->children[other->filled];
                }

                filled = idxKeys;
            }

            /* Splits node and adds a new entry
             *
             * @key the key of the new entry
             * @n1 first split result (right node)
             * @n2 second split result (left node)
             * @child1 left child of the new entry
             * @child2 right child of the new entry, ignored for leaf nodes
             *
             * @return key of the median which should be pushed to the parent
             *
             * There are 2 variants implemented:
             * a) leaf nodes: the median is kept in one of the splitted nodes
             * b) internal nodes: the median is pushed out and should be added to the parent
             */
            K split(const K& key, Node* n1, Node* n2, std::size_t child1, std::size_t child2) {
                bool toCheck = true;
                std::size_t idx = 0;
                std::size_t childrenSourceIdx = 0;
                std::size_t childrenTargetIdx = 0;

                // first node
                for (std::size_t i = 0; i < nodeSize / 2 + (leaf ? 1 : 0); ++i) {
                    if (toCheck && (keys[idx] > key)) {
                        // add key and value
                        n1->keys[i] = key;

                        // replace original child by left and right
                        n1->children[childrenTargetIdx] = child1;
                        ++childrenTargetIdx;

                        if (!leaf) {
                            n1->children[childrenTargetIdx] = child2;
                            ++childrenTargetIdx;
                            ++childrenSourceIdx;
                        }

                        // deactivate check
                        toCheck = false;
                    } else {
                        // add key and value
                        n1->keys[i] = keys[idx];
                        ++idx;

                        // add child (could be right or left one)
                        n1->children[childrenTargetIdx] = children[childrenSourceIdx];
                        ++childrenTargetIdx;
                        ++childrenSourceIdx;
                    }
                }

                K median;
                if (toCheck && ((idx == nodeSize) || (keys[idx] > key))) {
                    median = key;

                    // consume it
                    if (!leaf) {
                        toCheck = false;

                        // append children
                        n1->children[childrenTargetIdx] = child1;
                        n2->children[0] = child2;
                        childrenTargetIdx = 1;
                        ++childrenSourceIdx;
                    } else {
                        childrenTargetIdx = 0;
                    }
                } else {
                    median = keys[idx];

                    // consume it
                    if (!leaf) {
                        ++idx;
                        // add last or first child?
                        // (this passes over the left/right delta)
                        if (toCheck) {
                            n1->children[childrenTargetIdx] = children[childrenSourceIdx];
                            ++childrenSourceIdx;
                            childrenTargetIdx = 0;
                        } else {
                            n2->children[0] = children[childrenSourceIdx];
                            ++childrenSourceIdx;
                            childrenTargetIdx = 1;
                        }
                    } else {
                        childrenTargetIdx = 0;
                    }
                }

                // second node
                for (std::size_t i = 0; i < nodeSize / 2; ++i) {
                    if (toCheck &&
                            ((idx == nodeSize) || (keys[idx] > key))) {
                        // add key and value
                        n2->keys[i] = key;

                        // replace original child by left and right
                        n2->children[childrenTargetIdx] = child1;
                        ++childrenTargetIdx;

                        if (!leaf) {
                            n2->children[childrenTargetIdx] = child2;
                            ++childrenTargetIdx;
                            ++childrenSourceIdx;
                        }

                        // deactivate check
                        toCheck = false;
                    } else {
                        // add key and value
                        n2->keys[i] = keys[idx];
                        ++idx;

                        // add child (could be right or left one)
                        n2->children[childrenTargetIdx] = children[childrenSourceIdx];
                        ++childrenTargetIdx;
                        ++childrenSourceIdx;
                    }
                }

                // set node attributes
                n1->filled = nodeSize / 2 + (leaf ? 1 : 0);
                n2->filled = nodeSize / 2;

                return median;
            }

            std::pair<std::size_t, const K*> findUpperAnchor(const K& key) const {
                std::size_t idx = filled;
                const K* k = nullptr;

                for (std::size_t i = 0; i < filled; ++i) {
                    if (keys[i] > key) {
                        idx = i;
                        k = &keys[i];
                        break;
                    }
                }

                return std::make_pair(children[idx], k);
            }

            std::pair<std::size_t, const K*> findLowerAnchor(const K& key) const {
                std::size_t idx = 0;
                const K* k = nullptr;

                for (std::size_t i = filled; i > 0; --i) {
                    if (keys[i - 1] <= key) {
                        idx = i;
                        k = &keys[i - 1];
                        break;
                    }
                }

                return std::make_pair(children[idx], k);
            }
        };

        provider_t provider;
        std::size_t* rootNode;
        std::size_t id;

        /* Dumps a node including children to a given ostream
         *
         * @id id of the node to dump
         * @os ostream which recieves dump
         *
         * The function is implemented recursive, so it's only supposed to work on a valid tree. The dumped data contains newlines (<std::endl>).
         */
        void dumpNode(std::size_t id, std::ostream& os) {
            Segment s = provider->getSegment(id);
            Node* n = static_cast<Node*>(s.ptr());

            os  << "--------" << std::endl
                << "id=" << id;


            if (n->leaf) {
                os << " (leaf)";
            } else {
                os << " (internal)";
            }

            os  << " filled="
                << n->filled;

            os  << " "
                << n->left
                << "<>"
                << n->right
                << std::endl;


            os << "K=";
            for (std::size_t i = 0; i < nodeSize; ++i) {
                if (i != 0) {
                    os << ",";
                }
                os << n->keys[i];
            }
            os << std::endl;

            os << "C=";
            for (std::size_t i = 0; i < nodeSize + 1; ++i) {
                if (i != 0) {
                    os << ",";
                }
                os << n->children[i];
            }
            os << std::endl;


            if (!n->leaf) {
                for (std::size_t i = 0; i < std::min(nodeSize, n->filled) + 1; ++i) {
                    if (n->children[i] != 0) {
                        dumpNode(n->children[i], os);
                    }
                }
            }
        }

        std::list<std::size_t> walkDown(const K& key) {
            std::size_t current = *rootNode;
            std::list<std::size_t> history;
            bool foundLeaf = false;

            while (!foundLeaf && (current != 0)) {
                Segment s = provider->getSegment(current);
                Node* n = static_cast<Node*>(s.ptr());
                history.push_front(current);
                current = std::get<0>(n->findUpperAnchor(key));
                foundLeaf = n->leaf;
            }

            return history;
        }

        std::pair<Segment, Node*> getParentNode(std::list<std::size_t>& history) {
            if (history.empty()) {
                // new root node
                Segment s = provider->createSegment(sizeof(Node));
                memset(s.ptr(), 0, sizeof(Node));
                Node* n = static_cast<Node*>(s.ptr());
                if (*rootNode == 0) {
                    n->leaf = true;
                }
                *rootNode = s.id();

                return std::make_pair(std::move(s), std::move(n));
            } else {
                std::size_t last = *history.begin();
                history.pop_front();
                Segment s = provider->getSegment(last);
                Node* n = static_cast<Node*>(s.ptr());

                return std::make_pair(std::move(s), std::move(n));
            }
        }
};


}

#endif

