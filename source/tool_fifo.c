//*****************************************************************************
//*			File Name		:FIFO.c
//*			Create Date	:2018-09-09
//*			Modufy Date	:2019-06-17
//*			Information :
//*****************************************************************************
#include "tool_fifo.h"
#include "string.h" 
#include "ztool.h"

objectVersionDef(FIFO, versionDef(1,00,0000));

//*****************************************************************************
//*	Macro Function 
//*****************************************************************************
#define tool_fifo_RS(src, max) if(src<(max-1)) src++; else src=0;
#define tool_fifo_LS(src, max) if(src>(0)) src--; else src=(max-1);

#define tool_fifo_RSMult(src, max, count) if((max-src)>count) src+=count; else src=(count-(max-src))
#define tool_fifo_LSMult(src, max, count) if(src<(count)) src=(src+max-count); else src=(src-count)
//*****************************************************************************
//*	Private Function 
//*****************************************************************************
//-------------------------------------
//	tool_fifo_memcpy 
//------------------------------------- 
static void* tool_fifo_memcpy(void *dst, const void *src, uint32_t len){
  char *d = dst;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dst;
}

//*****************************************************************************
//*	Public Function 
//*****************************************************************************
//-------------------------------------
//	tool_fifo_flush 
//------------------------------------- 
void tool_fifo_flush(tool_fifo_t *fifo){	
	fifo->head = fifo->tail = 0;
}
//-------------------------------------
//	tool_fifo_getSize 
//------------------------------------- 
uint32_t tool_fifo_getSize(tool_fifo_t *fifo){
	return fifo->itemSz;
}

//-------------------------------------
//	tool_fifo_getCount 
//------------------------------------- 
uint32_t tool_fifo_getCount(tool_fifo_t *fifo){
	return fifo->count;
}

//-------------------------------------
//	tool_fifo_getFree 
//------------------------------------- 
uint32_t tool_fifo_getFree(tool_fifo_t *fifo){
	if(fifo->head>fifo->tail)
		return (fifo->count-1) - fifo->head + fifo->tail;
	
	else if(fifo->head<fifo->tail)
		return fifo->tail - fifo->head - 1;
	
	else
		return (fifo->count-1);	
}

//-------------------------------------
//	tool_fifo_getUsed
//------------------------------------- 
uint32_t tool_fifo_getUsed(tool_fifo_t *fifo){
	return fifo->count - tool_fifo_getFree(fifo) - 1;
	
}
//-------------------------------------
//	tool_fifo_getUsed
//------------------------------------- 
uint8_t tool_fifo_getStatus(tool_fifo_t *fifo){
	return fifo->config.status;
	
}
//-------------------------------------
//	tool_fifo_isFull 
//------------------------------------- 
bool tool_fifo_isFull(tool_fifo_t *fifo){
	return (tool_fifo_getFree(fifo)==0)?true:false;
}

//-------------------------------------
//	tool_fifo_isEmpty 
//------------------------------------- 
bool tool_fifo_isEmpty(tool_fifo_t *fifo){
	return (tool_fifo_getFree(fifo))==(fifo->count-1)?true:false;
}

//-------------------------------------
//	tool_fifo_init 
//------------------------------------- 
bool tool_fifo_init(tool_fifo_t *fifo,	const tool_fifo_config_t *config){
	memset(fifo, 0x00, sizeof(tool_fifo_t));
	fifo->data = config->buffer;
	fifo->count = config->count;
	fifo->itemSz = config->itemSize;
	fifo->head = fifo->tail = 0;
	
	return true;
}

//-------------------------------------
//	tool_fifo_initAddr 
//------------------------------------- 
tool_fifo_t* tool_fifo_initAddr(const tool_fifo_config_t *config){
	
	tool_fifo_t* fifo = (tool_fifo_t*)config->address;
	memset(fifo, 0x00, sizeof(tool_fifo_t));
	
	uint32_t temp = (uint32_t)config->address;
	temp += sizeof(tool_fifo_t);
	
	fifo->data = (void*)temp;

	fifo->count = ((config->bufferSize - sizeof(tool_fifo_t))/config->itemSize);
	fifo->itemSz = config->itemSize;
	fifo->head = fifo->tail = 0;
		
	return fifo;
}

tool_fifo_t* tool_fifo_reload(const tool_fifo_config_t *config){
	tool_fifo_t* fifo = (tool_fifo_t*)config->address;
	
	void* pData = (void*)((uint32_t)config->address + sizeof(tool_fifo_t));
	int count = ((config->bufferSize - sizeof(tool_fifo_t))/config->itemSize);
	
	if(fifo->data != pData)
		return (tool_fifo_t*)0x00000000;
		
	if(fifo->count != count)
		return (tool_fifo_t*)0x00000000;
		
	if(fifo->itemSz != config->itemSize)
		return (tool_fifo_t*)0x00000000;
		
	return fifo;
}

//-------------------------------------
//	tool_fifo_insert 
//------------------------------------- 
bool tool_fifo_insert(tool_fifo_t *fifo, const void *data){
	if ((tool_fifo_isFull(fifo) & isGBitLow(fifo->config.func, fifo_func_override))) 
		return false;	
	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexHead))
		return false;	
	setGBitHigh(fifo->config.status, fifo_status_isMutexHead);
	
	uint8_t *ptr = fifo->data;
	ptr += fifo->head * fifo->itemSz;
	tool_fifo_memcpy(ptr, data, fifo->itemSz);
	tool_fifo_RS(fifo->head, fifo->count);		

	if(tool_fifo_isFull(fifo)){
		tool_fifo_RS(fifo->tail, fifo->count);
	}
	
	//UnsetMutexInsert
	setGBitLow(fifo->config.status, fifo_status_isMutexHead);
	return true;
}

//-------------------------------------
//	tool_fifo_insertMulti
//------------------------------------- 
int tool_fifo_insertMulti(tool_fifo_t *fifo, const void *data, uint32_t count){
	if ((tool_fifo_isFull(fifo) & isGBitLow(fifo->config.func, fifo_func_override))) 
		return count;	
	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexHead))
		return count;
	setGBitHigh(fifo->config.status, fifo_status_isMutexHead);	
	
	uint8_t *ptr;
	uint32_t headToEnd = fifo->count - fifo->head;
	uint32_t overflowCount = 0;
	if(tool_fifo_getFree(fifo)<count)
		overflowCount = count - tool_fifo_getFree(fifo);
	
	//start copy
	ptr = ((uint8_t*)fifo->data) + (fifo->head * fifo->itemSz);
	
	if(isGBitLow(fifo->config.func, fifo_func_override)){
		//func override is disable
		if((count - overflowCount) > headToEnd){
			//need segmentation
			
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * headToEnd));
			tool_fifo_RSMult(fifo->head, fifo->count, headToEnd);	

			ptr = ((uint8_t*)fifo->data) + (fifo->head * fifo->itemSz);
			data = ((uint8_t*)data) + (headToEnd * fifo->itemSz);
			
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * (count-(headToEnd + overflowCount))));
			tool_fifo_RSMult(fifo->head, fifo->count, (count-(headToEnd + overflowCount)));
		}
		else{
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * (count - overflowCount)));
			tool_fifo_RSMult(fifo->head, fifo->count, (count - overflowCount));			
		}
		
		setGBitLow(fifo->config.status, fifo_status_isMutexHead);
		return overflowCount;
	}
	else{
		//func override is enable
		if(count > headToEnd){
			//need segmentation
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * headToEnd));
			tool_fifo_RSMult(fifo->head, fifo->count, headToEnd);	

			ptr = ((uint8_t*)fifo->data) + (fifo->head * fifo->itemSz);
			data = ((uint8_t*)data) + (headToEnd * fifo->itemSz);			
			
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * (count - headToEnd)));
			tool_fifo_RSMult(fifo->head, fifo->count, (count - headToEnd));		
			
			tool_fifo_RSMult(fifo->tail, fifo->count, overflowCount);
		}
		else{
			tool_fifo_memcpy(ptr, data, (fifo->itemSz * count));
			tool_fifo_RSMult(fifo->head, fifo->count, count);			
			
			tool_fifo_RSMult(fifo->tail, fifo->count, overflowCount);
		}
		
		setGBitLow(fifo->config.status, fifo_status_isMutexHead);
		return 0;
	}
}

//-------------------------------------
//	tool_fifo_insertTail 
//------------------------------------- 
bool tool_fifo_insertTail(tool_fifo_t *fifo, const void *data){
	if ((tool_fifo_isFull(fifo) & isGBitLow(fifo->config.func, fifo_func_override))) 
		return false;	
	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexTail))
		return false;
	setGBitHigh(fifo->config.status, fifo_status_isMutexTail);	
	
	uint8_t *ptr = fifo->data;
	tool_fifo_LS(fifo->tail, fifo->count);
	ptr += fifo->tail * fifo->itemSz;
	tool_fifo_memcpy(ptr, data, fifo->itemSz);

	if(tool_fifo_isFull(fifo)){
		tool_fifo_LS(fifo->head, fifo->count);
	}
		
	setGBitLow(fifo->config.status, fifo_status_isMutexTail);
	return true;
}

//-------------------------------------
//	tool_fifo_pop 
//------------------------------------- 
bool tool_fifo_pop(tool_fifo_t *fifo, void *buffer){
	if (tool_fifo_isEmpty(fifo)) 
		return false;	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexTail))
		return false;
	setGBitHigh(fifo->config.status, fifo_status_isMutexTail);		
	
	uint8_t *ptr = fifo->data;
	ptr += fifo->tail * fifo->itemSz;
	tool_fifo_memcpy(buffer, ptr, fifo->itemSz);
	tool_fifo_RS(fifo->tail, fifo->count);

	setGBitLow(fifo->config.status, fifo_status_isMutexTail);	
	return true;
}

//-------------------------------------
//	tool_fifo_popMulti 
//------------------------------------- 
int tool_fifo_popMulti(tool_fifo_t *fifo, void *buffer, uint32_t count){
	if (tool_fifo_isEmpty(fifo)) 
		return 0;	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexTail))
		return 0;
	setGBitHigh(fifo->config.status, fifo_status_isMutexTail);
	
	uint8_t *ptr;
	uint32_t tailToEnd = fifo->count - fifo->tail;
	uint32_t rCount;
	
	if(tool_fifo_getUsed(fifo)>count)
		rCount = count;
	else
		rCount = tool_fifo_getUsed(fifo);
	
	ptr = ((uint8_t*)fifo->data) + (fifo->tail * fifo->itemSz);
	
	
	if(rCount > tailToEnd){
		tool_fifo_memcpy(buffer, ptr, (tailToEnd * fifo->itemSz));
		tool_fifo_RSMult(fifo->tail, fifo->count, tailToEnd);	
		
		ptr = ((uint8_t*)fifo->data) + (fifo->tail * fifo->itemSz);
		buffer = ((uint8_t*)buffer) + (tailToEnd * fifo->itemSz);			
		
		tool_fifo_memcpy(buffer, ptr, ((rCount - tailToEnd) * fifo->itemSz));
		tool_fifo_RSMult(fifo->tail, fifo->count, (rCount - tailToEnd));		
	
	}
	else{
		
		tool_fifo_memcpy(buffer, ptr, (rCount * fifo->itemSz));
		tool_fifo_RSMult(fifo->tail, fifo->count, rCount);
	}

	setGBitLow(fifo->config.status, fifo_status_isMutexTail);
	return rCount;
}

//-------------------------------------
//	tool_fifo_popHead
//------------------------------------- 
bool tool_fifo_popHead(tool_fifo_t *fifo, void *buffer){
	if (tool_fifo_isEmpty(fifo)) 
		return false;	
	//SetMutexInsert
	if(isGBitHigh(fifo->config.func, fifo_func_mutex) & isGBitHigh(fifo->config.status, fifo_status_isMutexHead))
		return false;
	setGBitHigh(fifo->config.status, fifo_status_isMutexHead);	
	
	uint8_t *ptr = fifo->data;
	
	tool_fifo_LS(fifo->head, fifo->count);
	ptr += fifo->head * fifo->itemSz;
	tool_fifo_memcpy(buffer, ptr, fifo->itemSz);

	setGBitLow(fifo->config.status, fifo_status_isMutexHead);		
	return true;
}

//-------------------------------------
//	tool_fifo_setFunc
//------------------------------------- 
void tool_fifo_setFunc(tool_fifo_t *fifo, uint8_t setting){
	setGBitHigh(fifo->config.func, setting);
}

//-------------------------------------
//	tool_fifo_unsetFunc
//------------------------------------- 
void tool_fifo_unsetFunc(tool_fifo_t *fifo, uint8_t setting){
	setGBitLow(fifo->config.func, setting);
}

const tool_fifo_api_t tool_fifo_api = {
	.flush 				= tool_fifo_flush,
	.getSize 			= tool_fifo_getSize,
	.getCount			= tool_fifo_getCount,
	.getFree 			= tool_fifo_getFree,
	.getUsed			= tool_fifo_getUsed,
	.getStatus		= tool_fifo_getStatus,
	.isFull 			= tool_fifo_isFull,
	.isEmpty 			= tool_fifo_isEmpty,
	.init 				= tool_fifo_init,
	.initAddr			= tool_fifo_initAddr,
	.reload				= tool_fifo_reload,
	.insert 			= tool_fifo_insert,
	.insertMulti	= tool_fifo_insertMulti,
	.insertTail		= tool_fifo_insertTail,
	.pop 					= tool_fifo_pop,
	.popMulti			= tool_fifo_popMulti,
	.popHead			= tool_fifo_popHead,
	.setFunc			= tool_fifo_setFunc,
	.unsetFunc		= tool_fifo_unsetFunc,
};

//*****************************************************************************
//*	End of file
//*****************************************************************************

