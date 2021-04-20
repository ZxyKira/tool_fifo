/* *****************************************************************************************
 *      File Name    :tool_fifo.h
 *      Create Date  :2018-09-09
 *      Modufy Date  :2021-04-20
 *      Information :
 */
#ifndef tool_fifo_H_
#define tool_fifo_H_

#include <stdint.h>
#include <stdbool.h> 

#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* *****************************************************************************************
 *    Version
 */ 
#define TOOL_FIFO_VERSION VERSION_DEFINEE(1, 0, 0)

/* *****************************************************************************************
 *    Macro
 */ 
#define TOOL_FIFO_VHEAD(rb)              (*(volatile uint32_t *) &(rb)->head)
#define TOOL_FIFO_VTAIL(rb)              (*(volatile uint32_t *) &(rb)->tail)

/* *****************************************************************************************
 *    Struct - tool_fifo_t
 */ 
typedef struct _tool_fifo_t{
  void *data;
  uint32_t count;
  uint32_t itemSz;
  uint32_t head;
  uint32_t tail;
}tool_fifo_t;

/* *****************************************************************************************
 *    Struct - tool_fifo_api_t
 */ 
typedef struct _tool_fifo_api_t{
  void     (*flush)       (tool_fifo_t* _this);
  uint32_t (*getSize)     (tool_fifo_t* _this);
  uint32_t (*getCount)    (tool_fifo_t* _this);
  uint32_t (*getFree)     (tool_fifo_t* _this);
  bool     (*isFull)      (tool_fifo_t* _this);
  bool     (*isEmpty)     (tool_fifo_t* _this);
  bool     (*init)        (tool_fifo_t* _this, void* buffer, int itemSize, int count);
  bool     (*insert)      (tool_fifo_t* _this, const void* data);
  int      (*insertMulti) (tool_fifo_t* _this, const void* data, int num);
  bool     (*insertTail)  (tool_fifo_t* _this, const void* data);
  bool     (*pop)         (tool_fifo_t* _this, void* buffer);
  int      (*popMulti)    (tool_fifo_t* _this, void* data, int num);
  bool     (*popHead)     (tool_fifo_t* _this, void* buffer);
}tool_fifo_api_t;

/* *****************************************************************************************
 *    static inline Method
 */ 

/*-------------------------------------
 *  tool_fifo_t* _this_flush 
 */ 
static inline void tool_fifo_flush(tool_fifo_t* _this){  
  _this->head = _this->tail = 0;
}



/*-------------------------------------
 *  tool_fifo_t* _this_getSize 
 */ 
static inline uint32_t tool_fifo_getSize(tool_fifo_t* _this){
  return _this->count;
}



/*-------------------------------------
 *  tool_fifo_t* _this_getCount 
 */ 
static inline uint32_t tool_fifo_getCount(tool_fifo_t* _this){
  return TOOL_FIFO_VHEAD(_this) - TOOL_FIFO_VTAIL(_this);
}



/*-------------------------------------
 *  tool_fifo_t* _this_getFree 
 */ 
static inline uint32_t tool_fifo_getFree(tool_fifo_t* _this){
  return _this->count - tool_fifo_getCount(_this);
}



/*-------------------------------------
 *  tool_fifo_t* _this_isFull 
 */ 
static inline bool tool_fifo_isFull(tool_fifo_t* _this){
  return (tool_fifo_getCount(_this) >= _this->count);
}



/*-------------------------------------
 *  tool_fifo_t* _this_isEmpty
 */ 
static inline bool tool_fifo_isEmpty(tool_fifo_t* _this){
  return TOOL_FIFO_VHEAD(_this) == TOOL_FIFO_VTAIL(_this);
}



/* *****************************************************************************************
 *    Link API
 */ 
extern const tool_fifo_api_t tool_fifo_api;



/* *****************************************************************************************
 *    Method list
 */ 
bool tool_fifo_init(tool_fifo_t* _this, void* buffer, int itemSize, int count);
bool tool_fifo_insert(tool_fifo_t* _this, const void* data);
int tool_fifo_insertMulti(tool_fifo_t* _this, const void* data, int num);
bool tool_fifo_insertTail(tool_fifo_t* _this, const void* data);
bool tool_fifo_pop(tool_fifo_t* _this, void* buffer);
int tool_fifo_popMulti(tool_fifo_t* _this, void* data, int num);
bool tool_fifo_popHead(tool_fifo_t* _this, void* buffer);



#ifdef __cplusplus
}
#endif //__cplusplus
#endif //tool_fifo_H_
/* *****************************************************************************************
 *  End of file
 */
