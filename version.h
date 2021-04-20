/* *****************************************************************************************
 *      File Name    :version.h
 *      Create Date  :2021-04-20
 *      Modufy Date  :
 *      Information  :
 */
#ifndef version_H_
#define version_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* *****************************************************************************************
 *    Macro
 */ 
#define VERSION_DEFINEE(major, minor, revision) (((major<<24)&0xFF000000) | ((minor<<16)&0x00FF0000) | ((revision<<0)&0x0000FFFF))

#define VERSION_GET_MAJOR(version) ((version >> 24) & 0x000000FF)
#define VERSION_GET_MINOR(version) ((version >> 16) & 0x000000FF)
#define VERSION_GET_REVISION(version) ((version >>  0) & 0x0000FFFF)

#define VERSION_CHECK_COMPATIBLE(target, major, minor) ((target & 0xFFFF0000) ^ VERSION_DEFINEE(major, minor, 0))

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //version_H_
/* *****************************************************************************************
 *  End of file
 */
