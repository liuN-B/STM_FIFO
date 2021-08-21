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
 * @brief		�б�(����)���ڵ��ʼ������
 * @details	    �´������߶�����б���Ҫ��������ʼ�������б�ĳ�ʼ����ʵ���ǳ�ʼ���б�ṹ��
 * 				List_t �еĸ�����Ա�������б�ĳ�ʼ��ͨ��ʹ���� vListInitialise()����ɣ�
 *
 * @param[in]	pxList  List_t�ṹ��������ʼ��ʱ�붨��þ�����������ַ�����Ρ�
 *                      List_t structure handle. When initializing, 
 *                      please define the handle and use its address
 *                      to pass parameters.
 *
 * @return      ��.
**/

void vListInitialise( List_t * const pxList )
{
	/* ����������ָ��ָ�����һ���ڵ㣬����һ���ڵ�*/
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	/* ���������һ���ڵ�(Ҳ�������Ϊ��һ���ڵ�)�ĸ��������ֵ����Ϊ���ȷ���ýڵ������������ڵ� */
    pxList->xListEnd.xItemValue = portMAX_DELAY;

	/* �����һ���ڵ�(Ҳ�������Ϊ��һ���ڵ�)�� pxNext �� pxPrevious ָ���ָ��ڵ�������ʾ����Ϊ�� */
    pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );     /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
    pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	/* ��ʼ������ڵ��������ֵΪ 0����ʾ����Ϊ�� */
    pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

    /* Write known values into the list if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList );
    listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList );
}
/*-----------------------------------------------------------*/
/**
 * @brief		�б���ڵ㣩��ʼ��������
 * @details	    �б���ڵ㣩�ĳ�ʼ���ܼ򵥣�ֻ�ǽ��б����Ա���� pvContainer ��ʼ��Ϊ NULL�����Ҹ����������Լ��ı�����ֵ��
 * @param[in]	pxItem  ListItem_t�ṹ��������ʼ��ʱ�붨��þ�����������ַ�����Ρ�
 *                      ListItem_t structure handle. When initializing, 
 *                      please define the handle and use its address
 *                      to pass parameters.
 * @return      ��.
**/
void vListInitialiseItem( ListItem_t * const pxItem )
{
    /* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ�����. */
    pxItem->pxContainer = NULL;

    /* Write known values into the list item if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
}
/*-----------------------------------------------------------*/
/**
 * @brief		�б���(�ڵ�)ĩβ���뺯�������ڵ���뵽�����β��  
 * @details	    ���ڵ���뵽�����β�����������Ϊͷ�������ǽ�һ���µĽڵ���뵽һ���յ�����
 *
 * @param[in]	pxList        �б���Ҫ������б��ڵ�Ҫ���������
 *              pxNewListItem Ҫ������б��� ��Ҫ����Ľڵ㣩
 *
 * @return      ��.
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

    /* ��ס�ýڵ����ڵ�����. */
    pxNewListItem->pxContainer = pxList;

    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/**
 * @brief		�б���ڵ㣩���뺯�������ڵ㰴���������в��뵽����
 * @details	    ���ڵ㰴���������в��뵽��������������ڵ��ֵ��ͬ�����½ڵ��ھɽڵ�ĺ������
 *
 * @param[in]	pxList        �б���Ҫ������б��ڵ�Ҫ���������
 *              pxNewListItem Ҫ������б��� ��Ҫ����Ľڵ㣩
 *
 * @return      ��.
**/
void vListInsert( List_t * const pxList,
                  ListItem_t * const pxNewListItem )
{
    ListItem_t * pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

	/*Ѱ�ҽڵ�Ҫ�����λ��*/
    if( xValueOfInsertion == portMAX_DELAY )
    {
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext ) /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. *//*lint !e440 The iterator moves to a different value, not xValueOfInsertion. */
        {
            /* û��������������ϵ���ֻΪ���ҵ��ڵ�Ҫ�����λ�� */ 
        }
    }
	/* �����������У����ڵ���� */ 
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    /* ��ס�ýڵ����ڵ����� */
    pxNewListItem->pxContainer = pxList;
	
	/* ����ڵ������++ */
    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/**
 * @brief		�б���(�ڵ�)ɾ��������
 * @details	    ���ڵ������ɾ��.
 *
 * @param[in]	pxItemToRemove  Ҫɾ�����б���(�ڵ�)
 *              pxNewListItem   Ҫ������б���(�ڵ�)
 *
 * @return      UBaseType_t     ����ɾ���б���(�ڵ�)�Ժ���б�ʣ���б���(�ڵ�)��Ŀ.
**/
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* ��ȡ�ڵ����ڵ����� */
    List_t * const pxList = pxItemToRemove->pxContainer;
	/* ��ָ���Ľڵ������ɾ��*/
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    
    mtCOVERAGE_TEST_DELAY();

    /*��������Ľڵ�����ָ�� */
    if( pxList->pxIndex == pxItemToRemove )
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
	/* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
    pxItemToRemove->pxContainer = NULL;
	/* ����ڵ������-- */
    ( pxList->uxNumberOfItems )--;
	/* ����������ʣ��ڵ�ĸ��� */
    return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/
