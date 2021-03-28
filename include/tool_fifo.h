/* *****************************************************************************************
 *      File Name    :tool_fifo.h
 *      Create Date  :2018-09-09
 *      Modufy Date  :2021-03-28
 *      Information :
 */
#ifndef tool_fifo_H
#define tool_fifo_H

#include <stdint.h>
#include <stdbool.h> 

#ifdef __cplusplus
extern "C" {
#endif

#define fifo_status_inited         (1 << 0)
#define fifo_statis_isFull         (1 << 1)
#define fifo_status_isOverride     (1 << 2)
#define fifo_status_isMutexHead    (1 << 4)
#define fifo_status_isMutexTail    (1 << 5)

#define fifo_func_logRecode     (1 << 0)
#define fifo_func_override      (1 << 1)
#define fifo_func_mutex         (1 << 2)


/* *****************************************************************************************
 *    Struct - tool_fifo_t
 */ 
typedef struct {
  void *data;
  uint32_t count;
  uint32_t itemSz;
  uint32_t head;
  uint32_t tail;
  const char *name;
  struct{
    uint8_t status;
    uint8_t func;
    uint8_t reserved[2];
  }config;
}tool_fifo_t;

/* *****************************************************************************************
 *    Struct - tool_fifo_config_t
 */ 
typedef struct{
  void* address;
  void* buffer;
  uint32_t bufferSize;
  int count;
  int itemSize;
}tool_fifo_config_t;

/* *****************************************************************************************
 *    Struct - tool_fifo_api_t
 */ 
typedef struct _tool_fifo_api_t{
  void (*flush)(tool_fifo_t *fifo);
  /**
   * @brief  Clear FIFO queue.
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  None
   */
  uint32_t (*getSize)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO block size.
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  Size of single block .
   */
  uint32_t (*getCount)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO usable count.
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  Totla of usable block count.
   */
  uint32_t (*getFree)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO unused block count
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  Totla of unused block count.
   */
  uint32_t (*getUsed)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO used block count
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  Totla of used block count.
   */
  uint8_t (*getStatus)(tool_fifo_t *fifo);
  bool (*isFull)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO status.
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  0:not full; 1:is full.
   */
  bool (*isEmpty)(tool_fifo_t *fifo);
  /**
   * @brief  Get FIFO status.
   * @param  fifo  : FIFO Handler memory pointer.
   * @return  0:not empty; 1:is empty.
   */
  bool (*init)(tool_fifo_t *fifo,  const tool_fifo_config_t *config);
  /**
   * @brief  Init FIFO<Normal mod>.
   * @param  fifo  : FIFO Handler memory pointer.
   * @param  config: FIFO config pointer.
   * @return  0:Init fail; 1:Init successful.
   */  
  tool_fifo_t* (*initAddr)(const tool_fifo_config_t *config);
  /**
   * @brief  Init FIFO<Address mod>.
   * @param  config: FIFO config pointer.
   * @return  0:Init fail; 1:Init successful.
   */  
  tool_fifo_t* (*reload)(const tool_fifo_config_t *config);
  /**
   * @brief  reload FIFO<Address mod>.
   * @param  config: FIFO config pointer.
   * @return  0:Init fail; 1:Init successful.
   */  
  bool (*insert)(tool_fifo_t *fifo, const void *data);
  /**
   * @brief  Add new data to FIFO.
   * @param  fifo  : FIFO Handler memory pointer.
   * @param  data  : Data pointer.
   * @return  0:FIFO is full ; 1:Successful.
   */  
   
  int (*insertMulti)(tool_fifo_t *fifo, const void *data, uint32_t count);
  
  bool (*insertTail)(tool_fifo_t *fifo, const void *data);
  /**
   * @brief  Add new data to FIFO tail<priority high>.
   * @param  fifo  : FIFO Handler memory pointer.
   * @param  data  : Data pointer.
   * @return  0:FIFO is full ; 1:Successful.
   */  
  bool (*pop)(tool_fifo_t *fifo, void *buffer);
  /**
   * @brief  pop data form FIFO.
   * @param  fifo  : FIFO Handler memory pointer.
   * @param  data  : Data out buffer pointer.
   * @return  0:FIFO is empty ; 1:Successful.
   */  
   
  int (*popMulti)(tool_fifo_t *fifo, void *buffer, uint32_t count);
  
  bool (*popHead)(tool_fifo_t *fifo, void *buffer);
  /**
   * @brief  pop data form FIFO Head.
   * @param  fifo  : FIFO Handler memory pointer.
   * @param  data  : Data out buffer pointer.
   * @return  0:FIFO is empty ; 1:Successful.
   */  
  void (*unsetFunc)(tool_fifo_t *fifo, uint8_t setting);
  void (*setFunc)(tool_fifo_t *fifo, uint8_t setting);
}tool_fifo_api_t;

/* *****************************************************************************************
 *    static inline Method
 */ 

/*-------------------------------------
 *  tool_fifo_flush 
 */ 
static inline void tool_fifo_flush(tool_fifo_t *fifo){  
  fifo->head = fifo->tail = 0;
}

/*-------------------------------------
 *  tool_fifo_getSize 
 */ 
static inline uint32_t tool_fifo_getSize(tool_fifo_t *fifo){
  return fifo->itemSz;
}

/*-------------------------------------
 *  tool_fifo_getCount 
 */ 
static inline uint32_t tool_fifo_getCount(tool_fifo_t *fifo){
  return fifo->count;
}

/*-------------------------------------
 *  tool_fifo_getFree 
 */ 
static inline uint32_t tool_fifo_getFree(tool_fifo_t *fifo){
  if(fifo->head>fifo->tail)
    return (fifo->count-1) - fifo->head + fifo->tail;
  
  else if(fifo->head<fifo->tail)
    return fifo->tail - fifo->head - 1;
  
  else
    return (fifo->count-1);  
}

/*-------------------------------------
 *  tool_fifo_getUsed 
 */ 
static inline uint32_t tool_fifo_getUsed(tool_fifo_t *fifo){
  return fifo->count - tool_fifo_getFree(fifo) - 1;
}

/*-------------------------------------
 *  tool_fifo_getStatus 
 */ 
static inline uint8_t tool_fifo_getStatus(tool_fifo_t *fifo){
  return fifo->config.status;
}

/*-------------------------------------
 *  tool_fifo_isFull 
 */ 
static inline bool tool_fifo_isFull(tool_fifo_t *fifo){
  return (tool_fifo_getFree(fifo)==0)?true:false;
}

/*-------------------------------------
 *  tool_fifo_isEmpty
 */ 
static inline bool tool_fifo_isEmpty(tool_fifo_t *fifo){
  return (tool_fifo_getFree(fifo))==(fifo->count-1)?true:false;
}

/* *****************************************************************************************
 *    Link API
 */ 
extern const tool_fifo_api_t tool_fifo_api;

/* *****************************************************************************************
 *    Method list
 */ 
bool tool_fifo_init(tool_fifo_t *fifo,  const tool_fifo_config_t *config);
tool_fifo_t* tool_fifo_initAddr(const tool_fifo_config_t *config);
tool_fifo_t* tool_fifo_reload(const tool_fifo_config_t *config);
bool tool_fifo_insert(tool_fifo_t *fifo, const void *data);
int tool_fifo_insertMulti(tool_fifo_t *fifo, const void *data, uint32_t count);
bool tool_fifo_insertTail(tool_fifo_t *fifo, const void *data);
bool tool_fifo_pop(tool_fifo_t *fifo, void *buffer);
int tool_fifo_popMulti(tool_fifo_t *fifo, void *buffer, uint32_t count);
bool tool_fifo_popHead(tool_fifo_t *fifo, void *buffer);
void tool_fifo_setFunc(tool_fifo_t *fifo, uint8_t setting);
void tool_fifo_unsetFunc(tool_fifo_t *fifo, uint8_t setting);

#ifdef __cplusplus
}
#endif
#endif
/* *****************************************************************************************
 *  End of file
 */
