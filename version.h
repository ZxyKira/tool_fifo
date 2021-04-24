/* *****************************************************************************************
 *      File Name    :version.h
 *      Create Date  :2021-04-20
 *      Modufy Date  :2021-04-24
 *      Information  :
 */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* *****************************************************************************************
 *    Macro
 */ 

/*----------------------------------------
 *  VERSION_DEFINEE
 *----------------------------------------*/
#ifndef VERSION_DEFINEE
#define VERSION_DEFINEE(major, minor, revision) (((major<<24)&0xFF000000) | ((minor<<16)&0x00FF0000) | ((revision<<0)&0x0000FFFF))
#endif



/*----------------------------------------
 *  VERSION_GET_MAJOR
 *----------------------------------------*/
#ifndef VERSION_GET_MAJOR
#define VERSION_GET_MAJOR(version) ((version >> 24) & 0x000000FF)
#endif



/*----------------------------------------
 *  VERSION_GET_MINOR
 *----------------------------------------*/
#ifndef VERSION_GET_MINOR
#define VERSION_GET_MINOR(version) ((version >> 16) & 0x000000FF)
#endif



/*----------------------------------------
 *  VERSION_GET_REVISION
 *----------------------------------------*/
#ifndef VERSION_GET_REVISION
#define VERSION_GET_REVISION(version) ((version >>  0) & 0x0000FFFF)
#endif



/*----------------------------------------
 *  VERSION_CHECK_COMPATIBLE
 *----------------------------------------*/
#ifndef VERSION_CHECK_COMPATIBLE
#define VERSION_CHECK_COMPATIBLE(target, min) (((VERSION_GET_MAJOR(target) ^ VERSION_GET_MAJOR(min)) | (VERSION_GET_MINOR(min) > VERSION_GET_MINOR(target)))?2:(min>target))
#endif



/* *****************************************************************************************
 *    Typedef List
 */ 

/* *****************************************************************************************
 *    Typedef Function
 */ 

/* *****************************************************************************************
 *    Struct/Union/Enum
 */ 

/* *****************************************************************************************
 *    Typedef Struct/Union/Enum
 */ 

/* *****************************************************************************************
 *    Inline Function
 */ 



#ifdef __cplusplus
}
#endif //__cplusplus
/* *****************************************************************************************
 *  End of file
 */
