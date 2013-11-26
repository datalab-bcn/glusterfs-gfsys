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

#ifndef __SYS_HEAP_H__
#define __SYS_HEAP_H__

typedef struct _sys_heap_item
{
    size_t index;
} sys_heap_item_t;

typedef struct _sys_heap_node
{
    uintptr_t         key;
    sys_heap_item_t * item;
} sys_heap_node_t;

typedef struct _sys_heap
{
    size_t            min;
    size_t            max;
    size_t            size;
    sys_heap_node_t * nodes;
} sys_heap_t;

#define __sys_heap_siftup(_hp, _idx, _parent, _heap, _item, _index, _key, \
                          _key1, _key2, _cmp) \
    ({ \
        typeof(_key) _key1, _key2 = (_key); \
        sys_heap_t * _hp = (_heap); \
        size_t _parent, _idx = (_index); \
        while (_idx > 0) \
        { \
            _parent = (_idx - 1) >> 1; \
            _key1 = (typeof(_key1))_hp->nodes[_parent].key; \
            if ((_cmp) <= 0) \
            { \
                break; \
            } \
            _hp->nodes[_idx].key = _key1; \
            _hp->nodes[_idx].item = _hp->nodes[_parent].item; \
            _hp->nodes[_idx].item->index = _idx; \
            _idx = _parent; \
        } \
        _hp->nodes[_idx].key = (uintptr_t)_key2; \
        _hp->nodes[_idx].item = (_item); \
        _hp->nodes[_idx].item->index = _idx; \
        _idx; \
    })
#define _sys_heap_siftup(_args...) __sys_heap_siftup(_args)
#define sys_heap_siftup(_heap, _item, _index, _key, _key1, _key2, _cmp) \
    _sys_heap_siftup(SYS_TMP(3), _heap, _item, _index, _key, _key1, _key2, \
                     _cmp)

#define __sys_heap_siftdown(_hp, _sz, _idx, _ch1, _ch2, _key3, _heap, _item, \
                            _index, _key, _key1, _key2, _cmp) \
    ({ \
        typeof(_key) _key1, _key2, _key3 = (_key); \
        sys_heap_t * _hp = (_heap); \
        size_t _sz = _hp->size, _idx = (_index); \
        size_t _ch2, _ch1 = (_idx << 1) + 1; \
        while (_ch1 < _sz) \
        { \
            _key1 = (typeof(_key1))_hp->nodes[_ch1].key; \
            _ch2 = _ch1 + 1; \
            if (_ch2 < _sz) \
            { \
                _key2 = (typeof(_key2))_hp->nodes[_ch2].key; \
                if ((_cmp) > 0) \
                { \
                    _ch1 = _ch2; \
                    _key1 = _key2; \
                } \
            } \
            _key2 = _key3; \
            if ((_cmp) >= 0) \
            { \
                break; \
            } \
            _hp->nodes[_idx].key = _key1; \
            _hp->nodes[_idx].item = _hp->nodes[_ch1].item; \
            _hp->nodes[_idx].item->index = _idx; \
            _idx = _ch1; \
            _ch1 = (_ch1 << 1) + 1; \
        } \
        _hp->nodes[_idx].key = _key3; \
        _hp->nodes[_idx].item = (_item); \
        _hp->nodes[_idx].item->index = _idx; \
        _idx; \
    })
#define _sys_heap_siftdown(_args...) __sys_heap_siftdown(_args)
#define sys_heap_siftdown(_heap, _item, _index, _key, _key1, _key2, _cmp) \
    _sys_heap_siftdown(SYS_TMP(6), _heap, _item, _index, _key, _key1, _key2, \
                       _cmp)

#define __sys_heap_insert(_hp, _idx, _heap, _item, _key, _key1, _key2, _cmp) \
    ({ \
        sys_heap_t * _hp = (_heap); \
        size_t _idx = _hp->size++; \
        if (unlikely(_idx >= _hp->max)) \
        { \
            _hp->max <<= 1; \
            SYS_CRESIZE( \
                &_hp->nodes, _hp->max, sys_mt_sys_heap_item_t, \
                E(), \
                LOG(W(), "Heap resize to %lu items failed", _hp->max), \
                NO_FAIL() \
            ); \
        } \
        sys_heap_siftup(_hp, _item, _idx, _key, _key1, _key2, _cmp); \
    })
#define _sys_heap_insert(_args...) __sys_heap_insert(_args)
#define sys_heap_insert(_heap, _item, _key, _key1, _key2, _cmp) \
    _sys_heap_insert(SYS_TMP(2), _heap, _item, _key, _key1, _key2, _cmp)

#define __sys_heap_peek(_hp, _pkey, _item, _heap, _key) \
    ({ \
        sys_heap_item_t * _item = NULL; \
        sys_heap_t * _hp = (_heap); \
        typeof(_key) _pkey = (_key); \
        if (_hp->size > 0) \
        { \
            *_pkey = (typeof(*_pkey))_hp->nodes[0].key; \
            _item = _hp->nodes[0].item; \
        } \
        _item; \
    })
#define _sys_heap_peek(_args...) __sys_heap_peek(_args)
#define sys_heap_peek(_heap, _key) _sys_heap_peek(SYS_TMP(3), _heap, _key)

#define __sys_heap_get(_hp, _sz, _pkey, _item, _heap, _key, _key1, _key2, \
                       _cmp) \
    ({ \
        sys_heap_item_t * _item = NULL; \
        sys_heap_t * _hp = (_heap); \
        typeof(_key) _pkey  = (_key); \
        size_t _sz = _hp->size; \
        if (_sz > 0) \
        { \
            *_pkey = (typeof(*_pkey))_hp->nodes[0].key; \
            _item = _hp->nodes[0].item; \
            _hp->size = --_sz; \
            sys_heap_siftdown(_hp, _hp->nodes[_sz].item, 0, \
                              (typeof(*_pkey))_hp->nodes[_sz].key, \
                              _key1, _key2, _cmp); \
            if (unlikely((_sz > _hp->min) && (_sz <= (_hp->max >> 2)))) \
            { \
                _hp->max >>= 1; \
                SYS_CRESIZE( \
                    &_hp->nodes, _hp->max, sys_mt_sys_heap_item_t, \
                    E(), \
                    NO_FAIL() \
                ); \
            } \
        } \
        _item; \
    })
#define _sys_heap_get(_args...) __sys_heap_get(_args)
#define sys_heap_get(_heap, _key, _key1, _key2, _cmp) \
    _sys_heap_get(SYS_TMP(4), _heap, _key, _key1, _key2, _cmp)

#define __sys_heap_delete(_hp, _sz, _pkey, _key3, _item, _heap, _index, _key, \
                          _key1, _key2, _cmp) \
    ({ \
        sys_heap_item_t * _item = NULL; \
        sys_heap_t * _hp = (_heap); \
        typeof(_key) _pkey  = (_key); \
        typeof(*(_key)) _key1, _key2, _key3; \
        size_t _sz = _hp->size, _idx = (_index); \
        if (_sz > _idx) \
        { \
            _sz--; \
            _item = _hp->nodes[_sz].item; \
            _key3 = _key2 = (typeof(_key3))_hp->nodes[_sz].key; \
            *_pkey = _key3; \
            _key1 = (typeof(_key1))_hp->nodes[_idx].key; \
            if ((_cmp) >= 0) \
            { \
                sys_heap_siftup(_hp, _item, _idx, _key3, _key1, _key2, _cmp); \
            } \
            else \
            { \
                sys_heap_siftdown(_hp, _item, _idx, _key3, _key1, _key2, \
                                  _cmp); \
            } \
            _hp->size = _sz; \
            if (unlikely((_sz > _hp->min) && (_sz < (_hp->max >> 2)))) \
            { \
                _hp->max >>= 1; \
                SYS_CRESIZE( \
                    &_hp->nodes, _hp->max, sys_mt_sys_heap_item_t, \
                    E(), \
                    NO_FAIL() \
                ); \
            } \
        } \
        _item; \
    })
#define _sys_heap_delete(_args...) __sys_heap_delete(_args)
#define sys_heap_delete(_heap, _index, _key, _key1, _key2, _cmp) \
    _sys_heap_delete(SYS_TMP(5), _heap, _index, _key, _key1, _key2, _cmp)

static inline err_t sys_heap_initialize(sys_heap_t * heap, size_t min)
{
    SYS_CALLOC(
        &heap->nodes, min, sys_mt_sys_heap_item_t,
        E(),
        RETERR()
    );

    heap->min = min;
    heap->max = min;
    heap->size = 0;

    return 0;
}

static inline void sys_heap_terminate(sys_heap_t * heap)
{
    SYS_FREE(heap->nodes);
}

static inline err_t sys_heap_allocate(sys_heap_t ** heap, size_t min)
{
    err_t error;

    SYS_MALLOC(
        heap, sys_mt_sys_heap_t,
        E(),
        RETERR()
    );
    SYS_CALL(
        sys_heap_initialize, (*heap, min),
        E(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    SYS_FREE(*heap);

    return error;
}

static inline void sys_heap_free(sys_heap_t * heap)
{
    sys_heap_terminate(heap);
    SYS_FREE(heap);
}

#endif /* __SYS_HEAP_H__ */
