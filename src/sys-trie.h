/*
  Copyright (c) 2012-2013 DataLab, S.L. <http://www.datalab.es>

  This file is part of the gfsys library for GlusterFS.

  The gfsys library for GlusterFS is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The gfsys library for GlusterFS is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the gfsys library for GlusterFS. If not, see
  <http://www.gnu.org/licenses/>.
*/

#ifndef __SYS_TRIE_H__
#define __SYS_TRIE_H__

#include <inttypes.h>

/*
 * TRIE_DIMENSION determines the size of each trie node. Valid dimensions are
 * 1, 2, 3 and 4.
 *
 *     Dimension 1: 1 bit per element,  8 elements per byte,  2 childs per node
 *     Dimension 2: 2 bits per element, 4 elements per byte,  4 childs per node
 *     Dimension 3: 4 bits per element, 2 elements per byte, 16 childs per node
 *     Dimension 4: 8 bits per element, 1 element per byte, 256 childs per node
 */
#define TRIE_DIMENSION 2

// Number of bits to index an element.
#define TRIE_INDEX_BITS (4 - (TRIE_DIMENSION))
// Number of bits used for each element.
#define TRIE_ELEM_BITS (1 << ((TRIE_DIMENSION) - 1))
// Number of childs per node
#define TRIE_CHILDS (1 << (TRIE_ELEM_BITS))
// Number of elements per byte.
#define TRIE_ELEM_PER_BYTE (1 << (TRIE_INDEX_BITS))
// Bitmask for an index.
#define TRIE_INDEX_MASK ((TRIE_ELEM_PER_BYTE) - 1)
// Bitmask for an element.
#define TRIE_ELEM_MASK (TRIE_CHILDS - 1)

typedef struct _sys_trie
{
    struct _sys_trie * childs[TRIE_CHILDS];
    struct _sys_trie * parent;
    void *             data;   // User data
    int32_t            count;  // Number of childs defined
    int32_t            length; // Length of the key
    uint8_t *          key;
} sys_trie_t;

#define sys_trie_for_each(_trie, _node) \
    for ((_node) = sys_trie_first(_trie); \
         (_node) != NULL; \
         (_node) = sys_trie_next(_node))

#define sys_trie_for_each_safe(_trie, _node, _tmp) \
    for ((_node) = sys_trie_first(_trie), (_tmp) = sys_trie_next(_node); \
         (_node) != NULL; \
         (_node) = (_tmp), (_tmp) = sys_trie_next(_node))

void sys_trie_init(sys_trie_t * trie);

void sys_trie_node_init(sys_trie_t * node, void * data);
int32_t sys_trie_node_alloc(void * data, sys_trie_t ** node);
void sys_trie_node_free(sys_trie_t * node);

/**
 * Lookup an entry inside the trie.
 */
int32_t sys_trie_lookup(sys_trie_t * trie, const void * key, size_t length,
                        sys_trie_t ** node);

/**
 * Insert a new entry into the trie.
 *
 * Tries to insert a new entry into the trie. If it was already present, 'old'
 * is set to point to it and EEXIST is returned. Otherwise, if 'node' points to
 * an entry, it is added to the trie, 'old' is set to NULL and 0 is returned.
 * If 'node' points to NULL, a new entry is created and assigned to 'node',
 * 'old' is set to NULL and 0 is returned.
 *
 * The caller will be still responsible of destroying the new entry by calling
 * sys_trie_node_free().
 */
int32_t sys_trie_insert(sys_trie_t * trie, const void * key, size_t length,
                        sys_trie_t ** node, sys_trie_t ** old);

/**
 * Replace an entry from the trie.
 *
 * This function behaves exactly as sys_trie_insert() with the only difference
 * that if the entry was already present, it is replaced by the new one.
 */
int32_t sys_trie_replace(sys_trie_t * trie, const void * key, size_t length,
                         sys_trie_t ** node, sys_trie_t ** old);

/**
 * Deletes an entry from the trie.
 *
 * The specified entry is removed from the trie. If it does not exist, ENOENT
 * is returned and 'node' is set to NULL. Otherwise 'node' will point to the
 * removed entry.
 *
 * Removed entries have key = NULL and length = 0.
 */
int32_t sys_trie_delete(sys_trie_t * trie, const void * key, size_t length,
                        sys_trie_t ** node);

/**
 * Swaps an entry by another in the trie.
 *
 * Removes the 'current' entry and replaces it by 'new'. 'new' will inherit the
 * key of 'current'.
 *
 * If 'current' does not belong to a trie, nothing is done and ENOENT is
 * returned.
 */
int32_t sys_trie_swap(sys_trie_t * current, sys_trie_t * new);

/**
 * Removes the specified entry from the trie.
 *
 * If the 'node' does not belong to a trie, ENOENT is returned.
 */
int32_t sys_trie_remove(sys_trie_t * node);

/**
 * Serialize a trie.
 */
int32_t sys_trie_store(sys_trie_t * trie, void * buffer, size_t * size,
                       int32_t (* store)(sys_trie_t *, void *, size_t *));

/**
 * Unserialize a trie.
 */
int32_t sys_trie_load(sys_trie_t ** trie, void * buffer, size_t size,
                      int32_t (* load)(sys_trie_t *, void *, size_t));

/**
 * Field access functions.
 */
void ** sys_trie_data(sys_trie_t * trie);
const void * sys_trie_key(sys_trie_t * trie);
size_t sys_trie_key_length(sys_trie_t * trie);

/**
 * Navigation functions.
 */
sys_trie_t * sys_trie_first(sys_trie_t * trie);
sys_trie_t * sys_trie_next(sys_trie_t * trie);

static inline err_t sys_trie_lookup_str(sys_trie_t * trie, const char * key,
                                        sys_trie_t ** node)
{
    return sys_trie_lookup(trie, key, strlen(key) + 1, node);
}

static inline err_t sys_trie_insert_str(sys_trie_t * trie, const char * key,
                                        sys_trie_t ** node, sys_trie_t ** old)
{
    return sys_trie_insert(trie, key, strlen(key) + 1, node, old);
}

static inline err_t sys_trie_replace_str(sys_trie_t * trie, const char * key,
                                         sys_trie_t ** node, sys_trie_t ** old)
{
    return sys_trie_replace(trie, key, strlen(key) + 1, node, old);
}

static inline err_t sys_trie_delete_str(sys_trie_t * trie, const char * key,
                                        sys_trie_t ** node)
{
    return sys_trie_delete(trie, key, strlen(key) + 1, node);
}

#endif /* __SYS_TRIE_H__ */
