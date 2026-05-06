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

#define ASSERT_NULL(value) ASSERT_FALSE(value)

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
    ASSERT_TRUE(verify_tail_head(&test_list, &test_node_1.node, &test_node_1.node, true));

    /* Find the node 1, previous node should be null */
    sys_snode_t *test_node_1_prev = &test_node_1.node;
    ASSERT_TRUE(sys_slist_find(&test_list, &test_node_1.node, &test_node_1_prev));
    ASSERT_NULL(test_node_1_prev);

    /* Finding and removing node 1 */
    sys_slist_find_and_remove(&test_list, &test_node_1.node);
    ASSERT_TRUE(verify_emptiness(&test_list));
}
