/* *****************************************************************************************
 *      File Name    :tool_fifo.c
 *      Create Date  :2018-09-09
 *      Modufy Date  :2021-04-20
 *      Information :
 */
#include "tool_fifo.h"
#include "string.h" 

/* *****************************************************************************************
 *  Macro Function 
 */
#define RB_INDH(rb)                ((rb)->head & ((rb)->count - 1))
#define RB_INDT(rb)                ((rb)->tail & ((rb)->count - 1))

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* *****************************************************************************************
 *  Private Function 
 */

/* *****************************************************************************************
 *  Public Function 
 */
  
/*----------------------------------------
 *  tool_fifo_init
 *----------------------------------------*/ 
bool tool_fifo_init(tool_fifo_t* _this, void *buffer, int itemSize, int count){
  _this->data = buffer;
  _this->count = count;
  _this->itemSz = itemSize;
  _this->head = _this->tail = 0;

  return true;
}



/*----------------------------------------
 *  tool_fifo_insert
 *----------------------------------------*/ 
bool tool_fifo_insert(tool_fifo_t* _this, const void* data){
  uint8_t *ptr = _this->data;

  /* We cannot insert when queue is full */
  if (tool_fifo_isFull(_this))
    return false;

  ptr += RB_INDH(_this) * _this->itemSz;
  memcpy(ptr, data, _this->itemSz);
  _this->head++;

  return true;
}



/*----------------------------------------
 *  tool_fifo_insertMulti
 *----------------------------------------*/ 
int tool_fifo_InsertMult(tool_fifo_t* _this, const void* data, int num){
  uint8_t *ptr = _this->data;
  int cnt1, cnt2;

  /* We cannot insert when queue is full */
  if (tool_fifo_isFull(_this))
    return 0;

  /* Calculate the segment lengths */
  cnt1 = cnt2 = tool_fifo_getFree(_this);
  if (RB_INDH(_this) + cnt1 >= _this->count)
    cnt1 = _this->count - RB_INDH(_this);
  cnt2 -= cnt1;

  cnt1 = MIN(cnt1, num);
  num -= cnt1;

  cnt2 = MIN(cnt2, num);
  num -= cnt2;

  /* Write segment 1 */
  ptr += RB_INDH(_this) * _this->itemSz;
  memcpy(ptr, data, cnt1 * _this->itemSz);
  _this->head += cnt1;

  /* Write segment 2 */
  ptr = (uint8_t *) _this->data + RB_INDH(_this) * _this->itemSz;
  data = (const uint8_t *) data + cnt1 * _this->itemSz;
  memcpy(ptr, data, cnt2 * _this->itemSz);
  _this->head += cnt2;

  return cnt1 + cnt2;
}



/*----------------------------------------
 *  tool_fifo_insertTail
 *----------------------------------------*/ 
bool tool_fifo_insertTail(tool_fifo_t* _this, const void* data){
  uint8_t *ptr = _this->data;

  /* We cannot insert when queue is full */
  if (tool_fifo_isFull(_this))
    return false;

  ptr += (RB_INDT(_this)-1) * _this->itemSz;
  memcpy(ptr, data, _this->itemSz);
  _this->tail--;

  return true;
}



/*----------------------------------------
 *  tool_fifo_pop
 *----------------------------------------*/ 
bool tool_fifo_Pop(tool_fifo_t* _this, void *data){
  uint8_t *ptr = _this->data;

  /* We cannot pop when queue is empty */
  if (tool_fifo_isEmpty(_this))
    return false;

  ptr += RB_INDT(_this) * _this->itemSz;
  memcpy(data, ptr, _this->itemSz);
  _this->tail++;

  return true;
}



/*----------------------------------------
 *  tool_fifo_popMulti
 *----------------------------------------*/ 
int tool_fifo_PopMult(tool_fifo_t* _this, void *data, int num)
{
  uint8_t *ptr = _this->data;
  int cnt1, cnt2;

  /* We cannot insert when queue is empty */
  if (tool_fifo_isEmpty(_this))
    return 0;

  /* Calculate the segment lengths */
  cnt1 = cnt2 = tool_fifo_getCount(_this);
  if (RB_INDT(_this) + cnt1 >= _this->count)
    cnt1 = _this->count - RB_INDT(_this);
  cnt2 -= cnt1;

  cnt1 = MIN(cnt1, num);
  num -= cnt1;

  cnt2 = MIN(cnt2, num);
  num -= cnt2;

  /* Write segment 1 */
  ptr += RB_INDT(_this) * _this->itemSz;
  memcpy(data, ptr, cnt1 * _this->itemSz);
  _this->tail += cnt1;

  /* Write segment 2 */
  ptr = (uint8_t *) _this->data + RB_INDT(_this) * _this->itemSz;
  data = (uint8_t *) data + cnt1 * _this->itemSz;
  memcpy(data, ptr, cnt2 * _this->itemSz);
  _this->tail += cnt2;

  return cnt1 + cnt2;
}



/*----------------------------------------
 *  tool_fifo_popHead
 *----------------------------------------*/
bool tool_fifo_popHead(tool_fifo_t* _this, void *data){
  uint8_t *ptr = _this->data;

  /* We cannot pop when queue is empty */
  if (tool_fifo_isEmpty(_this))
    return false;

  ptr += (RB_INDH(_this)-1) * _this->itemSz;
  memcpy(data, ptr, _this->itemSz);
  _this->head--;

  return true;
}



/* *****************************************************************************************
 *  API Link
 */
const tool_fifo_api_t tool_fifo_api = {
  .flush         = tool_fifo_flush,
  .getSize       = tool_fifo_getSize,
  .getCount      = tool_fifo_getCount,
  .getFree       = tool_fifo_getFree,
  .isFull        = tool_fifo_isFull,
  .isEmpty       = tool_fifo_isEmpty,
  .init          = tool_fifo_init,
  .insert        = tool_fifo_insert,
  .insertMulti   = tool_fifo_insertMulti,
  .insertTail    = tool_fifo_insertTail,
  .pop           = tool_fifo_pop,
  .popMulti      = tool_fifo_popMulti,
  .popHead       = tool_fifo_popHead,
};

/* *****************************************************************************************
 *  End of file
 */

