/*
 * FreeRTOS Kernel V10.4.4
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "list.h"

/* Lint e9021, e961 and e750 are suppressed as a MISRA exception justified
 * because the MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be
 * defined for the header files above, but not in this file, in order to
 * generate the correct privileged Vs unprivileged linkage and placement. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750 !e9021. */

/*-----------------------------------------------------------
* PUBLIC LIST API documented in list.h
*----------------------------------------------------------*/

/**
 * @brief		列表(链表)根节点初始化函数
 * @details	    新创建或者定义的列表需要对其做初始化处理，列表的初始化其实就是初始化列表结构体
 * 				List_t 中的各个成员变量，列表的初始化通过使函数 vListInitialise()来完成，
 *
 * @param[in]	pxList  List_t结构体句柄，初始化时请定义该句柄，并用其地址来传参。
 *                      List_t structure handle. When initializing, 
 *                      please define the handle and use its address
 *                      to pass parameters.
 *
 * @return      无.
**/

void vListInitialise( List_t * const pxList )
{
	/* 将链表索引指针指向最后一个节点，即第一个节点*/
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	/* 将链表最后一个节点(也可以理解为第一个节点)的辅助排序的值设置为最大，确保该节点就是链表的最后节点 */
    pxList->xListEnd.xItemValue = portMAX_DELAY;

	/* 将最后一个节点(也可以理解为第一个节点)的 pxNext 和 pxPrevious 指针均指向节点自身，表示链表为空 */
    pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );     /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
    pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	/* 初始化链表节点计数器的值为 0，表示链表为空 */
    pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

    /* Write known values into the list if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList );
    listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList );
}
/*-----------------------------------------------------------*/
/**
 * @brief		列表项（节点）初始化函数。
 * @details	    列表项（节点）的初始化很简单，只是将列表项成员变量 pvContainer 初始化为 NULL，并且给用于完整性检查的变量赋值。
 * @param[in]	pxItem  ListItem_t结构体句柄，初始化时请定义该句柄，并用其地址来传参。
 *                      ListItem_t structure handle. When initializing, 
 *                      please define the handle and use its address
 *                      to pass parameters.
 * @return      无.
**/
void vListInitialiseItem( ListItem_t * const pxItem )
{
    /* 初始化该节点所在的链表为空，表示节点还没有插入任何链表. */
    pxItem->pxContainer = NULL;

    /* Write known values into the list item if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
}
/*-----------------------------------------------------------*/
/**
 * @brief		列表项(节点)末尾插入函数，将节点插入到链表的尾部  
 * @details	    将节点插入到链表的尾部（可以理解为头部）就是将一个新的节点插入到一个空的链表
 *
 * @param[in]	pxList        列表项要插入的列表（节点要插入的链表）
 *              pxNewListItem 要插入的列表项 （要插入的节点）
 *
 * @return      无.
**/
void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
{
    ListItem_t * const pxIndex = pxList->pxIndex;

    /* Only effective when configASSERT() is also defined, these tests may catch
     * the list data structures being overwritten in memory.  They will not catch
     * data errors caused by incorrect configuration or use of FreeRTOS. */
    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

    /* Insert a new list item into pxList, but rather than sort the list,
     * makes the new list item the last item to be removed by a call to
     * listGET_OWNER_OF_NEXT_ENTRY(). */
    pxNewListItem->pxNext = pxIndex;
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;

    /* Only used during decision coverage testing. */
    mtCOVERAGE_TEST_DELAY();

    pxIndex->pxPrevious->pxNext = pxNewListItem;
    pxIndex->pxPrevious = pxNewListItem;

    /* 记住该节点所在的链表. */
    pxNewListItem->pxContainer = pxList;

    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/**
 * @brief		列表项（节点）插入函数。将节点按照升序排列插入到链表
 * @details	    将节点按照升序排列插入到链表，如果有两个节点的值相同，则新节点在旧节点的后面插入
 *
 * @param[in]	pxList        列表项要插入的列表（节点要插入的链表）
 *              pxNewListItem 要插入的列表项 （要插入的节点）
 *
 * @return      无.
**/
void vListInsert( List_t * const pxList,
                  ListItem_t * const pxNewListItem )
{
    ListItem_t * pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

	/*寻找节点要插入的位置*/
    if( xValueOfInsertion == portMAX_DELAY )
    {
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext ) /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. *//*lint !e440 The iterator moves to a different value, not xValueOfInsertion. */
        {
            /* 没有事情可做，不断迭代只为了找到节点要插入的位置 */ 
        }
    }
	/* 根据升序排列，将节点插入 */ 
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    /* 记住该节点所在的链表 */
    pxNewListItem->pxContainer = pxList;
	
	/* 链表节点计数器++ */
    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/**
 * @brief		列表项(节点)删除函数。
 * @details	    将节点从链表删除.
 *
 * @param[in]	pxItemToRemove  要删除的列表项(节点)
 *              pxNewListItem   要插入的列表项(节点)
 *
 * @return      UBaseType_t     返回删除列表项(节点)以后的列表剩余列表项(节点)数目.
**/
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* 获取节点所在的链表 */
    List_t * const pxList = pxItemToRemove->pxContainer;
	/* 将指定的节点从链表删除*/
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    
    mtCOVERAGE_TEST_DELAY();

    /*调整链表的节点索引指针 */
    if( pxList->pxIndex == pxItemToRemove )
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
	/* 初始化该节点所在的链表为空，表示节点还没有插入任何链表 */
    pxItemToRemove->pxContainer = NULL;
	/* 链表节点计数器-- */
    ( pxList->uxNumberOfItems )--;
	/* 返回链表中剩余节点的个数 */
    return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/
