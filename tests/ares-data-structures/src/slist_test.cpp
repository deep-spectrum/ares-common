/**
 * @file slist_test.cpp
 *
 * @brief
 *
 * @date 5/6/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/data-structures/sys/slist.h>
#include <ares/util.hpp>
#include <gtest/gtest.h>

#define ASSERT_NULL(value)     ASSERT_FALSE(value)
#define ASSERT_NOT_NULL(value) ASSERT_TRUE(value)

static sys_slist_t test_list;
static sys_slist_t append_list;

struct container_node {
    sys_snode_t node;
    int unused;
};

static container_node test_node_1;
static container_node test_node_2;
static container_node test_node_3;
static container_node test_node_4;

static inline bool verify_emptiness(sys_slist_t *list) {
    sys_snode_t *node;
    sys_snode_t *s_node;
    container_node *cnode;
    container_node *s_cnode;
    int count;

    if (!sys_slist_is_empty(list)) {
        return false;
    }

    if (sys_slist_peek_head(list)) {
        return false;
    }

    if (sys_slist_peek_tail(list)) {
        return false;
    }

    if (sys_slist_len(list) != 0) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_NODE(list, node) { count++; }

    if (count) {
        return false;
    }

    SYS_SLIST_FOR_EACH_NODE_SAFE(list, node, s_node) { count++; }

    if (count) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_CONTAINER(list, cnode, node) { count++; }

    if (count) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) { count++; }

    if (count) {
        return false;
    }

    return true;
}

static inline bool verify_content_amount(sys_slist_t *list, int amount) {
    sys_snode_t *node;
    sys_snode_t *s_node;
    container_node *cnode;
    container_node *s_cnode;
    int count;

    if (sys_slist_is_empty(list)) {
        return false;
    }

    if (!sys_slist_peek_head(list)) {
        return false;
    }

    if (!sys_slist_peek_tail(list)) {
        return false;
    }

    if (sys_slist_len(list) != amount) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_NODE(list, node) { count++; }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_NODE_SAFE(list, node, s_node) { count++; }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_CONTAINER(list, cnode, node) { count++; }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) { count++; }

    if (count != amount) {
        return false;
    }

    return true;
}

static inline bool verify_tail_head(sys_slist_t *list, sys_snode_t *head,
                                    sys_snode_t *tail, bool same) {
    if (sys_slist_peek_head(list) != head) {
        return false;
    }

    if (sys_slist_peek_tail(list) != tail) {
        return false;
    }

    if (same) {
        if (sys_slist_peek_head(list) != sys_slist_peek_tail(list)) {
            return false;
        }
    } else {
        if (sys_slist_peek_head(list) == sys_slist_peek_tail(list)) {
            return false;
        }
    }

    return true;
}

TEST(dlist_api, test_slist) {
    sys_slist_init(&test_list);
    ASSERT_TRUE(verify_emptiness(&test_list));

    /* Appending node 1 */
    sys_slist_append(&test_list, &test_node_1.node);
    ASSERT_TRUE(verify_content_amount(&test_list, 1));
    ASSERT_TRUE(verify_tail_head(&test_list, &test_node_1.node,
                                 &test_node_1.node, true));

    /* Find the node 1, previous node should be null */
    sys_snode_t *test_node_1_prev = &test_node_1.node;
    ASSERT_TRUE(
        sys_slist_find(&test_list, &test_node_1.node, &test_node_1_prev));
    ASSERT_NULL(test_node_1_prev);

    /* Finding and removing node 1 */
    sys_slist_find_and_remove(&test_list, &test_node_1.node);
    ASSERT_TRUE(verify_emptiness(&test_list));

    /* Prepending node 1 */
    sys_slist_prepend(&test_list, &test_node_1.node);
    ASSERT_TRUE(verify_content_amount(&test_list, 1));
    ASSERT_TRUE(verify_tail_head(&test_list, &test_node_1.node,
                                 &test_node_1.node, true));

    /* Removing node 1 */
    sys_slist_remove(&test_list, nullptr, &test_node_1.node);
    ASSERT_TRUE(verify_emptiness(&test_list));

    /* Appending node 1 */
    sys_slist_append(&test_list, &test_node_1.node);
    /* Prepending node 2 */
    sys_slist_prepend(&test_list, &test_node_2.node);

    ASSERT_TRUE(verify_content_amount(&test_list, 2));

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_1.node, false)));

    /* Appending node 3 */
    sys_slist_append(&test_list, &test_node_3.node);

    ASSERT_TRUE((verify_content_amount(&test_list, 3)));

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_3.node, false)));

    ASSERT_TRUE((sys_slist_peek_next(&test_node_2.node) == &test_node_1.node));

    /* Inserting node 4 after node 2, peek with nocheck variant */
    sys_slist_insert(&test_list, &test_node_2.node, &test_node_4.node);

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_3.node, false)));

    ASSERT_TRUE(
        (sys_slist_peek_next_no_check(&test_node_2.node) == &test_node_4.node));

    /* Find the node 4 and get the previous node*/
    sys_snode_t *test_node_4_prev = nullptr;

    ASSERT_TRUE(
        sys_slist_find(&test_list, &test_node_4.node, &test_node_4_prev));
    ASSERT_EQ(&test_node_2.node, test_node_4_prev);

    /* Finding and removing node 1 */
    sys_slist_find_and_remove(&test_list, &test_node_1.node);
    ASSERT_TRUE((verify_content_amount(&test_list, 3)));

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_3.node, false)));

    /* Removing node 3 */
    sys_slist_remove(&test_list, &test_node_4.node, &test_node_3.node);
    ASSERT_TRUE((verify_content_amount(&test_list, 2)));

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_4.node, false)));

    /* Removing node 4 */
    sys_slist_remove(&test_list, &test_node_2.node, &test_node_4.node);
    ASSERT_TRUE((verify_content_amount(&test_list, 1)));

    ASSERT_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                                  &test_node_2.node, true)));

    /* Removing node 2 */
    sys_slist_remove(&test_list, nullptr, &test_node_2.node);
    ASSERT_TRUE((verify_emptiness(&test_list)));

    /* test iterator from a node */
    struct data_node {
        sys_snode_t node;
        int data;
    } data_node[6] = {
        {.data = 0}, {.data = 1}, {.data = 2},
        {.data = 3}, {.data = 4}, {.data = 5},
    };
    sys_snode_t *node = nullptr;
    int ii;

    sys_slist_init(&test_list);

    for (ii = 0; ii < 6; ii++) {
        sys_slist_append(&test_list, &data_node[ii].node);
    }

    ii = 0;
    SYS_SLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 2) {
            break;
        }
    }
    ASSERT_EQ(ii, 3);

    ii = 0;
    SYS_SLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 3) {
            break;
        }
    }
    ASSERT_EQ(ii, 1);

    ii = 0;
    SYS_SLIST_ITERATE_FROM_NODE(&test_list, node) { ii++; }
    ASSERT_EQ(ii, 2);

    /* test sys_slist_remove and sys_slist_append inside safe node iteration */
    sys_snode_t *s_node, *prev, *removed;
    sys_snode_t append;

    removed = nullptr;
    SYS_SLIST_FOR_EACH_NODE_SAFE(&test_list, node, s_node) {
        /* Remove first node under iteration */
        if (removed == nullptr) {
            sys_slist_remove(&test_list, nullptr, node);
            removed = node;
        }
    }
    ASSERT_NOT_NULL(removed);
    ASSERT_TRUE((verify_content_amount(&test_list, 5)));
    sys_slist_prepend(&test_list, removed);

    removed = nullptr;
    SYS_SLIST_FOR_EACH_NODE_SAFE(&test_list, node, s_node) {
        /* Remove last node under iteration */
        if (node->next == nullptr) {
            sys_slist_remove(&test_list, prev, node);
            removed = node;
        }
        prev = node;
    }
    ASSERT_NOT_NULL(removed);
    ASSERT_TRUE((verify_content_amount(&test_list, 5)));
    sys_slist_append(&test_list, removed);

    SYS_SLIST_FOR_EACH_NODE_SAFE(&test_list, node, s_node) {
        /* Append on first iteration */
        if (test_list.head == node) {
            sys_slist_append(&test_list, &append);
        }
    }
    ASSERT_TRUE((verify_content_amount(&test_list, 7)));
    sys_slist_find_and_remove(&test_list, &append);

    SYS_SLIST_FOR_EACH_NODE_SAFE(&test_list, node, s_node) {
        /* Append on last iteration */
        if (node->next == nullptr) {
            sys_slist_append(&test_list, &append);
        }
    }
    ASSERT_TRUE((verify_content_amount(&test_list, 7)));
    sys_slist_find_and_remove(&test_list, &append);

    /* test sys_slist_remove and sys_slist_append inside safe container
     * iteration */
    struct container_node *cnode, *s_cnode, *cprev, *cremoved;
    struct container_node cappend;

    cremoved = nullptr;
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&test_list, cnode, s_cnode, node) {
        /* Remove on first iteration */
        if (cremoved == nullptr) {
            sys_slist_remove(&test_list, nullptr, &cnode->node);
            cremoved = cnode;
        }
    }
    ASSERT_NOT_NULL(cremoved);
    ASSERT_TRUE((verify_content_amount(&test_list, 5)));
    sys_slist_prepend(&test_list, &cremoved->node);

    cremoved = nullptr;
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&test_list, cnode, s_cnode, node) {
        /* Remove on last iteration */
        if (cnode->node.next == nullptr) {
            sys_slist_remove(&test_list, &cprev->node, &cnode->node);
            cremoved = cnode;
        }
        cprev = cnode;
    }
    ASSERT_NOT_NULL(cremoved);
    ASSERT_TRUE((verify_content_amount(&test_list, 5)));
    sys_slist_append(&test_list, &cremoved->node);

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&test_list, cnode, s_cnode, node) {
        /* Append on first iteration */
        if (test_list.head == &cnode->node) {
            sys_slist_append(&test_list, &cappend.node);
        }
    }
    ASSERT_TRUE((verify_content_amount(&test_list, 7)));
    sys_slist_find_and_remove(&test_list, &cappend.node);

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&test_list, cnode, s_cnode, node) {
        /* Append on last iteration */
        if (cnode->node.next == nullptr) {
            sys_slist_append(&test_list, &cappend.node);
        }
    }
    ASSERT_TRUE((verify_content_amount(&test_list, 7)));
    sys_slist_find_and_remove(&test_list, &cappend.node);

    /* test sys_slist_get_not_empty() and sys_slist_get() APIs */
    for (ii = 0; ii < 6; ii++) {
        node = sys_slist_get_not_empty(&test_list);
        ASSERT_EQ(((struct data_node *)node)->data, ii);
    }
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list since we just emptied it */
        sys_slist_append(&test_list, &data_node[ii].node);
    }
    for (ii = 0; ii < 6; ii++) {
        node = sys_slist_get(&test_list);
        ASSERT_EQ(((struct data_node *)node)->data, ii);
    }
    node = sys_slist_get(&test_list);
    ASSERT_EQ(node, nullptr);

    /* test sys_slist_append_list() */
    sys_slist_init(&append_list);
    struct data_node data_node_append[6] = {
        {.data = 6}, {.data = 7},  {.data = 8},
        {.data = 9}, {.data = 10}, {.data = 11},
    };
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list, which we just emptied */
        sys_slist_append(&test_list, &data_node[ii].node);
        /* Build append_list so that the node pointers are correct */
        sys_slist_append(&append_list, &data_node_append[ii].node);
    }
    sys_slist_append_list(&test_list, &data_node_append[0].node,
                          &data_node_append[5].node);
    for (ii = 0; ii < 12; ii++) {
        node = sys_slist_get(&test_list);
        ASSERT_EQ(((struct data_node *)node)->data, ii);
    }

    /* test sys_slist_append_list with empty list */
    sys_slist_init(&test_list);
    sys_slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list only */
        sys_slist_append(&test_list, &data_node[ii].node);
    }
    sys_slist_append_list(&test_list, append_list.head, append_list.tail);
    node = sys_slist_peek_tail(&test_list);
    ASSERT_EQ(((struct data_node *)node)->data, data_node[5].data);

    /* test sys_slist_merge_slist */
    sys_slist_init(&test_list);
    sys_slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate both lists */
        sys_slist_append(&test_list, &data_node[ii].node);
        sys_slist_append(&append_list, &data_node_append[ii].node);
    }
    sys_slist_merge_slist(&test_list, &append_list);
    for (ii = 0; ii < 12; ii++) {
        node = sys_slist_get(&test_list);
        ASSERT_EQ(((struct data_node *)node)->data, ii);
    }
    ASSERT_TRUE(sys_slist_is_empty(&append_list));

    /* test sys_slist_merge_slist with empty list */
    sys_slist_init(&test_list);
    sys_slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list only */
        sys_slist_append(&test_list, &data_node[ii].node);
    }

    sys_slist_merge_slist(&test_list, &append_list);
    node = sys_slist_peek_tail(&test_list);
    ASSERT_EQ(((struct data_node *)node)->data, data_node[5].data);
}
