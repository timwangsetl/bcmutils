/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Power Manager
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      PMErrors.h
*
* Description:    Power Manager Error Codes
*
* Revision History:
*
*   09/25/09  QHS  Pre-release
*   11/20/09  QHS  Ver 1.0 - Initial Release
*   07/02/10  QHS  Ver 1.2 - Add "Config Saved" info message
*   10/18/10  QHS  Change comment style from C++ to strict C.
*
*----------------------------------------------------------------------------*/

#define SUCCESS                  0      /* Success                          */

#define ERROR_PORT_INVALID      -1      /* Port number is not valid         */
#define ERROR_PWR_SUPLY_INVALID -2      /* Power supply number is not valid */
#define ERROR_PARAMETER_INVALID -3      /* Parameter is not valid           */

#define ERROR_RESOURCE_CREATE   -4      /* Can not create required resource */
#define ERROR_RESOURCE_INVALID  -5      /* Resource is not valid            */
#define ERROR_RESOURCE_CONFIG   -6      /* Can not configure resource       */
#define ERROR_RESOURCE_READ     -7      /* Can not read from resource       */
#define ERROR_RESOURCE_WRITE    -8      /* Can not write to resource        */
#define ERROR_RESOURCE_NOT_FND  -9      /* Can not find the resource        */

#define ERROR_CONFIG_LOAD       -10     /* Can not load configuration       */
#define ERROR_CONFIG_SAVE       -11     /* Can not save configuration       */
#define ERROR_CONFIG_INVALID    -12     /* Configuration data is invalid    */
#define ERROR_CONFIG_CORRUPT    -13     /* Configuration data is corrupt    */

#define ERROR_SYSTEM_OVERLOAD   -14     /* System overload                  */
#define ERROR_PORT_OVERLOAD     -15     /* Port overload                    */
#define ERROR_STARTUP_OVERLOAD  -16     /* Startup overload                 */

#define INFO_DEFAULTS_RESTORED   1      /* Restored to factory defaults     */
#define INFO_SYSTEM_RESET        2      /* System has been reset            */
#define INFO_CONFIG_SAVED        3      /* Configuration has been saved     */

